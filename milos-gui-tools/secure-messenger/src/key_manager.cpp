#include "key_manager.h"
#include <QUuid>
#include <QDebug>

KeyManager::KeyManager(QObject* parent)
    : QObject(parent)
{
}

KeyManager::~KeyManager() {
}

bool KeyManager::initialize() {
    return true;
}

bool KeyManager::distributeKey(const USBKeyInfo& keyInfo, const QString& devicePath) {
    // Add key to managed keys
    m_keys.append(keyInfo);
    
    emit keyDistributed(keyInfo.keyId, devicePath);
    return true;
}

bool KeyManager::revokeKey(const QString& keyId) {
    // Find and revoke key
    for (int i = 0; i < m_keys.size(); i++) {
        if (m_keys[i].keyId == keyId) {
            m_keys[i].status = USBKeyStatus::Revoked;
            m_revokedKeys.append(keyId);
            emit keyRevoked(keyId);
            return true;
        }
    }
    
    return false;
}

USBKeyInfo KeyManager::getKeyInfo(const QString& keyId) const {
    for (const USBKeyInfo& key : m_keys) {
        if (key.keyId == keyId) {
            return key;
        }
    }
    
    return USBKeyInfo();
}

QList<USBKeyInfo> KeyManager::getAllKeys(const QString& userId) const {
    if (userId.isEmpty()) {
        return m_keys;
    }
    
    QList<USBKeyInfo> filtered;
    for (const USBKeyInfo& key : m_keys) {
        if (key.userId == userId) {
            filtered.append(key);
        }
    }
    
    return filtered;
}

QList<QString> KeyManager::getRevokedKeys() const {
    return m_revokedKeys;
}

QString KeyManager::generateDistributionId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

