#include "emergencyshutdown.h"
#include "auditlogger.h"
#include "milos/logging/logger.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QDateTime>

EmergencyShutdown::EmergencyShutdown(QObject *parent)
    : QObject(parent)
{
}

void EmergencyShutdown::shutdown()
{
    // Log to audit service before shutdown
    AuditLogger logger;
    QVariantMap eventData;
    eventData["action"] = "emergency_shutdown";
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    eventData["user"] = qgetenv("USER");
    logger.logEmergencyAction("emergency_shutdown", eventData);
    
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
        LOG_INFO("Emergency shutdown initiated successfully");
        return true;
    } else {
        LOG_WARNING(QString("Failed to initiate shutdown: %1").arg(reply.error().message()));
        return false;
    }
}

