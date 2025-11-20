#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

/**
 * @brief Audit Logger
 * 
 * Logs key operations to the Audit Service (Epic 16).
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
     * @brief Log key operation
     * @param operation Operation name
     * @param userId User ID
     * @param eventData Additional event data
     */
    void logKeyOperation(const QString& operation, const QString& userId, const QVariantMap& eventData = QVariantMap());

private:
    bool m_initialized;
};

#endif // AUDIT_LOGGER_H

