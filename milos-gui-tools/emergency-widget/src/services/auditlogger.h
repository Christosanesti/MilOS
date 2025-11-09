#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class AuditLogger : public QObject
{
    Q_OBJECT

public:
    explicit AuditLogger(QObject *parent = nullptr);
    
    Q_INVOKABLE void logEvent(const QString &eventType, const QVariantMap &eventData);
    Q_INVOKABLE void logEmergencyAction(const QString &actionType, const QVariantMap &details);

signals:
    void eventLogged(const QString &eventType);
    void error(const QString &errorMessage);

private:
    // TODO: Integrate with milos-audit-service D-Bus interface
    bool logViaAuditService(const QString &eventType, const QVariantMap &eventData);
};

#endif // AUDITLOGGER_H

