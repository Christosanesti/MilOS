#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <QObject>
#include <QString>

/**
 * @brief Audit Logger
 * 
 * Integrates with Audit Service (Epic 16) for logging segmentation operations.
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
     * @brief Log segmentation operation
     * @param operation Operation type (create, update, delete, etc.)
     * @param segmentId Segment ID
     * @param details Operation details
     * @return true if log successful, false otherwise
     */
    bool logSegmentationOperation(const QString& operation, const QString& segmentId, const QString& details = QString());

private:
    bool m_initialized;
    class QDBusInterface* m_interface;
};

#endif // AUDIT_LOGGER_H

