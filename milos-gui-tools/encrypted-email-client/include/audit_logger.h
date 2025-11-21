#ifndef ENCRYPTED_EMAIL_AUDIT_LOGGER_H
#define ENCRYPTED_EMAIL_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logEmailSent(const QString& messageId, const QString& to);
    void logEmailReceived(const QString& messageId, const QString& from);
    void logAccountAdded(const QString& accountId, const QString& email);

private:
    QString generateEventId() const;
};

#endif // ENCRYPTED_EMAIL_AUDIT_LOGGER_H

