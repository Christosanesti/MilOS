#include "audit_logger.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QDateTime>
#include <QDebug>

AuditLogger::AuditLogger(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

AuditLogger::~AuditLogger() {
}

bool AuditLogger::initialize() {
    if (m_initialized) {
        return true;
    }

    // Check if Audit Service is available
    QDBusInterface iface("org.milos.AuditService",
                        "/org/milos/AuditService",
                        "org.milos.AuditService",
                        QDBusConnection::sessionBus());

    if (!iface.isValid()) {
        qWarning() << "Audit Service not available";
        return false;
    }

    m_initialized = true;
    return true;
}

void AuditLogger::logKeyOperation(const QString& operation, const QString& userId, const QVariantMap& eventData) {
    if (!m_initialized) {
        return;
    }

    QDBusInterface iface("org.milos.AuditService",
                        "/org/milos/AuditService",
                        "org.milos.AuditService",
                        QDBusConnection::sessionBus());

    if (!iface.isValid()) {
        qWarning() << "Audit Service not available for logging";
        return;
    }

    QVariantMap logData;
    logData["operation"] = operation;
    logData["user_id"] = userId;
    logData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    logData["component"] = "SecureMessenger";
    
    // Merge event data
    for (auto it = eventData.constBegin(); it != eventData.constEnd(); ++it) {
        logData[it.key()] = it.value();
    }

    QDBusReply<void> reply = iface.call("LogEvent", "SecureMessenger", operation, logData);
    if (!reply.isValid()) {
        qWarning() << "Failed to log audit event:" << reply.error().message();
    }
}

