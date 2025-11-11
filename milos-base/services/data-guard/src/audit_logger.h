#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief Audit Logger for Data Transmission Guard Service
 * 
 * Integrates with MilOS Audit Service via D-Bus to log
 * transmission attempts, policy violations, and service events.
 */
class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    /**
     * @brief Initialize audit logger
     * @param auditServiceBus D-Bus service name (default: org.milos.AuditService)
     * @param auditServicePath D-Bus object path (default: /org/milos/AuditService)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& auditServiceBus = "org.milos.AuditService",
                    const QString& auditServicePath = "/org/milos/AuditService");

    /**
     * @brief Log transmission attempt (blocked or allowed)
     * @param source Source IP address
     * @param destination Destination IP address
     * @param protocol Network protocol (TCP, UDP, etc.)
     * @param encrypted Whether packet was encrypted
     * @param action Action taken (ALLOW, BLOCK)
     * @return true if logging successful, false otherwise
     */
    bool logTransmissionAttempt(const QString& source,
                                const QString& destination,
                                const QString& protocol,
                                bool encrypted,
                                const QString& action);

    /**
     * @brief Log policy violation
     * @param policyId Policy ID that was violated
     * @param violationType Type of violation
     * @param details Additional violation details
     * @return true if logging successful, false otherwise
     */
    bool logPolicyViolation(const QString& policyId,
                           const QString& violationType,
                           const QVariantMap& details);

    /**
     * @brief Check if audit service is available
     * @return true if available, false otherwise
     */
    bool isAuditServiceAvailable() const;

private:
    QString m_auditServiceBus;
    QString m_auditServicePath;
    bool m_initialized;
    bool m_gracefulDegradation;  // Continue operating if audit service unavailable

    /**
     * @brief Log event to audit service via D-Bus
     * @param eventType Event type
     * @param eventData Event data
     * @return true if logging successful, false otherwise
     */
    bool logToAuditService(const QString& eventType, const QVariantMap& eventData);
};

#endif // AUDIT_LOGGER_H

