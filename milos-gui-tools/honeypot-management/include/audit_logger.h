#ifndef HONEYPOT_MANAGEMENT_AUDIT_LOGGER_H
#define HONEYPOT_MANAGEMENT_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logHoneypotDeployed(const QString& honeypotId, const QString& type);
    void logThreatDetected(const QString& honeypotId, const QString& threatType);

private:
    QString generateEventId() const;
};

#endif // HONEYPOT_MANAGEMENT_AUDIT_LOGGER_H

