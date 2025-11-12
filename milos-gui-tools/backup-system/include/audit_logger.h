#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>

/**
 * @brief Audit Logger
 * 
 * Integrates with Audit Service (Epic 16) for logging backup operations.
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
     * @brief Log backup operation
     * @param operation Operation type (create, restore, delete, etc.)
     * @param details Operation details
     * @return true if log successful, false otherwise
     */
    bool logBackupOperation(const QString& operation, const QString& details);

private:
    bool m_initialized;
};

#endif // AUDIT_LOGGER_H

