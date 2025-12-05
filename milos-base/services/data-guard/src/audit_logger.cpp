#include "audit_logger.h"
#include <milos/logging/logger.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QCryptographicHash>
#include <QByteArray>

AuditLogger::AuditLogger(QObject* parent)
    : QObject(parent)
    , m_auditServiceBus("org.milos.AuditService")
    , m_auditServicePath("/org/milos/AuditService")
    , m_initialized(false)
    , m_gracefulDegradation(true)
{
}

AuditLogger::~AuditLogger() {
}

bool AuditLogger::initialize(const QString& auditServiceBus, const QString& auditServicePath) {
    m_auditServiceBus = auditServiceBus;
    m_auditServicePath = auditServicePath;
    
    // Check if audit service is available
    if (!isAuditServiceAvailable()) {
        LOG_WARNING("Audit service not available (graceful degradation enabled)");
        // Continue with graceful degradation
    }
    
    m_initialized = true;
    return true;
}

bool AuditLogger::logTransmissionAttempt(const QString& source,
                                         const QString& destination,
                                         const QString& protocol,
                                         bool encrypted,
                                         const QString& action) {
    // Create STIG-compliant event data
    QVariantMap eventData;
    eventData["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    eventData["event_type"] = "transmission_attempt";
    eventData["user_id"] = "system";  // System-initiated event
    eventData["source_ip"] = source;
    eventData["destination_ip"] = destination;
    eventData["protocol"] = protocol;
    eventData["encrypted"] = encrypted;
    eventData["action"] = action;
    eventData["result"] = (action == "ALLOW") ? "success" : "blocked";
    eventData["service"] = "milos-data-guard";
    eventData["stig_compliant"] = true;
    
    // Generate STIG-compliant log entry with integrity hash
    QString stigLogEntry = generateSTIGLogEntry(eventData);
    
    // Log to audit service
    return logToAuditService("transmission_attempt", eventData);
}

bool AuditLogger::logPolicyViolation(const QString& policyId,
                                     const QString& violationType,
                                     const QVariantMap& details) {
    // Create STIG-compliant event data
    QVariantMap eventData = details;
    eventData["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    eventData["event_type"] = "policy_violation";
    eventData["user_id"] = details.value("user_id", "system").toString();
    eventData["policy_id"] = policyId;
    eventData["violation_type"] = violationType;
    eventData["result"] = "failure";
    eventData["service"] = "milos-data-guard";
    eventData["stig_compliant"] = true;
    
    // Generate STIG-compliant log entry with integrity hash
    QString stigLogEntry = generateSTIGLogEntry(eventData);
    
    // Log to audit service
    return logToAuditService("policy_violation", eventData);
}

bool AuditLogger::isAuditServiceAvailable() const {
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        return false;
    }
    
    // Check if service is available
    QDBusInterface interface(m_auditServiceBus, m_auditServicePath, 
                           "org.milos.AuditService", connection);
    
    if (!interface.isValid()) {
        return false;
    }
    
    // Try to call GetHealthStatus method to verify service is responding
    QDBusReply<QString> reply = interface.call("GetHealthStatus");
    return reply.isValid();
}

bool AuditLogger::logToAuditService(const QString& eventType, const QVariantMap& eventData) {
    if (!m_initialized) {
        return false;
    }
    
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        if (m_gracefulDegradation) {
            // Log locally or queue for later
            LOG_INFO(QString("Audit log (local): %1 - %2")
                     .arg(eventType).arg(eventData.value("timestamp").toString()));
            return true;
        }
        return false;
    }
    
    // Create D-Bus interface to audit service
    QDBusInterface interface(m_auditServiceBus, m_auditServicePath, 
                           "org.milos.AuditService", connection);
    
    if (!interface.isValid()) {
        if (m_gracefulDegradation) {
            // Log locally or queue for later
            LOG_INFO(QString("Audit log (local): %1 - %2")
                     .arg(eventType).arg(eventData.value("timestamp").toString()));
            return true;
        }
        LOG_ERROR(QString("Audit service interface invalid: %1")
                  .arg(interface.lastError().message()));
        return false;
    }
    
    // Generate STIG-compliant log entry with integrity hash
    QString stigLogEntry = generateSTIGLogEntry(eventData);
    
    // Call LogEvent method on audit service
    // Method signature: LogEvent(QString eventData) - takes JSON string
    // Use STIG-compliant log entry
    QString eventDataJson = stigLogEntry;
    
    QDBusReply<QString> reply = interface.call("LogEvent", eventDataJson);
    
    if (!reply.isValid()) {
        if (m_gracefulDegradation) {
            // Log locally or queue for later
            LOG_INFO(QString("Audit log (local): %1 - %2")
                     .arg(eventType).arg(eventData.value("timestamp").toString()));
            return true;
        }
        LOG_ERROR(QString("Failed to log to audit service: %1")
                  .arg(reply.error().message()));
        return false;
    }
    
    // LogEvent returns event ID (QString), not bool
    return !reply.value().isEmpty();
}

QString AuditLogger::generateSTIGLogEntry(const QVariantMap& eventData) {
    // Create JSON document from event data
    QJsonObject eventObj;
    for (auto it = eventData.begin(); it != eventData.end(); ++it) {
        eventObj[it.key()] = QJsonValue::fromVariant(it.value());
    }
    
    // Ensure required STIG fields are present
    if (!eventObj.contains("timestamp")) {
        eventObj["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    }
    if (!eventObj.contains("stig_compliant")) {
        eventObj["stig_compliant"] = true;
    }
    
    // Convert to JSON string
    QJsonDocument doc(eventObj);
    QString logData = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    
    // Generate integrity hash
    QString integrityHash = generateIntegrityHash(logData);
    eventObj["integrity_hash"] = QString("sha256:%1").arg(integrityHash);
    
    // Return final STIG-compliant log entry
    QJsonDocument finalDoc(eventObj);
    return QString::fromUtf8(finalDoc.toJson(QJsonDocument::Compact));
}

QString AuditLogger::generateIntegrityHash(const QString& logData) {
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(logData.toUtf8());
    return QString::fromLatin1(hash.result().toHex());
}

