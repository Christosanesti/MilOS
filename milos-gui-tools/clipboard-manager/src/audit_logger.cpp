#include "audit_logger.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <iostream>

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

    // Check D-Bus connection
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        std::cerr << "Cannot connect to D-Bus system bus for audit logging" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool AuditLogger::logClipboardOperation(const QString& operation, const QString& details) {
    if (!m_initialized) {
        return false;
    }

    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        return false;
    }

    // Create D-Bus message to Audit Service
    QDBusMessage message = QDBusMessage::createSignal(
        "/org/milos/AuditService",
        "org.milos.AuditService",
        "LogEvent"
    );

    QJsonObject eventObj;
    eventObj["event_type"] = "clipboard_operation";
    eventObj["operation"] = operation;
    eventObj["details"] = details;
    eventObj["timestamp"] = QDateTime::currentMSecsSinceEpoch();

    QJsonDocument doc(eventObj);
    QList<QVariant> args;
    args.append(QVariant::fromValue(doc.toJson(QJsonDocument::Compact)));

    message.setArguments(args);

    if (!connection.send(message)) {
        std::cerr << "Failed to send audit log message" << std::endl;
        return false;
    }

    return true;
}

