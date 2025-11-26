#include "securemode.h"
#include "auditlogger.h"
#include "widgetconfig.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QSettings>

SecureMode::SecureMode(QObject *parent)
    : QObject(parent)
    , m_active(false)
{
    // Check current secure mode status
    checkSecureModeStatus();
}

void SecureMode::activateSecureMode()
{
    // Implement actual secure mode activation via systemd D-Bus
    QStringList servicesToStop = getNonEssentialServiceList();
    QStringList successfullyStopped;
    
    for (const QString &service : servicesToStop) {
        if (stopService(service)) {
            successfullyStopped.append(service);
        }
    }
    
    if (!successfullyStopped.isEmpty()) {
        m_stoppedServices = successfullyStopped;
        m_active = true;
        
        // Save state to file
        QString stateFile = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/milos-secure-mode.state";
        QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation));
        QFile file(stateFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const QString &service : successfullyStopped) {
                out << service << "\n";
            }
        }
        
        emit activeChanged();
        emit secureModeActivated();
        
        // Log to audit service
        AuditLogger logger;
        QVariantMap eventData;
        eventData["action"] = "activate";
        eventData["services_disabled"] = successfullyStopped.join(",");
        eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        logger.logEmergencyAction("secure_mode_activation", eventData);
    } else {
        emit error("Failed to activate secure mode. Please check system permissions.");
    }
}

void SecureMode::deactivateSecureMode()
{
    // Implement actual secure mode deactivation via systemd D-Bus
    for (const QString &service : m_stoppedServices) {
        startService(service);
    }
    
    m_stoppedServices.clear();
    m_active = false;
    
    // Remove state file
    QString stateFile = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/milos-secure-mode.state";
    QFile::remove(stateFile);
    
    emit activeChanged();
    emit secureModeDeactivated();
}

QStringList SecureMode::getNonEssentialServices()
{
    return getNonEssentialServiceList();
}

bool SecureMode::stopService(const QString &serviceName)
{
    // Implement systemd D-Bus integration
    QDBusConnection systemBus = QDBusConnection::systemBus();
    QDBusInterface systemdInterface("org.freedesktop.systemd1",
                                    "/org/freedesktop/systemd1",
                                    "org.freedesktop.systemd1.Manager",
                                    systemBus);
    
    if (!systemdInterface.isValid()) {
        qWarning() << "Cannot connect to systemd D-Bus interface";
        return false;
    }
    
    QDBusReply<QDBusObjectPath> reply = systemdInterface.call("StopUnit", serviceName, "replace");
    
    if (reply.isValid()) {
        qDebug() << "Stopped service:" << serviceName;
        return true;
    } else {
        qWarning() << "Failed to stop service" << serviceName << ":" << reply.error().message();
        return false;
    }
}

bool SecureMode::startService(const QString &serviceName)
{
    // Implement systemd D-Bus integration
    QDBusConnection systemBus = QDBusConnection::systemBus();
    QDBusInterface systemdInterface("org.freedesktop.systemd1",
                                    "/org/freedesktop/systemd1",
                                    "org.freedesktop.systemd1.Manager",
                                    systemBus);
    
    if (!systemdInterface.isValid()) {
        qWarning() << "Cannot connect to systemd D-Bus interface";
        return false;
    }
    
    QDBusReply<QDBusObjectPath> reply = systemdInterface.call("StartUnit", serviceName, "replace");
    
    if (reply.isValid()) {
        qDebug() << "Started service:" << serviceName;
        return true;
    } else {
        qWarning() << "Failed to start service" << serviceName << ":" << reply.error().message();
        return false;
    }
}

void SecureMode::checkSecureModeStatus()
{
    // Check if secure mode is active by checking if services are stopped
    // This is a simple check - in production, use a state file or D-Bus property
    QString stateFile = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/milos-secure-mode.state";
    if (QFile::exists(stateFile)) {
        m_active = true;
        // Load stopped services from state file
        QFile file(stateFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (!line.isEmpty()) {
                    m_stoppedServices.append(line);
                }
            }
        }
    }
}

QStringList SecureMode::getNonEssentialServiceList()
{
    // Load from configuration file
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/emergency-widget.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    
    settings.beginGroup("SecureMode");
    QStringList services = settings.value("nonEssentialServices", QStringList()).toStringList();
    settings.endGroup();
    
    // If no services configured, use defaults
    if (services.isEmpty()) {
        services = QStringList({
            "bluetooth.service",
            "cups.service",
            "avahi-daemon.service",
            "NetworkManager-dispatcher.service",
            "ModemManager.service"
        });
        
        // Save defaults to config
        settings.beginGroup("SecureMode");
        settings.setValue("nonEssentialServices", services);
        settings.endGroup();
        settings.sync();
    }
    
    return services;
}

