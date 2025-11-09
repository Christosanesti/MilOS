#include "useraccountmanager.h"
#include <QRegularExpression>

UserAccountManager::UserAccountManager(QObject *parent)
    : QObject(parent)
{
}

bool UserAccountManager::validateUsername(const QString &username)
{
    if (username.length() < 3 || username.length() > 32) {
        return false;
    }
    if (!QRegularExpression("^[a-zA-Z0-9_]+$").match(username).hasMatch()) {
        return false;
    }
    if (!QRegularExpression("^[a-zA-Z]").match(username).hasMatch()) {
        return false;
    }
    return true;
}

bool UserAccountManager::usernameAvailable(const QString &username)
{
    // TODO: Check if username is available (not already in use)
    // This would check /etc/passwd or use system APIs
    return true;
}

bool UserAccountManager::createUserAccount(const QString &username, const QString &password, const QString &fullName)
{
    if (!validateUsername(username)) {
        emit userAccountError("Invalid username");
        return false;
    }
    
    // TODO: Implement actual user account creation
    // This would:
    // 1. Hash the password securely
    // 2. Create user account using system APIs (useradd, etc.)
    // 3. Set up home directory
    // 4. Configure user groups
    // 5. Emit userAccountCreated() on success
    // 6. Emit userAccountError() on failure
    
    emit userAccountCreated();
    return true;
}

