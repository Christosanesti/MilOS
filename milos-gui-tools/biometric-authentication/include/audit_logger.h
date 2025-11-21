#ifndef BIOMETRIC_AUTHENTICATION_AUDIT_LOGGER_H
#define BIOMETRIC_AUTHENTICATION_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logUserEnrolled(const QString& userId, const QString& biometricType);
    void logAuthenticationAttempt(const QString& userId, bool success);
    void logMFAAuthentication(const QString& userId, bool success);

private:
    QString generateEventId() const;
};

#endif // BIOMETRIC_AUTHENTICATION_AUDIT_LOGGER_H

