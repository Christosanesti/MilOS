#include "key_generator.h"
#include <QUuid>
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

KeyGenerator::KeyGenerator(QObject* parent)
    : QObject(parent)
{
}

KeyGenerator::~KeyGenerator() {
}

bool KeyGenerator::initialize() {
    // Generate or load master key
    // In production, this would be stored securely
    m_masterKey = generateRandomKey(32);  // 256-bit key
    return true;
}

USBKeyInfo KeyGenerator::generateKey(const QString& userId,
                                     UserRole role,
                                     int expirationDays) {
    USBKeyInfo keyInfo;
    keyInfo.keyId = generateKeyId();
    keyInfo.userId = userId;
    keyInfo.role = role;
    keyInfo.status = USBKeyStatus::Pending;
    keyInfo.createdAt = QDateTime::currentDateTime();
    
    if (expirationDays > 0) {
        keyInfo.expiresAt = keyInfo.createdAt.addDays(expirationDays);
    }
    
    // Derive key from master key
    keyInfo.keyData = deriveKey(m_masterKey, userId);
    
    emit keyGenerated(keyInfo.keyId);
    
    return keyInfo;
}

QByteArray KeyGenerator::deriveKey(const QByteArray& masterKey, const QString& userId) const {
    // Use HKDF-like key derivation
    // In production, use proper PQC key derivation (Epic 17)
    QByteArray input = masterKey + userId.toUtf8();
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(input);
    return hash.result();
}

bool KeyGenerator::writeKeyToUSB(const USBKeyInfo& keyInfo, const QString& devicePath) {
    QFileInfo info(devicePath);
    if (!info.exists() || !info.isWritable()) {
        return false;
    }
    
    // Write key to USB device
    // Create a hidden file on the USB device
    QString keyFilePath;
    if (info.isDir()) {
        keyFilePath = QDir(devicePath).absoluteFilePath(".milos_key");
    } else {
        // For block devices, would need to mount first
        return false;
    }
    
    QFile keyFile(keyFilePath);
    if (!keyFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QTextStream stream(&keyFile);
    stream << keyInfo.keyId << "\n";
    stream << keyInfo.userId << "\n";
    stream << keyInfo.keyData.toBase64() << "\n";
    stream << static_cast<int>(keyInfo.role) << "\n";
    stream << keyInfo.createdAt.toString(Qt::ISODate) << "\n";
    if (keyInfo.expiresAt.isValid()) {
        stream << keyInfo.expiresAt.toString(Qt::ISODate) << "\n";
    }
    
    keyFile.close();
    
    emit keyWrittenToUSB(keyInfo.keyId, devicePath);
    return true;
}

QString KeyGenerator::generateKeyId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QByteArray KeyGenerator::generateRandomKey(int length) const {
    QByteArray key;
    key.resize(length);
    
    QRandomGenerator* rng = QRandomGenerator::global();
    for (int i = 0; i < length; i++) {
        key[i] = static_cast<char>(rng->bounded(256));
    }
    
    return key;
}

