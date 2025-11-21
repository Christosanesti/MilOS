#ifndef ADVANCED_NETWORK_DEFENSE_AUDIT_LOGGER_H
#define ADVANCED_NETWORK_DEFENSE_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logThreatDetected(const QString& threatId, const QString& threatType);
    void logForensicCapture(const QString& evidenceId, const QString& type);
    void logWorkflowExecuted(const QString& workflowId, const QString& threatId);

private:
    QString generateEventId() const;
};

#endif // ADVANCED_NETWORK_DEFENSE_AUDIT_LOGGER_H

