#include "e2e_encryption.h"
#include <QUuid>
#include <QDebug>
#include <QCryptographicHash>

// In production, would include PQC Abstraction Layer from Epic 17
// #include <PQC_Abstraction_Layer.h>

E2EEncryption::E2EEncryption(QObject* parent)
    : QObject(parent)
{
}

E2EEncryption::~E2EEncryption() {
}

bool E2EEncryption::initialize() {
    if (!generateKeyPair()) {
        return false;
    }
    return true;
}

QByteArray E2EEncryption::encryptMessage(const QByteArray& messageData, const QString& recipientId) {
    QByteArray recipientKey = getRecipientPublicKey(recipientId);
    if (recipientKey.isEmpty()) {
        emit encryptionFailed("Recipient public key not found");
        return QByteArray();
    }

    // In production, would use PQC Abstraction Layer to encrypt
    // For now, placeholder encryption
    QByteArray encrypted = messageData;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(messageData);
    hash.addData(recipientKey);
    encrypted.append(hash.result());

    return encrypted;
}

QByteArray E2EEncryption::decryptMessage(const QByteArray& encryptedData, const QString& senderId) {
    // In production, would use PQC Abstraction Layer to decrypt
    // For now, placeholder decryption
    if (encryptedData.size() < 32) {
        emit decryptionFailed("Invalid encrypted data");
        return QByteArray();
    }

    QByteArray decrypted = encryptedData.left(encryptedData.size() - 32);
    return decrypted;
}

QByteArray E2EEncryption::encryptFile(const QByteArray& fileData, const QString& recipientId) {
    return encryptMessage(fileData, recipientId);
}

QByteArray E2EEncryption::decryptFile(const QByteArray& encryptedData, const QString& senderId) {
    return decryptMessage(encryptedData, senderId);
}

QByteArray E2EEncryption::encryptMedia(const QByteArray& mediaData, const QString& recipientId) {
    return encryptMessage(mediaData, recipientId);
}

QByteArray E2EEncryption::decryptMedia(const QByteArray& encryptedData, const QString& senderId) {
    return decryptMessage(encryptedData, senderId);
}

bool E2EEncryption::generateKeyPair() {
    // In production, would use PQC Abstraction Layer to generate key pair
    // For now, generate placeholder keys
    m_localPrivateKey = QUuid::createUuid().toByteArray();
    m_localPublicKey = QUuid::createUuid().toByteArray();
    return true;
}

QByteArray E2EEncryption::getRecipientPublicKey(const QString& recipientId) const {
    return m_recipientKeys.value(recipientId, QByteArray());
}

void E2EEncryption::setRecipientPublicKey(const QString& recipientId, const QByteArray& publicKey) {
    m_recipientKeys[recipientId] = publicKey;
}

