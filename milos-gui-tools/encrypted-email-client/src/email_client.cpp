#include "email_client.h"
#include <QUuid>
#include <QCryptographicHash>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

EmailClient::EmailClient(QObject* parent)
    : QObject(parent)
    , m_connected(false)
{
}

EmailClient::~EmailClient() {
}

bool EmailClient::initialize() {
    return generateEncryptionKey();
}

bool EmailClient::addAccount(const EmailAccount& account) {
    m_accounts[account.id] = account;
    return true;
}

bool EmailClient::connectToServer(const QString& accountId) {
    if (!m_accounts.contains(accountId)) {
        return false;
    }
    
    // In production, would connect to actual email server
    // For now, simulate connection
    m_connected = true;
    emit connectionChanged(true);
    
    return true;
}

void EmailClient::disconnectFromServer(const QString& accountId) {
    Q_UNUSED(accountId);
    
    m_connected = false;
    emit connectionChanged(false);
}

bool EmailClient::sendEmail(const EmailMessage& message) {
    if (!m_connected) {
        return false;
    }
    
    // Encrypt email before sending
    QByteArray encrypted = encryptEmail(message);
    
    // In production, would send via SMTP
    // For now, store locally
    m_messages[message.id] = message;
    
    emit emailSent(message.id);
    
    return true;
}

QList<EmailMessage> EmailClient::receiveEmails(const QString& accountId, int limit) {
    Q_UNUSED(accountId);
    
    QList<EmailMessage> emails;
    
    // In production, would fetch from IMAP/POP3 server
    // For now, return stored messages
    int count = 0;
    for (auto it = m_messages.begin(); it != m_messages.end() && count < limit; ++it, ++count) {
        emails.append(it.value());
    }
    
    return emails;
}

EmailMessage EmailClient::getEmail(const QString& messageId) const {
    return m_messages.value(messageId, EmailMessage());
}

QByteArray EmailClient::encryptEmail(const EmailMessage& message) {
    // Serialize email to JSON
    QJsonObject json;
    json["id"] = message.id;
    json["from"] = message.from;
    json["to"] = QJsonArray::fromStringList(message.to);
    json["subject"] = message.subject;
    json["body"] = message.body;
    json["sent_at"] = message.sentAt.toString(Qt::ISODate);
    
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    
    // In production, would use proper encryption (AES-256 or PQC)
    // For now, simple XOR encryption as placeholder
    QByteArray encrypted = data;
    for (int i = 0; i < encrypted.size(); ++i) {
        encrypted[i] = encrypted[i] ^ m_encryptionKey[i % m_encryptionKey.size()];
    }
    
    return encrypted;
}

EmailMessage EmailClient::decryptEmail(const QByteArray& encryptedData) {
    if (encryptedData.isEmpty()) {
        return EmailMessage();
    }
    
    // Decrypt
    QByteArray decrypted = encryptedData;
    for (int i = 0; i < decrypted.size(); ++i) {
        decrypted[i] = decrypted[i] ^ m_encryptionKey[i % m_encryptionKey.size()];
    }
    
    // Parse JSON
    QJsonDocument doc = QJsonDocument::fromJson(decrypted);
    if (doc.isNull()) {
        return EmailMessage();
    }
    
    QJsonObject json = doc.object();
    EmailMessage message;
    message.id = json["id"].toString();
    message.from = json["from"].toString();
    
    QJsonArray toArray = json["to"].toArray();
    for (const QJsonValue& value : toArray) {
        message.to.append(value.toString());
    }
    
    message.subject = json["subject"].toString();
    message.body = json["body"].toString();
    message.sentAt = QDateTime::fromString(json["sent_at"].toString(), Qt::ISODate);
    message.isEncrypted = true;
    
    return message;
}

bool EmailClient::generateEncryptionKey() {
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData("milos-email-encryption-key");
    m_encryptionKey = hash.result();
    return true;
}

