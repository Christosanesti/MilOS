#ifndef ENTERPRISE_MANAGEMENT_AUDIT_LOGGER_H
#define ENTERPRISE_MANAGEMENT_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logConfigChange(const QString& key, const QString& oldValue, const QString& newValue);
    void logDeploymentCreated(const QString& deploymentId, const QString& name);
    void logDeploymentExecuted(const QString& deploymentId);
    void logSiteAdded(const QString& siteId, const QString& name);

private:
    QString generateEventId() const;
};

#endif // ENTERPRISE_MANAGEMENT_AUDIT_LOGGER_H

