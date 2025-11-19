#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>

/**
 * @brief Audit Logger
 * 
 * Integrates with Audit Service (Epic 16) for logging dashboard operations.
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
     * @brief Log dashboard operation
     * @param operation Operation type
     * @param details Operation details
     * @return true if log successful, false otherwise
     */
    bool logDashboardOperation(const QString& operation, const QString& details = QString());

private:
    bool m_initialized;
    class QDBusInterface* m_interface;
};

#endif // AUDIT_LOGGER_H

