#include "emergencyshutdown.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

EmergencyShutdown::EmergencyShutdown(QObject *parent)
    : QObject(parent)
{
}

void EmergencyShutdown::shutdown()
{
    bool success = shutdownViaSystemd();
    
    if (success) {
        emit shutdownInitiated();
    } else {
        emit error("Failed to initiate shutdown. Please check system permissions.");
    }
}

bool EmergencyShutdown::shutdownViaSystemd()
{
    // Connect to systemd-logind D-Bus interface
    QDBusConnection systemBus = QDBusConnection::systemBus();
    
    // Get the login manager object path
    QDBusInterface loginManager("org.freedesktop.login1",
                                "/org/freedesktop/login1",
                                "org.freedesktop.login1.Manager",
                                systemBus);
    
    if (!loginManager.isValid()) {
        qWarning() << "Cannot connect to systemd-logind D-Bus interface";
        return false;
    }
    
    // Call PowerOff method with interactive=false (non-interactive shutdown)
    // The method signature is: PowerOff(boolean interactive)
    QDBusReply<void> reply = loginManager.call("PowerOff", false);
    
    if (reply.isValid()) {
        qDebug() << "Emergency shutdown initiated successfully";
        return true;
    } else {
        qWarning() << "Failed to initiate shutdown:" << reply.error().message();
        return false;
    }
}

