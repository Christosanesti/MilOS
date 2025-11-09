#include "useraccountscreen.h"
#include <QRegularExpression>

UserAccountScreen::UserAccountScreen(QQuickItem *parent)
    : QQuickItem(parent)
    , m_username("")
    , m_password("")
    , m_fullName("")
    , m_usernameValid(false)
{
}

void UserAccountScreen::setUsername(const QString &username)
{
    if (m_username != username) {
        m_username = username;
        validateUsername();
        emit usernameChanged();
    }
}

void UserAccountScreen::setPassword(const QString &password)
{
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}

void UserAccountScreen::setFullName(const QString &fullName)
{
    if (m_fullName != fullName) {
        m_fullName = fullName;
        emit fullNameChanged();
    }
}

void UserAccountScreen::validateUsername()
{
    bool wasValid = m_usernameValid;
    
    if (m_username.length() < 3 || m_username.length() > 32) {
        m_usernameValid = false;
    } else if (!QRegularExpression("^[a-zA-Z0-9_]+$").match(m_username).hasMatch()) {
        m_usernameValid = false;
    } else if (!QRegularExpression("^[a-zA-Z]").match(m_username).hasMatch()) {
        m_usernameValid = false;
    } else {
        m_usernameValid = true;
    }
    
    if (wasValid != m_usernameValid) {
        emit usernameValidChanged();
    }
}

