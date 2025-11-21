#ifndef AI_SECURITY_MONITORING_AUDIT_LOGGER_H
#define AI_SECURITY_MONITORING_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logThreatDetected(const QString& threatId, const QString& threatType);
    void logThreatContained(const QString& threatId);
    void logAnomalyDetected(const QString& type, const QString& details);

private:
    QString generateEventId() const;
};

#endif // AI_SECURITY_MONITORING_AUDIT_LOGGER_H

