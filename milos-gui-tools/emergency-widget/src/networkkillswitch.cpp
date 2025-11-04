#include "networkkillswitch.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusReply>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>

NetworkKillSwitch::NetworkKillSwitch(QObject *parent)
    : QObject(parent)
    , m_networkEnabled(true)
    , m_dbusConnection(QDBusConnection::systemBus())
{
    // Check initial network status
    checkNetworkStatus();
}

bool NetworkKillSwitch::isNetworkManagerAvailable() const
{
    QDBusConnection connection = QDBusConnection::systemBus();
    return connection.isConnected() && 
           connection.interface()->isServiceRegistered("org.freedesktop.NetworkManager");
}

bool NetworkKillSwitch::isSystemdNetworkdAvailable() const
{
    QDBusConnection connection = QDBusConnection::systemBus();
    // systemd-networkd doesn't have a standard D-Bus service name
    // Check via systemctl or process existence
    QProcess process;
    process.start("systemctl", QStringList() << "is-active" << "systemd-networkd");
    process.waitForFinished(3000);
    return process.exitCode() == 0;
}

bool NetworkKillSwitch::disableNetwork()
{
    // Try NetworkManager first, then systemd-networkd
    bool success = false;
    
    if (isNetworkManagerAvailable()) {
        success = disableNetworkViaNetworkManager();
    } else if (isSystemdNetworkdAvailable()) {
        success = disableNetworkViaSystemd();
    } else {
        qWarning() << "No network manager available (NetworkManager or systemd-networkd)";
        emit networkDisableCompleted(false, "No network manager available");
        return false;
    }

    if (success) {
        m_networkEnabled = false;
        emit networkStatusChanged(false);
        
        // Log audit event
        QVariantMap eventData;
        eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        eventData["action"] = "network_kill_switch";
        eventData["status"] = "disabled";
        logAuditEvent("emergency_network_kill_switch", eventData);
    }

    emit networkDisableCompleted(success, success ? QString() : "Failed to disable network");
    return success;
}

bool NetworkKillSwitch::disableNetworkViaNetworkManager()
{
    QDBusInterface networkManager("org.freedesktop.NetworkManager",
                                  "/org/freedesktop/NetworkManager",
                                  "org.freedesktop.NetworkManager",
                                  m_dbusConnection);

    if (!networkManager.isValid()) {
        qWarning() << "NetworkManager D-Bus interface invalid";
        return false;
    }

    // Get all devices
    QDBusReply<QList<QDBusObjectPath>> devicesReply = networkManager.call("GetDevices");
    if (!devicesReply.isValid()) {
        qWarning() << "Failed to get network devices:" << devicesReply.error().message();
        return false;
    }

    QList<QDBusObjectPath> devices = devicesReply.value();
    bool allDisabled = true;

    // Disable each network device
    for (const QDBusObjectPath &devicePath : devices) {
        QDBusInterface device("org.freedesktop.NetworkManager",
                             devicePath.path(),
                             "org.freedesktop.NetworkManager.Device",
                             m_dbusConnection);

        if (!device.isValid()) {
            continue;
        }

        // Get device type
        QDBusReply<quint32> deviceTypeReply = device.call("Get", "org.freedesktop.NetworkManager.Device", "DeviceType");
        if (!deviceTypeReply.isValid()) {
            continue;
        }

        // Only disable ethernet and wifi devices (type 1 = ethernet, type 2 = wifi)
        quint32 deviceType = deviceTypeReply.value();
        if (deviceType == 1 || deviceType == 2) {
            // Disable device
            QDBusReply<void> disableReply = device.call("Disconnect");
            if (!disableReply.isValid()) {
                qWarning() << "Failed to disable device" << devicePath.path() << ":" << disableReply.error().message();
                allDisabled = false;
            }
        }
    }

    return allDisabled;
}

bool NetworkKillSwitch::disableNetworkViaSystemd()
{
    // systemd-networkd doesn't have a direct D-Bus API for disabling interfaces
    // Use systemctl to stop network services or disable interfaces via networkctl
    
    QProcess process;
    process.start("networkctl", QStringList() << "down" << "--all");
    process.waitForFinished(5000);
    
    if (process.exitCode() != 0) {
        qWarning() << "Failed to disable network via systemd-networkd:" << process.errorString();
        return false;
    }

    return true;
}

void NetworkKillSwitch::checkNetworkStatus()
{
    bool wasEnabled = m_networkEnabled;
    
    if (isNetworkManagerAvailable()) {
        m_networkEnabled = checkNetworkStatusViaNetworkManager();
    } else if (isSystemdNetworkdAvailable()) {
        m_networkEnabled = checkNetworkStatusViaSystemd();
    } else {
        // Default to enabled if no network manager detected
        m_networkEnabled = true;
    }

    if (wasEnabled != m_networkEnabled) {
        emit networkStatusChanged(m_networkEnabled);
    }
}

bool NetworkKillSwitch::checkNetworkStatusViaNetworkManager()
{
    QDBusInterface networkManager("org.freedesktop.NetworkManager",
                                  "/org/freedesktop/NetworkManager",
                                  "org.freedesktop.NetworkManager",
                                  m_dbusConnection);

    if (!networkManager.isValid()) {
        return true; // Default to enabled
    }

    // Get network state
    QDBusReply<quint32> stateReply = networkManager.call("state");
    if (!stateReply.isValid()) {
        return true; // Default to enabled
    }

    // NetworkManager states: 20 = disconnected, 30 = disconnecting, 40 = connecting, 50 = connected, etc.
    // Consider network enabled if state >= 40 (connecting or connected)
    quint32 state = stateReply.value();
    return state >= 40;
}

bool NetworkKillSwitch::checkNetworkStatusViaSystemd()
{
    QProcess process;
    process.start("networkctl", QStringList() << "status");
    process.waitForFinished(3000);
    
    if (process.exitCode() != 0) {
        return true; // Default to enabled
    }

    QString output = process.readAllStandardOutput();
    // Check if any interface is in "routable" or "configured" state
    return output.contains("routable") || output.contains("configured");
}

void NetworkKillSwitch::logAuditEvent(const QString &actionType, const QVariantMap &eventData)
{
    // TODO: Implement audit service D-Bus call when audit service is available
    // For now, log to console
    qDebug() << "Audit Log:" << actionType << eventData;
    
    // Future implementation:
    // QDBusInterface auditService("org.milos.AuditService",
    //                             "/org/milos/AuditService",
    //                             "org.milos.AuditService",
    //                             m_dbusConnection);
    // auditService.call("LogEvent", actionType, eventData);
}

