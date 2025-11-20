#include "user_manager.h"
#include <QDebug>

UserManager::UserManager(QObject* parent)
    : QObject(parent)
{
}

UserManager::~UserManager() {
}

bool UserManager::initialize() {
    return true;
}

QList<QVariantMap> UserManager::getAllUsers() const {
    QList<QVariantMap> users;
    
    for (const QVariantMap& user : m_users.values()) {
        users.append(user);
    }
    
    return users;
}

QVariantMap UserManager::getUserInfo(const QString& userId) const {
    return m_users.value(userId, QVariantMap());
}

bool UserManager::updateUserStatus(const QString& userId, UserStatus status) {
    if (!m_users.contains(userId)) {
        return false;
    }

    QVariantMap& user = m_users[userId];
    user["status"] = static_cast<int>(status);
    
    emit userStatusUpdated(userId, status);
    
    return true;
}

bool UserManager::revokeUserAccess(const QString& userId) {
    if (!m_users.contains(userId)) {
        return false;
    }

    QVariantMap& user = m_users[userId];
    user["status"] = static_cast<int>(UserStatus::Revoked);
    
    emit userAccessRevoked(userId);
    emit userStatusUpdated(userId, UserStatus::Revoked);
    
    return true;
}

bool UserManager::restoreUserAccess(const QString& userId) {
    if (!m_users.contains(userId)) {
        return false;
    }

    QVariantMap& user = m_users[userId];
    user["status"] = static_cast<int>(UserStatus::Active);
    
    emit userAccessRestored(userId);
    emit userStatusUpdated(userId, UserStatus::Active);
    
    return true;
}

