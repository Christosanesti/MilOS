#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief Audit Logger for Update Service
 * 
 * Integrates with MilOS Audit Service via D-Bus to log
 * update activities, signature verifications, and service events.
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
     * @brief Log update activity
     * @param updateId Update ID
     * @param action Action taken (check, apply, rollback)
     * @param packageList List of packages involved
     * @param status Update status
     * @param details Additional details
     * @return true if logging successful, false otherwise
     */
    bool logUpdateActivity(const QString& updateId,
                          const QString& action,
                          const QStringList& packageList,
                          const QString& status,
                          const QVariantMap& details = QVariantMap());

    /**
     * @brief Log signature verification
     * @param packageName Package name
     * @param verified Whether signature was verified
     * @param keyId GPG key ID used for verification
     * @return true if logging successful, false otherwise
     */
    bool logSignatureVerification(const QString& packageName,
                                 bool verified,
                                 const QString& keyId = QString());

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

