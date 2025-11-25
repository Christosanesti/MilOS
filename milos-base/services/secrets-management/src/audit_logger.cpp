#include "audit_logger.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <iostream>

AuditLogger::AuditLogger()
    : m_initialized(false)
    , m_serviceName("org.milos.AuditService")
{
}

AuditLogger::~AuditLogger() {
}

bool AuditLogger::initialize() {
    if (m_initialized) {
        return true;
    }

    // Connect to D-Bus audit service
    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        std::cerr << "Cannot connect to D-Bus system bus" << std::endl;
        // Still mark as initialized to allow fallback logging
        m_initialized = true;
        return false;
    }

    // Check if audit service is available
    QDBusInterface auditInterface(m_serviceName.c_str(),
                                  "/org/milos/AuditService",
                                  "org.milos.AuditService",
                                  bus);
    
    if (!auditInterface.isValid()) {
        std::cerr << "Audit service not available, will use fallback logging" << std::endl;
        // Still mark as initialized to allow fallback logging
        m_initialized = true;
        return false;
    }

    m_initialized = true;
    return true;
}

void AuditLogger::logEvent(const std::string& eventType, const std::string& eventData) {
    if (!m_initialized) {
        return;
    }

    // Send to D-Bus audit service
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface auditInterface(m_serviceName.c_str(),
                                  "/org/milos/AuditService",
                                  "org.milos.AuditService",
                                  bus);

    if (auditInterface.isValid()) {
        // Parse eventData as JSON and add event_type and timestamp
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(eventData).toUtf8(), &error);
        
        QJsonObject jsonObj;
        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            jsonObj = doc.object();
        } else {
            // If eventData is not JSON, create a JSON object with the data as a string
            jsonObj["data"] = QString::fromStdString(eventData);
        }
        
        // Add required fields
        jsonObj["event_type"] = QString::fromStdString(eventType);
        jsonObj["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        jsonObj["service"] = "secrets-management";
        
        QJsonDocument fullDoc(jsonObj);
        QString eventDataJson = QString::fromUtf8(fullDoc.toJson());
        
        // Call LogEvent method
        QDBusReply<QString> reply = auditInterface.call("LogEvent", eventDataJson);
        if (reply.isValid()) {
            QString eventId = reply.value();
            std::cout << "[AUDIT] Event logged: " << eventType << " (ID: " << eventId.toStdString() << ")" << std::endl;
            return;
        } else {
            std::cerr << "[AUDIT] Failed to log event: " << reply.error().message().toStdString() << std::endl;
        }
    }

    // Fallback: log to stdout
    std::cout << "[AUDIT] " << eventType << ": " << eventData << std::endl;
}

