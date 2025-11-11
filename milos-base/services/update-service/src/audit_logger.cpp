#include "audit_logger.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonArray>
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

bool AuditLogger::logUpdateActivity(const QString& updateId,
                                    const QString& action,
                                    const QStringList& packageList,
                                    const QString& status,
                                    const QVariantMap& details) {
    QVariantMap eventData = details;
    eventData["update_id"] = updateId;
    eventData["action"] = action;
    eventData["package_list"] = QVariant::fromValue(packageList);
    eventData["status"] = status;
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    eventData["service"] = "milos-update-service";
    
    return logToAuditService("update_activity", eventData);
}

bool AuditLogger::logSignatureVerification(const QString& packageName,
                                           bool verified,
                                           const QString& keyId) {
    QVariantMap eventData;
    eventData["package_name"] = packageName;
    eventData["verified"] = verified;
    if (!keyId.isEmpty()) {
        eventData["key_id"] = keyId;
    }
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    eventData["service"] = "milos-update-service";
    
    return logToAuditService("signature_verification", eventData);
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
    
    // Convert eventData to JSON string for LogEvent method
    QJsonObject jsonObj;
    for (auto it = eventData.begin(); it != eventData.end(); ++it) {
        if (it.value().type() == QVariant::StringList) {
            QJsonArray arr;
            QStringList list = it.value().toStringList();
            for (const QString& item : list) {
                arr.append(item);
            }
            jsonObj[it.key()] = arr;
        } else {
            jsonObj[it.key()] = QJsonValue::fromVariant(it.value());
        }
    }
    QJsonDocument doc(jsonObj);
    QString eventDataJson = QString::fromUtf8(doc.toJson());
    
    // Call LogEvent method on audit service
    // Method signature: LogEvent(QString eventData) where eventData is JSON string
    QDBusReply<QString> reply = interface.call("LogEvent", eventDataJson);
    
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
    
    // LogEvent returns event ID, so if we get a non-empty string, it succeeded
    return !reply.value().isEmpty();
}

