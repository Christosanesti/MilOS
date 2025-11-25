#include "quickactionsservice.h"
#include <QDBusReply>
#include <QDebug>
#include <QWindow>
#include <QGuiApplication>
#include <QStandardPaths>
#include <QSettings>

QuickActionsService::QuickActionsService(QObject *parent)
    : QObject(parent)
    , m_currentContext("")
    , m_enabled(true)
    , m_contextTimer(new QTimer(this))
    , m_dbusConnection(QDBusConnection::sessionBus())
    , m_quickActionsInterface(nullptr)
{
    // Setup context update timer (check every 500ms)
    m_contextTimer->setInterval(500);
    m_contextTimer->setSingleShot(false);
    connect(m_contextTimer, &QTimer::timeout, this, &QuickActionsService::updateContext);
    
    // Try to connect to quick actions D-Bus service
    m_quickActionsInterface = new QDBusInterface("org.milos.QuickActions",
                                                 "/org/milos/QuickActions",
                                                 "org.milos.QuickActions",
                                                 m_dbusConnection,
                                                 this);
    
    if (!m_quickActionsInterface->isValid()) {
        qWarning() << "Quick Actions D-Bus service not available, using fallback context detection";
    }
    
    // Load configuration
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/quick-actions.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    m_enabled = settings.value("enabled", true).toBool();
    settings.endGroup();
    
    // Start context detection
    if (m_enabled) {
        m_contextTimer->start();
        updateContext();
    }
}

QuickActionsService::~QuickActionsService()
{
}

void QuickActionsService::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        
        // Save to config
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/quick-actions.ini", QSettings::IniFormat);
        settings.beginGroup("General");
        settings.setValue("enabled", enabled);
        settings.endGroup();
        
        if (enabled) {
            m_contextTimer->start();
            updateContext();
        } else {
            m_contextTimer->stop();
            m_currentContext = "";
            m_availableActions.clear();
            emit currentContextChanged("");
            emit availableActionsChanged();
        }
        
        emit enabledChanged(enabled);
    }
}

void QuickActionsService::executeAction(const QString &actionId)
{
    if (!m_enabled) {
        emit actionError("Quick actions are disabled");
        return;
    }
    
    // Find action in available actions
    for (const QVariant &actionVar : m_availableActions) {
        QVariantMap action = actionVar.toMap();
        if (action["id"].toString() == actionId) {
            QString service = action["service"].toString();
            QString path = action["path"].toString();
            QString interface = action["interface"].toString();
            QString method = action["method"].toString();
            QVariantList args = action["args"].toList();
            
            bool success = callToolDBusMethod(service, path, interface, method, args);
            emit actionExecuted(actionId, success);
            
            if (!success) {
                emit actionError(QString("Failed to execute action: %1").arg(actionId));
            }
            return;
        }
    }
    
    emit actionError(QString("Action not found: %1").arg(actionId));
}

void QuickActionsService::refreshContext()
{
    updateContext();
}

QVariantMap QuickActionsService::getActionInfo(const QString &actionId)
{
    for (const QVariant &actionVar : m_availableActions) {
        QVariantMap action = actionVar.toMap();
        if (action["id"].toString() == actionId) {
            return action;
        }
    }
    return QVariantMap();
}

void QuickActionsService::updateContext()
{
    QString newContext = detectApplicationContext();
    
    if (newContext != m_currentContext) {
        m_currentContext = newContext;
        emit currentContextChanged(newContext);
        updateAvailableActions();
    }
}

void QuickActionsService::onActiveWindowChanged()
{
    updateContext();
}

