#ifndef AIR_GAP_AUDIT_LOGGER_H
#define AIR_GAP_AUDIT_LOGGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

/**
 * @brief Audit Logger
 * 
 * Logs all air-gap detection and enforcement events.
 */
class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    /**
     * @brief Initialize audit logger
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Log air-gap status change
     * @param oldStatus Old status
     * @param newStatus New status
     */
    void logStatusChange(const QString& oldStatus, const QString& newStatus);

    /**
     * @brief Log policy level change
     * @param oldLevel Old policy level
     * @param newLevel New policy level
     */
    void logPolicyChange(const QString& oldLevel, const QString& newLevel);

    /**
     * @brief Log blocked operation
     * @param operation Operation name
     * @param reason Block reason
     */
    void logBlockedOperation(const QString& operation, const QString& reason);

private:
    QString generateEventId() const;
};

#endif // AIR_GAP_AUDIT_LOGGER_H

