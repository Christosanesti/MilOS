#include "radio_encryption.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>

RadioEncryption::RadioEncryption(QObject* parent)
    : QObject(parent)
{
}

RadioEncryption::~RadioEncryption() {
}

bool RadioEncryption::initialize() {
    return true;
}

QByteArray RadioEncryption::encrypt(const QByteArray& payload, const QByteArray& key) const {
    return applyEncryption(payload, key, true);
}

QByteArray RadioEncryption::decrypt(const QByteArray& encryptedData, const QByteArray& key) const {
    return applyEncryption(encryptedData, key, false);
}

QByteArray RadioEncryption::generateKey(int length) const {
    QByteArray key;
    key.resize(length);
    
    for (int i = 0; i < length; ++i) {
        key[i] = QRandomGenerator::global()->bounded(256);
    }
    
    return key;
}

QByteArray RadioEncryption::applyEncryption(const QByteArray& data, const QByteArray& key, bool encrypt) const {
    if (key.isEmpty()) {
        return data;
    }
    
    // In production, would use proper encryption (AES, etc.)
    // For now, simple XOR-based encryption
    QByteArray result = data;
    
    for (int i = 0; i < result.size(); ++i) {
        result[i] = result[i] ^ key[i % key.size()];
    }
    
    return result;
}

