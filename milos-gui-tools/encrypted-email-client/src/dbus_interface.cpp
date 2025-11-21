#include "dbus_interface.h"
#include "email_client.h"
#include <QDebug>
#include <QDBusConnection>

EncryptedEmailDBusInterface::EncryptedEmailDBusInterface(QObject* parent)
    : QObject(parent)
    , m_emailClient(nullptr)
{
}

EncryptedEmailDBusInterface::~EncryptedEmailDBusInterface() {
}

bool EncryptedEmailDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.EncryptedEmail")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/EncryptedEmail", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void EncryptedEmailDBusInterface::setEmailClient(EmailClient* client) {
    m_emailClient = client;
}

bool EncryptedEmailDBusInterface::AddAccount(const QString& email, const QString& server, int port, const QString& protocol) {
    if (!m_emailClient) {
        return false;
    }
    
    EmailAccount account;
    account.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    account.email = email;
    account.server = server;
    account.port = port;
    account.protocol = protocol;
    account.useEncryption = true;
    account.encryptionMethod = "TLS";
    
    return m_emailClient->addAccount(account);
}

bool EncryptedEmailDBusInterface::ConnectToServer(const QString& accountId) {
    if (!m_emailClient) {
        return false;
    }
    
    return m_emailClient->connectToServer(accountId);
}

void EncryptedEmailDBusInterface::DisconnectFromServer(const QString& accountId) {
    if (m_emailClient) {
        m_emailClient->disconnectFromServer(accountId);
    }
}

bool EncryptedEmailDBusInterface::SendEmail(const QString& to, const QString& subject, const QString& body) {
    if (!m_emailClient) {
        return false;
    }
    
    EmailMessage message;
    message.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    message.to = QStringList() << to;
    message.subject = subject;
    message.body = body;
    message.sentAt = QDateTime::currentDateTime();
    message.isEncrypted = true;
    
    return m_emailClient->sendEmail(message);
}

QStringList EncryptedEmailDBusInterface::ReceiveEmails(const QString& accountId, int limit) {
    QStringList result;
    
    if (!m_emailClient) {
        return result;
    }
    
    QList<EmailMessage> emails = m_emailClient->receiveEmails(accountId, limit);
    for (const EmailMessage& email : emails) {
        result.append(email.id);
    }
    
    return result;
}

QVariantMap EncryptedEmailDBusInterface::GetEmail(const QString& messageId) {
    QVariantMap result;
    
    if (!m_emailClient) {
        return result;
    }
    
    EmailMessage email = m_emailClient->getEmail(messageId);
    
    result["id"] = email.id;
    result["from"] = email.from;
    result["to"] = email.to;
    result["subject"] = email.subject;
    result["body"] = email.body;
    result["sent_at"] = email.sentAt.toString(Qt::ISODate);
    result["is_encrypted"] = email.isEncrypted;
    
    return result;
}

