#ifndef ENCRYPTED_EMAIL_DBUS_INTERFACE_H
#define ENCRYPTED_EMAIL_DBUS_INTERFACE_H

#include "email_client.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class EmailClient;

/**
 * @brief Encrypted Email D-Bus Interface
 */
class EncryptedEmailDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.EncryptedEmail")

public:
    explicit EncryptedEmailDBusInterface(QObject* parent = nullptr);
    ~EncryptedEmailDBusInterface();

    bool initialize();
    void setEmailClient(EmailClient* client);

public slots:
    bool AddAccount(const QString& email, const QString& server, int port, const QString& protocol);
    bool ConnectToServer(const QString& accountId);
    void DisconnectFromServer(const QString& accountId);
    bool SendEmail(const QString& to, const QString& subject, const QString& body);
    QStringList ReceiveEmails(const QString& accountId, int limit);
    QVariantMap GetEmail(const QString& messageId);

private:
    EmailClient* m_emailClient;
};

#endif // ENCRYPTED_EMAIL_DBUS_INTERFACE_H

