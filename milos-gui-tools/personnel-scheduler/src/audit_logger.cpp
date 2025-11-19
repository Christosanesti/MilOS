#include "audit_logger.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDateTime>
#include <QDebug>

AuditLogger::AuditLogger(QObject* parent)
    : QObject(parent)
    , m_auditInterface(nullptr)
    , m_auditServiceAvailable(false)
{
    initializeAuditInterface();
}

bool AuditLogger::initializeAuditInterface() {
    if (m_auditInterface) {
        return m_auditServiceAvailable;
    }
    
    m_auditInterface = new QDBusInterface(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        QDBusConnection::systemBus(),
        this
    );
    
    m_auditServiceAvailable = m_auditInterface->isValid();
    
    if (!m_auditServiceAvailable) {
        qWarning() << "Audit Service not available";
    }
    
    return m_auditServiceAvailable;
}

void AuditLogger::logDeviceOperation(const QString& eventType, const QString& deviceId, const QVariantMap& eventData) {
    if (!m_auditServiceAvailable) {
        return;
    }
    
    QVariantMap auditData = eventData;
    auditData["device_id"] = deviceId;
    auditData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        "LogEvent"
    );
    
    message << eventType << auditData;
    
    QDBusConnection::systemBus().call(message, QDBus::NoBlock);
}

void AuditLogger::logBiometricCapture(const QString& deviceId, bool success) {
    QVariantMap eventData;
    eventData["success"] = success;
    logDeviceOperation("biometric_capture", deviceId, eventData);
}

void AuditLogger::logBiometricVerification(const QString& deviceId, int matchScore) {
    QVariantMap eventData;
    eventData["match_score"] = matchScore;
    logDeviceOperation("biometric_verification", deviceId, eventData);
}

void AuditLogger::logBiometricEnrollment(const QString& deviceId, bool success) {
    QVariantMap eventData;
    eventData["success"] = success;
    logDeviceOperation("biometric_enrollment", deviceId, eventData);
}


