#ifndef SECURE_COMMUNICATION_HUB_AUDIT_LOGGER_H
#define SECURE_COMMUNICATION_HUB_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logMessageSent(const QString& messageId, const QString& type, const QString& to);
    void logMessageReceived(const QString& messageId, const QString& type);
    void logMessageExpired(const QString& messageId);

private:
    QString generateEventId() const;
};

#endif // SECURE_COMMUNICATION_HUB_AUDIT_LOGGER_H

