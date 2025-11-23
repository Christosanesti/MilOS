#ifndef SECURE_DEV_AUDIT_LOGGER_H
#define SECURE_DEV_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logSecurityIssue(const QString& issueId, const QString& severity);
    void logVulnerabilityFound(const QString& dependency, const QString& version);

private:
    QString generateEventId() const;
};

#endif // SECURE_DEV_AUDIT_LOGGER_H





