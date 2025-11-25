#include "networkkillswitch.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QVariant>
#include <QDebug>

NetworkKillSwitch::NetworkKillSwitch(QObject *parent)
    : QObject(parent)
    , m_networkEnabled(true)
    , m_requiresConfirmation(true)
{
    checkNetworkStatus();
}

void NetworkKillSwitch::setRequiresConfirmation(bool requires)
{
    if (m_requiresConfirmation != requires) {
        m_requiresConfirmation = requires;
        emit requiresConfirmationChanged();
    }
}

void NetworkKillSwitch::disableNetwork()
{
    // Implement network disable via D-Bus
    // Try NetworkManager first, then fallback to systemd-networkd
    
    bool success = false;
    
    // Try NetworkManager first
    success = disableNetworkManager();
    
    // Fallback to systemd-networkd
    if (!success) {
        success = disableSystemdNetworkd();
    }
    
    if (success) {
        m_networkEnabled = false;
        emit networkEnabledChanged();
        emit networkDisabled();
    } else {
        emit error("Failed to disable network. Please check system permissions.");
    }
}

void NetworkKillSwitch::enableNetwork()
{
    // Implement network enable via D-Bus
    // Try NetworkManager first, then fallback to systemd-networkd
    
    bool success = false;
    
    // Try NetworkManager first
    success = enableNetworkManager();
    
    // Fallback to systemd-networkd
    if (!success) {
        success = enableSystemdNetworkd();
    }
    
    if (success) {
        m_networkEnabled = true;
        emit networkEnabledChanged();
        emit networkEnabled();
    } else {
        emit error("Failed to enable network. Please check system permissions.");
    }
}

void NetworkKillSwitch::checkNetworkStatus()
{
    // Check NetworkManager first
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface propertiesInterface("org.freedesktop.NetworkManager",
                                        "/org/freedesktop/NetworkManager",
                                        "org.freedesktop.DBus.Properties",
                                        bus);
    
    if (propertiesInterface.isValid()) {
        QDBusReply<QVariant> reply = propertiesInterface.call("Get",
                                                              "org.freedesktop.NetworkManager",
                                                              "NetworkingEnabled");
        if (reply.isValid()) {
            m_networkEnabled = reply.value().toBool();
            emit networkEnabledChanged();
            return;
        }
    }
    
    // Fallback: check systemd-networkd
    QDBusInterface systemdInterface("org.freedesktop.systemd1",
                                    "/org/freedesktop/systemd1",
                                    "org.freedesktop.systemd1.Manager",
                                    bus);
    
    if (systemdInterface.isValid()) {
        // Check if systemd-networkd.service is active
        QDBusReply<QString> reply = systemdInterface.call("GetUnitFileState", "systemd-networkd.service");
        if (reply.isValid() && reply.value() != "disabled") {
            // Network is likely enabled if service is active
            m_networkEnabled = true;
            emit networkEnabledChanged();
            return;
        }
    }
    
    // Default: assume enabled if we can't determine
    m_networkEnabled = true;
    emit networkEnabledChanged();
}

QStringList NetworkKillSwitch::getNetworkInterfaces()
{
    QStringList interfaces;
    
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface nmInterface("org.freedesktop.NetworkManager",
                               "/org/freedesktop/NetworkManager",
                               "org.freedesktop.NetworkManager",
                               bus);
    
    if (nmInterface.isValid()) {
        QDBusReply<QList<QDBusObjectPath>> reply = nmInterface.call("GetDevices");
        if (reply.isValid()) {
            QList<QDBusObjectPath> devices = reply.value();
            for (const QDBusObjectPath &devicePath : devices) {
                QDBusInterface propertiesInterface("org.freedesktop.NetworkManager",
                                                    devicePath.path(),
                                                    "org.freedesktop.DBus.Properties",
                                                    bus);
                if (propertiesInterface.isValid()) {
                    QDBusReply<QVariant> ifaceReply = propertiesInterface.call("Get",
                                                                                "org.freedesktop.NetworkManager.Device",
                                                                                "Interface");
                    if (ifaceReply.isValid()) {
                        interfaces.append(ifaceReply.value().toString());
                    }
                }
            }
        }
    }
    
    return interfaces;
}

bool NetworkKillSwitch::disableNetworkManager()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface propertiesInterface("org.freedesktop.NetworkManager",
                                        "/org/freedesktop/NetworkManager",
                                        "org.freedesktop.DBus.Properties",
                                        bus);
    
    if (!propertiesInterface.isValid()) {
        qWarning() << "NetworkManager D-Bus interface not available";
        return false;
    }
    
    // Disable all networking using Properties.Set
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.NetworkManager",
                                                        "/org/freedesktop/NetworkManager",
                                                        "org.freedesktop.DBus.Properties",
                                                        "Set");
    msg << "org.freedesktop.NetworkManager" << "NetworkingEnabled" << QVariant(false);
    
    QDBusReply<void> reply = bus.call(msg);
    if (reply.isValid()) {
        qDebug() << "NetworkManager networking disabled";
        return true;
    } else {
        qWarning() << "Failed to disable NetworkManager:" << reply.error().message();
        return false;
    }
}

bool NetworkKillSwitch::disableSystemdNetworkd()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface systemdInterface("org.freedesktop.systemd1",
                                    "/org/freedesktop/systemd1",
                                    "org.freedesktop.systemd1.Manager",
                                    bus);
    
    if (!systemdInterface.isValid()) {
        qWarning() << "systemd D-Bus interface not available";
        return false;
    }
    
    // Stop systemd-networkd service
    QDBusReply<QDBusObjectPath> reply = systemdInterface.call("StopUnit", "systemd-networkd.service", "replace");
    if (reply.isValid()) {
        qDebug() << "systemd-networkd service stopped";
        return true;
    } else {
        qWarning() << "Failed to stop systemd-networkd:" << reply.error().message();
        return false;
    }
}

bool NetworkKillSwitch::enableNetworkManager()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface propertiesInterface("org.freedesktop.NetworkManager",
                                        "/org/freedesktop/NetworkManager",
                                        "org.freedesktop.DBus.Properties",
                                        bus);
    
    if (!propertiesInterface.isValid()) {
        qWarning() << "NetworkManager D-Bus interface not available";
        return false;
    }
    
    // Enable all networking using Properties.Set
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.NetworkManager",
                                                        "/org/freedesktop/NetworkManager",
                                                        "org.freedesktop.DBus.Properties",
                                                        "Set");
    msg << "org.freedesktop.NetworkManager" << "NetworkingEnabled" << QVariant(true);
    
    QDBusReply<void> reply = bus.call(msg);
    if (reply.isValid()) {
        qDebug() << "NetworkManager networking enabled";
        return true;
    } else {
        qWarning() << "Failed to enable NetworkManager:" << reply.error().message();
        return false;
    }
}

bool NetworkKillSwitch::enableSystemdNetworkd()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface systemdInterface("org.freedesktop.systemd1",
                                    "/org/freedesktop/systemd1",
                                    "org.freedesktop.systemd1.Manager",
                                    bus);
    
    if (!systemdInterface.isValid()) {
        qWarning() << "systemd D-Bus interface not available";
        return false;
    }
    
    // Start systemd-networkd service
    QDBusReply<QDBusObjectPath> reply = systemdInterface.call("StartUnit", "systemd-networkd.service", "replace");
    if (reply.isValid()) {
        qDebug() << "systemd-networkd service started";
        return true;
    } else {
        qWarning() << "Failed to start systemd-networkd:" << reply.error().message();
        return false;
    }
}

