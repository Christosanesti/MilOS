#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDBusInterface>
#include <QDBusReply>

/**
 * @brief Audit Logger
 * 
 * Logs device operations to Audit Service (Epic 16).
 */
class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);

    /**
     * @brief Log device operation
     * @param eventType Event type (e.g., "device_connected", "device_error")
     * @param deviceId Device ID
     * @param eventData Additional event data
     */
    void logDeviceOperation(const QString& eventType, const QString& deviceId, const QVariantMap& eventData = QVariantMap());

    /**
     * @brief Log biometric capture
     * @param deviceId Device ID
     * @param success Whether capture was successful
     */
    void logBiometricCapture(const QString& deviceId, bool success);

    /**
     * @brief Log biometric verification
     * @param deviceId Device ID
     * @param matchScore Match score (0-100)
     */
    void logBiometricVerification(const QString& deviceId, int matchScore);

    /**
     * @brief Log biometric enrollment
     * @param deviceId Device ID
     * @param success Whether enrollment was successful
     */
    void logBiometricEnrollment(const QString& deviceId, bool success);

private:
    QDBusInterface* m_auditInterface;
    bool m_auditServiceAvailable;

    bool initializeAuditInterface();
};

#endif // AUDIT_LOGGER_H


