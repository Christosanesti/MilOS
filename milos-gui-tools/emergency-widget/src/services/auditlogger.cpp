#include "auditlogger.h"
#include "milos/logging/logger.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

AuditLogger::AuditLogger(QObject *parent)
    : QObject(parent)
{
}

void AuditLogger::logEvent(const QString &eventType, const QVariantMap &eventData)
{
    // Add timestamp and user identification
    QVariantMap fullEventData = eventData;
    fullEventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    fullEventData["user"] = qgetenv("USER");
    fullEventData["event_type"] = eventType;
    
    bool success = logViaAuditService(eventType, fullEventData);
    
    if (success) {
        emit eventLogged(eventType);
    } else {
        emit error("Failed to log event to audit service.");
    }
}

void AuditLogger::logEmergencyAction(const QString &actionType, const QVariantMap &details)
{
    QString eventType = "emergency_" + actionType;
    logEvent(eventType, details);
}

bool AuditLogger::logViaAuditService(const QString &eventType, const QVariantMap &eventData)
{
    // Implement milos-audit-service D-Bus integration
    // Interface: org.milos.AuditService
    // Method: LogEvent(event_data as JSON string)
    // Service: org.milos.AuditService
    
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface auditInterface("org.milos.AuditService",
                                  "/org/milos/AuditService",
                                  "org.milos.AuditService",
                                  bus);
    
    if (!auditInterface.isValid()) {
        // Service not available, log to console as fallback
        qWarning() << "Audit service not available, logging to console:" << eventType << eventData;
        return false;
    }
    
    // Convert event data to JSON string
    QJsonObject jsonObj;
    for (auto it = eventData.constBegin(); it != eventData.constEnd(); ++it) {
        jsonObj[it.key()] = QJsonValue::fromVariant(it.value());
    }
    
    QJsonDocument doc(jsonObj);
    QString eventDataJson = QString::fromUtf8(doc.toJson());
    
    // Call LogEvent method
    QDBusReply<QString> reply = auditInterface.call("LogEvent", eventDataJson);
    if (reply.isValid()) {
        QString eventId = reply.value();
        LOG_INFO(QString("Event logged to audit service: %1, Event ID: %2").arg(eventType, eventId));
        return true;
    } else {
        LOG_WARNING(QString("Failed to log event to audit service: %1").arg(reply.error().message()));
        // Fallback: log to console
        LOG_INFO(QString("Audit Log (fallback): %1").arg(eventType));
        return false;
    }
}

