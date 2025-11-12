#include "audit_logger.h"
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDateTime>
#include <QVariantMap>
#include <QDebug>
#include <iostream>

AuditLogger::AuditLogger(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_interface(nullptr)
{
}

AuditLogger::~AuditLogger() {
    if (m_interface) {
        delete m_interface;
    }
}

bool AuditLogger::initialize() {
    if (m_initialized) {
        return true;
    }

    m_interface = new QDBusInterface("org.milos.AuditService",
                                     "/org/milos/AuditService",
                                     "org.milos.AuditService",
                                     QDBusConnection::systemBus(),
                                     this);
    
    m_initialized = m_interface->isValid();
    if (!m_initialized) {
        std::cerr << "Warning: Failed to connect to Audit Service (continuing without audit logging)" << std::endl;
    }

    return true;
}

bool AuditLogger::logSegmentationOperation(const QString& operation, const QString& segmentId, const QString& details) {
    if (!m_initialized || !m_interface) {
        return false;
    }

    QVariantMap eventData;
    eventData["operation"] = operation;
    eventData["segmentId"] = segmentId;
    eventData["details"] = details;
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString event = "network_segmentation_" + operation;
    QDBusReply<void> reply = m_interface->call("LogEvent", event, eventData);
    
    return reply.isValid();
}

