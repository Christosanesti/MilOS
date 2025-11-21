#include "image_generation_service.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>

ImageGenerationService::ImageGenerationService(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_available(false)
{
    // Find wrapper script
    m_wrapperPath = findWrapperScript();
    
    // Set output directory
    m_outputDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/milos-images";
    QDir().mkpath(m_outputDirectory);
    
    // Check availability
    m_available = !m_wrapperPath.isEmpty() && QFileInfo::exists(m_wrapperPath);
    
    if (m_available) {
        // Register D-Bus service
        if (!QDBusConnection::systemBus().registerService("org.milos.ImageGeneration")) {
            qWarning() << "Failed to register D-Bus service org.milos.ImageGeneration";
        }
        
        if (!QDBusConnection::systemBus().registerObject("/org/milos/ImageGeneration", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
            qWarning() << "Failed to register D-Bus object /org/milos/ImageGeneration";
        }
        
        qDebug() << "Image Generation Service initialized";
    } else {
        qWarning() << "Image Generation Service not available - wrapper script not found";
    }
}

ImageGenerationService::~ImageGenerationService()
{
    if (m_process) {
        m_process->kill();
        m_process->deleteLater();
    }
}

QString ImageGenerationService::findWrapperScript() const
{
    // Check common locations
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/../lib/milos/image-generation-wrapper.js",
        "/usr/lib/milos/image-generation-wrapper.js",
        "/opt/milos/lib/image-generation-wrapper.js",
        QDir::homePath() + "/.local/lib/milos/image-generation-wrapper.js"
    };
    
    for (const QString &path : searchPaths) {
        if (QFileInfo::exists(path)) {
            return path;
        }
    }
    
    return QString();
}

QString ImageGenerationService::callWrapper(const QString &command, const QStringList &args)
{
    if (!m_available) {
        emit errorOccurred("Service not available - wrapper script not found");
        return QString();
    }
    
    if (m_process && m_process->state() != QProcess::NotRunning) {
        emit errorOccurred("Another operation is in progress");
        return QString();
    }
    
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &ImageGenerationService::onProcessFinished);
        connect(m_process, &QProcess::errorOccurred,
                this, &ImageGenerationService::onProcessError);
    }
    
    QStringList fullArgs;
    fullArgs << m_wrapperPath << command << args;
    
    m_process->start("node", fullArgs);
    
    if (!m_process->waitForFinished(300000)) { // 5 minute timeout
        emit errorOccurred("Operation timed out");
        return QString();
    }
    
    QByteArray output = m_process->readAllStandardOutput();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(output, &error);
    
    if (error.error != QJsonParseError::NoError) {
        emit errorOccurred(QString("Failed to parse response: %1").arg(error.errorString()));
        return QString();
    }
    
    QJsonObject obj = doc.object();
    if (!obj.value("success").toBool()) {
        QString errorMsg = obj.value("error").toString();
        emit errorOccurred(errorMsg);
        return QString();
    }
    
    QJsonObject result = obj.value("result").toObject();
    QString imageUrl = result.value("imageUrl").toString();
    
    // Download and save image if URL provided
    if (!imageUrl.isEmpty()) {
        // TODO: Download image from URL and save to m_outputDirectory
        QString imagePath = m_outputDirectory + "/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
        return imagePath;
    }
    
    return QString();
}

QString ImageGenerationService::generateImage(const QString &prompt, const QVariantMap &options)
{
    QStringList args;
    args << prompt;
    
    // Add options if provided
    if (options.contains("styles")) {
        args << "--styles" << options.value("styles").toString();
    }
    if (options.contains("count")) {
        args << "--count" << QString::number(options.value("count").toInt());
    }
    
    QString imagePath = callWrapper("generate", args);
    
    if (!imagePath.isEmpty()) {
        emit imageGenerated(imagePath);
    }
    
    return imagePath;
}

QString ImageGenerationService::editImage(const QString &imagePath, const QString &editPrompt, const QVariantMap &options)
{
    QStringList args;
    args << imagePath << editPrompt;
    
    QString resultPath = callWrapper("edit", args);
    
    if (!resultPath.isEmpty()) {
        emit imageEdited(resultPath);
    }
    
    return resultPath;
}

QString ImageGenerationService::restoreImage(const QString &imagePath, const QVariantMap &options)
{
    QStringList args;
    args << imagePath;
    
    QString resultPath = callWrapper("restore", args);
    
    if (!resultPath.isEmpty()) {
        emit imageRestored(resultPath);
    }
    
    return resultPath;
}

bool ImageGenerationService::isAvailable() const
{
    return m_available;
}

QString ImageGenerationService::getStatus() const
{
    if (!m_available) {
        return "unavailable - wrapper script not found";
    }
    
    if (m_process && m_process->state() != QProcess::NotRunning) {
        return "busy";
    }
    
    return "ready";
}

void ImageGenerationService::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString error = m_process->readAllStandardError();
        emit errorOccurred(QString("Process failed: %1").arg(error));
    }
}

void ImageGenerationService::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start process";
            break;
        case QProcess::Crashed:
            errorMsg = "Process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Process timed out";
            break;
        default:
            errorMsg = "Unknown process error";
    }
    
    emit errorOccurred(errorMsg);
}