QString QuickActionsService::detectApplicationContext()
{
    // Try D-Bus service first
    if (m_quickActionsInterface && m_quickActionsInterface->isValid()) {
        QDBusReply<QString> reply = m_quickActionsInterface->call("GetCurrentContext");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    
    // Fallback: Detect from active window
    QWindow *activeWindow = QGuiApplication::focusWindow();
    if (!activeWindow) {
        return "";
    }
    
    QString windowTitle = activeWindow->title().toLower();
    QString className = activeWindow->metaObject()->className();
    
    // Map window titles/classes to contexts
    if (windowTitle.contains("encryption") || className.contains("Encryption")) {
        return "encryption_manager";
    } else if (windowTitle.contains("network") || className.contains("Network")) {
        return "network_dashboard";
    } else if (windowTitle.contains("anonymity") || className.contains("Anonymity")) {
        return "anonymity_controller";
    } else if (windowTitle.contains("backup") || className.contains("Backup")) {
        return "backup_system";
    } else if (windowTitle.contains("clipboard") || className.contains("Clipboard")) {
        return "clipboard_manager";
    }
    
    return "";
}

void QuickActionsService::updateAvailableActions()
{
    m_availableActions.clear();
    
    if (!m_enabled || m_currentContext.isEmpty()) {
        emit availableActionsChanged();
        return;
    }
    
    // Try D-Bus service first
    if (m_quickActionsInterface && m_quickActionsInterface->isValid()) {
        QDBusReply<QVariantList> reply = m_quickActionsInterface->call("GetAvailableActions", m_currentContext);
        if (reply.isValid()) {
            m_availableActions = reply.value();
            emit availableActionsChanged();
            return;
        }
    }
    
    // Fallback: Define actions per context
    if (m_currentContext == "encryption_manager") {
        QVariantMap encryptAction;
        encryptAction["id"] = "encrypt_file";
        encryptAction["label"] = "Encrypt File";
        encryptAction["icon"] = "document-encrypt";
        encryptAction["service"] = "org.milos.EncryptionManager";
        encryptAction["path"] = "/org/milos/EncryptionManager";
        encryptAction["interface"] = "org.milos.EncryptionManager";
        encryptAction["method"] = "EncryptFile";
        encryptAction["priority"] = "primary";
        m_availableActions.append(encryptAction);
        
        QVariantMap decryptAction;
        decryptAction["id"] = "decrypt_file";
        decryptAction["label"] = "Decrypt File";
        decryptAction["icon"] = "document-decrypt";
        decryptAction["service"] = "org.milos.EncryptionManager";
        decryptAction["path"] = "/org/milos/EncryptionManager";
        decryptAction["interface"] = "org.milos.EncryptionManager";
        decryptAction["method"] = "DecryptFile";
        decryptAction["priority"] = "secondary";
        m_availableActions.append(decryptAction);
    } else if (m_currentContext == "network_dashboard") {
        QVariantMap startCaptureAction;
        startCaptureAction["id"] = "start_capture";
        startCaptureAction["label"] = "Start Capture";
        startCaptureAction["icon"] = "media-record";
        startCaptureAction["service"] = "org.milos.NetworkDashboard";
        startCaptureAction["path"] = "/org/milos/NetworkDashboard";
        startCaptureAction["interface"] = "org.milos.NetworkDashboard";
        startCaptureAction["method"] = "StartCapture";
        startCaptureAction["priority"] = "primary";
        m_availableActions.append(startCaptureAction);
        
        QVariantMap stopCaptureAction;
        stopCaptureAction["id"] = "stop_capture";
        stopCaptureAction["label"] = "Stop Capture";
        stopCaptureAction["icon"] = "media-playback-stop";
        stopCaptureAction["service"] = "org.milos.NetworkDashboard";
        stopCaptureAction["path"] = "/org/milos/NetworkDashboard";
        stopCaptureAction["interface"] = "org.milos.NetworkDashboard";
        stopCaptureAction["method"] = "StopCapture";
        stopCaptureAction["priority"] = "secondary";
        m_availableActions.append(stopCaptureAction);
    }
    
    emit availableActionsChanged();
}

bool QuickActionsService::callToolDBusMethod(const QString &service, const QString &path,
                                            const QString &interface, const QString &method,
                                            const QVariantList &args)
{
    QDBusInterface toolInterface(service, path, interface, m_dbusConnection, this);
    
    if (!toolInterface.isValid()) {
        qWarning() << "Invalid D-Bus interface:" << service << path << interface;
        return false;
    }
    
    QDBusMessage message = toolInterface.call(method, args);
    
    if (message.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "D-Bus call failed:" << message.errorMessage();
        return false;
    }
    
    return true;
}

