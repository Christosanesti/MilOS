#include "actiondetector.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

ActionDetector::ActionDetector(QObject* parent)
    : QObject(parent)
    , m_currentWindowId("")
    , m_currentApplicationName("")
{
    // Monitor active window changes via KWin
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.connect("org.kde.KWin",
                      "/KWin",
                      "org.kde.KWin",
                      "activeWindowChanged",
                      this,
                      SLOT(onActiveWindowChanged()));
}

QVariantMap ActionDetector::getActiveWindowContext()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        return QVariantMap();
    }

    // Get active window
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "getActiveWindow"
    );

    QDBusReply<QString> reply = connection.call(message);
    if (!reply.isValid()) {
        return QVariantMap();
    }

    QString windowId = reply.value();
    QString applicationName = detectApplicationName(windowId);

    QVariantMap context;
    context["window_id"] = windowId;
    context["application_name"] = applicationName;
    context["tool_context"] = applicationName; // Simplified
    context["is_active"] = true;

    return context;
}

QVariantList ActionDetector::getAvailableActions(const QVariantMap& windowContext)
{
    QString applicationName = windowContext.value("application_name").toString();
    return getActionsForApplication(applicationName);
}

void ActionDetector::onActiveWindowChanged()
{
    QVariantMap context = getActiveWindowContext();
    QString windowId = context.value("window_id").toString();
    QString applicationName = context.value("application_name").toString();

    if (windowId != m_currentWindowId) {
        m_currentWindowId = windowId;
        m_currentApplicationName = applicationName;
        emit activeWindowChanged(windowId, applicationName);
    }
}

QString ActionDetector::detectApplicationName(const QString& windowId)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        return QString();
    }

    // Get window class/application name
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "getWindowClass"
    );
    message << windowId;

    QDBusReply<QString> reply = connection.call(message);
    if (reply.isValid()) {
        return reply.value();
    }

    return QString();
}

QVariantList ActionDetector::getActionsForApplication(const QString& applicationName)
{
    QVariantList actions;

    // Define actions for each security tool
    if (applicationName.contains("encryption", Qt::CaseInsensitive) ||
        applicationName.contains("encrypt", Qt::CaseInsensitive)) {
        QVariantMap encryptAction;
        encryptAction["action_id"] = "encrypt_file";
        encryptAction["action_name"] = "Encrypt File";
        encryptAction["tool_name"] = "milos-encryption-manager";
        encryptAction["action_type"] = "ENCRYPT_FILE";
        encryptAction["d_bus_interface"] = "org.milos.EncryptionManager";
        encryptAction["d_bus_method"] = "EncryptFile";
        actions.append(encryptAction);

        QVariantMap decryptAction;
        decryptAction["action_id"] = "decrypt_file";
        decryptAction["action_name"] = "Decrypt File";
        decryptAction["tool_name"] = "milos-encryption-manager";
        decryptAction["action_type"] = "DECRYPT_FILE";
        decryptAction["d_bus_interface"] = "org.milos.EncryptionManager";
        decryptAction["d_bus_method"] = "DecryptFile";
        actions.append(decryptAction);
    }

    if (applicationName.contains("network", Qt::CaseInsensitive)) {
        QVariantMap captureAction;
        captureAction["action_id"] = "start_capture";
        captureAction["action_name"] = "Start Capture";
        captureAction["tool_name"] = "milos-network-dashboard";
        captureAction["action_type"] = "START_CAPTURE";
        captureAction["d_bus_interface"] = "org.milos.NetworkDashboard";
        captureAction["d_bus_method"] = "StartCapture";
        actions.append(captureAction);

        QVariantMap threatsAction;
        threatsAction["action_id"] = "view_threats";
        threatsAction["action_name"] = "View Threats";
        threatsAction["tool_name"] = "milos-network-dashboard";
        threatsAction["action_type"] = "VIEW_THREATS";
        threatsAction["d_bus_interface"] = "org.milos.NetworkDashboard";
        threatsAction["d_bus_method"] = "GetThreats";
        actions.append(threatsAction);
    }

    if (applicationName.contains("backup", Qt::CaseInsensitive)) {
        QVariantMap backupAction;
        backupAction["action_id"] = "create_backup";
        backupAction["action_name"] = "Create Backup";
        backupAction["tool_name"] = "milos-backup";
        backupAction["action_type"] = "CREATE_BACKUP";
        backupAction["d_bus_interface"] = "org.milos.BackupSystem";
        backupAction["d_bus_method"] = "CreateBackup";
        actions.append(backupAction);
    }

    return actions;
}

