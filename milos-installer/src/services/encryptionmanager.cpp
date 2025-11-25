#include "encryptionmanager.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <QRegularExpression>

EncryptionManager::EncryptionManager(QObject *parent)
    : QObject(parent)
    , m_encryptionInProgress(false)
    , m_encryptionProgress(0.0)
    , m_process(nullptr)
    , m_devicePath("")
{
}

void EncryptionManager::setupEncryption(const QString &password, const QString &algorithm)
{
    if (m_encryptionInProgress) {
        return;
    }
    
    if (!validateDiskSpace()) {
        emit encryptionError("Insufficient disk space for encryption");
        return;
    }
    
    m_encryptionInProgress = true;
    m_encryptionProgress = 0.0;
    emit encryptionInProgressChanged();
    emit encryptionProgressChanged();
    
    // Implement actual LUKS2 encryption setup using cryptsetup
    // Note: In a real installer, the device path would be passed as a parameter
    // For now, we'll use a default or detect the root partition
    
    if (m_devicePath.isEmpty()) {
        // Try to detect root partition
        QProcess dfProcess;
        dfProcess.start("df", QStringList() << "/" << "--output=source");
        dfProcess.waitForFinished(1000);
        QString output = dfProcess.readAllStandardOutput();
        QStringList lines = output.split("\n");
        if (lines.size() > 1) {
            m_devicePath = lines[1].trimmed();
        } else {
            emit encryptionError("Could not determine device to encrypt");
            m_encryptionInProgress = false;
            emit encryptionInProgressChanged();
            return;
        }
    }
    
    // Determine encryption algorithm (default to aes-xts-plain64 if not specified)
    QString algo = algorithm.isEmpty() ? "aes-xts-plain64" : algorithm;
    
    // Create LUKS2 container using cryptsetup
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
            QString output = m_process->readAllStandardOutput();
            // Parse progress from cryptsetup output if available
            if (output.contains("%")) {
                // Extract percentage
                QRegularExpression regex(R"((\d+)%)");
                QRegularExpressionMatch match = regex.match(output);
                if (match.hasMatch()) {
                    m_encryptionProgress = match.captured(1).toDouble();
                    emit encryptionProgressChanged();
                }
            }
        });
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                m_encryptionProgress = 100.0;
                emit encryptionProgressChanged();
                m_encryptionInProgress = false;
                emit encryptionInProgressChanged();
                emit encryptionComplete();
            } else {
                QString error = m_process->readAllStandardError();
                emit encryptionError(QString("Encryption failed: %1").arg(error));
                m_encryptionInProgress = false;
                m_encryptionProgress = 0.0;
                emit encryptionInProgressChanged();
                emit encryptionProgressChanged();
            }
        });
    }
    
    // Format device with LUKS2
    // cryptsetup luksFormat --type luks2 --cipher <algorithm> <device>
    QStringList args;
    args << "luksFormat";
    args << "--type" << "luks2";
    args << "--cipher" << algo;
    args << "--key-size" << "512"; // 512 bits for XTS mode
    args << "--hash" << "sha512";
    args << "--iter-time" << "5000"; // 5 seconds for key derivation
    args << "--use-urandom"; // Use /dev/urandom for key material
    args << "--batch-mode"; // Non-interactive
    args << m_devicePath;
    
    m_process->start("cryptsetup", args);
    
    // Send password to cryptsetup
    if (m_process->waitForStarted(5000)) {
        m_process->write((password + "\n").toLocal8Bit());
        m_process->write((password + "\n").toLocal8Bit()); // Confirm
        m_process->closeWriteChannel();
    } else {
        emit encryptionError("Failed to start cryptsetup");
        m_encryptionInProgress = false;
        emit encryptionInProgressChanged();
    }
    
    // Simulate progress updates (cryptsetup doesn't always provide progress)
    QTimer *progressTimer = new QTimer(this);
    connect(progressTimer, &QTimer::timeout, [this, progressTimer]() {
        if (m_encryptionInProgress && m_encryptionProgress < 90.0) {
            m_encryptionProgress += 5.0;
            emit encryptionProgressChanged();
        } else {
            progressTimer->stop();
            progressTimer->deleteLater();
        }
    });
    progressTimer->start(2000); // Update every 2 seconds
}

void EncryptionManager::cancelEncryption()
{
    if (!m_encryptionInProgress) {
        return;
    }
    
    // Cancel ongoing encryption operation
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->kill();
        m_process->waitForFinished(5000);
    }
    
    m_encryptionInProgress = false;
    m_encryptionProgress = 0.0;
    emit encryptionInProgressChanged();
    emit encryptionProgressChanged();
}

bool EncryptionManager::validateDiskSpace()
{
    // Check available disk space before encryption
    // Encryption requires additional space for LUKS2 header (~16MB) and metadata
    
    QProcess process;
    process.start("df", QStringList() << "-BG" << "/");
    process.waitForFinished(1000);
    QString output = process.readAllStandardOutput();
    
    // Parse available space
    QRegularExpression regex(R"(\s+(\d+)G\s+\d+G)");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        int availableGB = match.captured(1).toInt();
        // Require at least 1GB free for LUKS2 header and overhead
        if (availableGB < 1) {
            return false;
        }
    } else {
        // If parsing fails, check using statvfs or similar
        // For now, return true as fallback
        return true;
    }
    
    return true;
}

