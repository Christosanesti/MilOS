#include "layouttemplatemanager_dbus.h"
#include "milos/logging/logger.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QUuid>
#include <QStandardPaths>
#include <QDir>

LayoutTemplateManagerDBus::LayoutTemplateManagerDBus(LayoutTemplateManager* manager, QObject* parent)
    : QObject(parent)
    , m_manager(manager)
    , m_windowPositioner(new WindowPositioner(this))
    , m_windowGrouper(new WindowGrouper(this))
    , m_xenonCoordinator(new XenonCoordinator(this))
    , m_layoutRestorer(new LayoutRestorer(this))
    , m_running(false)
    , m_initialized(false)
{
}

LayoutTemplateManagerDBus::~LayoutTemplateManagerDBus()
{
    stop();
}

bool LayoutTemplateManagerDBus::initialize()
{
    if (m_initialized) {
        return true;
    }

    if (!m_manager) {
        qWarning() << "LayoutTemplateManager instance not provided";
        return false;
    }

    m_initialized = true;
    return true;
}

bool LayoutTemplateManagerDBus::start()
{
    if (!m_initialized) {
        qWarning() << "D-Bus interface not initialized";
        return false;
    }

    if (m_running) {
        return true;
    }

    if (!registerInterface()) {
        qWarning() << "Failed to register D-Bus interface";
        return false;
    }

    m_running = true;
    LOG_INFO("Layout Template Manager D-Bus interface started");
    return true;
}

void LayoutTemplateManagerDBus::stop()
{
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    qDebug() << "Layout Template Manager D-Bus interface stopped";
}

bool LayoutTemplateManagerDBus::registerInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }

    // Register service
    if (!connection.registerService("org.milos.LayoutTemplateManager")) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus service:" << error.message();
        }
        return false;
    }

    // Register object
    if (!connection.registerObject("/org/milos/LayoutTemplateManager", this,
                                   QDBusConnection::ExportAllSlots |
                                   QDBusConnection::ExportAllSignals)) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus object:" << error.message();
            connection.unregisterService("org.milos.LayoutTemplateManager");
            return false;
        }
    }

    return true;
}

void LayoutTemplateManagerDBus::unregisterInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject("/org/milos/LayoutTemplateManager");
    connection.unregisterService("org.milos.LayoutTemplateManager");
}

QString LayoutTemplateManagerDBus::CreateTemplate(const QVariantMap& templateData)
{
    if (!m_manager) {
        return QString();
    }

    // Generate template ID if not provided
    QVariantMap templateCopy = templateData;
    if (!templateCopy.contains("id") || templateCopy.value("id").toString().isEmpty()) {
        QString name = templateCopy.value("name").toString();
        QString id = QString("%1_%2").arg(name.toLower().replace(" ", "_")).arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8));
        templateCopy["id"] = id;
    }

    // Validate template
    if (!m_manager->validateTemplate(templateCopy)) {
        emit TemplateValidationFailed(templateCopy.value("id").toString(), QStringList() << "Template validation failed");
        return QString();
    }

    // Save template
    m_manager->saveTemplate(templateCopy);
    return templateCopy.value("id").toString();
}

bool LayoutTemplateManagerDBus::ApplyTemplate(const QString& templateId)
{
    if (!m_manager) {
        return false;
    }

    QVariantMap template = m_manager->getTemplate(templateId);
    if (template.isEmpty()) {
        emit TemplateValidationFailed(templateId, QStringList() << "Template not found");
        return false;
    }

    // Save current layout before applying new template
    m_layoutRestorer->saveCurrentLayout();

    // Position windows
    bool positionSuccess = m_windowPositioner->positionWindows(template);
    if (!positionSuccess) {
        emit TemplateApplied(templateId, "partial_success");
    }

    // Group windows
    bool groupSuccess = m_windowGrouper->groupWindows(template);
    if (!groupSuccess) {
        emit TemplateApplied(templateId, "partial_success");
    }

    // Coordinate xenon effects
    m_xenonCoordinator->synchronizeEffects();

    // Mark template as applied
    m_manager->applyTemplate(templateId);
    emit TemplateApplied(templateId, positionSuccess && groupSuccess ? "success" : "partial_success");
    return true;
}

bool LayoutTemplateManagerDBus::RestoreLayout()
{
    if (!m_manager || !m_layoutRestorer) {
        return false;
    }

    // Load last saved layout
    QString storagePath = m_layoutRestorer->getLayoutStoragePath();
    // Find most recent layout file
    QDir dir(storagePath);
    QStringList filters;
    filters << "*.json";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);
    
    if (files.isEmpty()) {
        emit TemplateApplied("", "no_saved_layout");
        return false;
    }

    // Load and restore most recent layout
    QFile file(files.first().absoluteFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QVariantMap layoutData = doc.object().toVariantMap();
        
        bool success = m_layoutRestorer->restoreLayout(layoutData);
        emit TemplateApplied("", success ? "restored" : "restore_failed");
        return success;
    }

    emit TemplateApplied("", "restore_failed");
    return false;
}

QVariantList LayoutTemplateManagerDBus::ListTemplates()
{
    if (!m_manager) {
        return QVariantList();
    }

    m_manager->loadTemplates();
    return m_manager->templates();
}

QVariantMap LayoutTemplateManagerDBus::GetTemplate(const QString& templateId)
{
    if (!m_manager) {
        return QVariantMap();
    }

    return m_manager->getTemplate(templateId);
}

QVariantMap LayoutTemplateManagerDBus::ValidateTemplate(const QString& templateId, const QVariantMap& monitorConfig)
{
    if (!m_manager) {
        QVariantMap result;
        result["valid"] = false;
        result["errors"] = QStringList() << "Manager not available";
        return result;
    }

    QVariantMap template = m_manager->getTemplate(templateId);
    if (template.isEmpty()) {
        QVariantMap result;
        result["valid"] = false;
        result["errors"] = QStringList() << "Template not found";
        return result;
    }

    // Validate monitor configuration
    int requiredMonitors = template.value("monitors").toInt();
    int availableMonitors = monitorConfig.value("count", 1).toInt();

    QVariantMap result;
    QStringList errors;

    if (availableMonitors < requiredMonitors) {
        errors << QString("Template requires %1 monitors, but only %2 available").arg(requiredMonitors).arg(availableMonitors);
    }

    result["valid"] = errors.isEmpty();
    result["errors"] = errors;
    result["warnings"] = QVariantList();

    if (!errors.isEmpty()) {
        emit TemplateValidationFailed(templateId, errors);
    }

    return result;
}


