#include "audit_logger.h"
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <iostream>

AuditLogger::AuditLogger()
    : m_initialized(false)
    , m_dbusInterface(nullptr)
{
}

AuditLogger::~AuditLogger() {
    if (m_dbusInterface) {
        delete static_cast<QDBusInterface*>(m_dbusInterface);
        m_dbusInterface = nullptr;
    }
}

bool AuditLogger::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create D-Bus interface to Audit Service
    QDBusInterface* interface = new QDBusInterface(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        QDBusConnection::systemBus()
    );

    if (!interface->isValid()) {
        std::cerr << "Warning: Audit Service D-Bus interface not available" << std::endl;
        delete interface;
        return false;
    }

    m_dbusInterface = interface;
    m_initialized = true;
    return true;
}

void AuditLogger::logEvent(const std::string& event, const std::string& details) {
    if (!m_initialized || !m_dbusInterface) {
        return;
    }

    QDBusInterface* interface = static_cast<QDBusInterface*>(m_dbusInterface);
    
    QVariantMap eventData;
    eventData["service"] = "file-integrity-monitoring";
    eventData["event"] = QString::fromStdString(event);
    eventData["details"] = QString::fromStdString(details);

    QDBusReply<void> reply = interface->call("LogEvent", 
                                              QString::fromStdString(event),
                                              eventData);
    
    if (!reply.isValid()) {
        std::cerr << "Failed to log event to Audit Service: " << reply.error().message().toStdString() << std::endl;
    }
}

