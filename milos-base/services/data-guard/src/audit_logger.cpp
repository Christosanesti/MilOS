#include "audit_logger.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <iostream>

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
        std::cerr << "Audit service not available (graceful degradation enabled)" << std::endl;
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
    QVariantMap eventData;
    eventData["source"] = source;
    eventData["destination"] = destination;
    eventData["protocol"] = protocol;
    eventData["encrypted"] = encrypted;
    eventData["action"] = action;
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    eventData["service"] = "milos-data-guard";
    
    return logToAuditService("transmission_attempt", eventData);
}

bool AuditLogger::logPolicyViolation(const QString& policyId,
                                     const QString& violationType,
                                     const QVariantMap& details) {
    QVariantMap eventData = details;
    eventData["policy_id"] = policyId;
    eventData["violation_type"] = violationType;
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    eventData["service"] = "milos-data-guard";
    
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
    
    // Try to call a simple method to verify service is responding
    // TODO: Use actual method name from Audit Service interface
    // For now, just check if interface is valid
    
    return true;
}

bool AuditLogger::logToAuditService(const QString& eventType, const QVariantMap& eventData) {
    if (!m_initialized) {
        return false;
    }
    
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        if (m_gracefulDegradation) {
            // Log locally or queue for later
            std::cout << "Audit log (local): " << eventType.toStdString() 
                      << " - " << eventData.value("timestamp").toString().toStdString() << std::endl;
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
            std::cout << "Audit log (local): " << eventType.toStdString() 
                      << " - " << eventData.value("timestamp").toString().toStdString() << std::endl;
            return true;
        }
        std::cerr << "Audit service interface invalid: " 
                  << interface.lastError().message().toStdString() << std::endl;
        return false;
    }
    
    // Call LogEvent method on audit service
    // Method signature: LogEvent(QString eventType, QVariantMap eventData)
    QDBusReply<bool> reply = interface.call("LogEvent", eventType, QVariant::fromValue(eventData));
    
    if (!reply.isValid()) {
        if (m_gracefulDegradation) {
            // Log locally or queue for later
            std::cout << "Audit log (local): " << eventType.toStdString() 
                      << " - " << eventData.value("timestamp").toString().toStdString() << std::endl;
            return true;
        }
        std::cerr << "Failed to log to audit service: " 
                  << reply.error().message().toStdString() << std::endl;
        return false;
    }
    
    return reply.value();
}

