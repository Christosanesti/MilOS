#include "passwordvalidator.h"
#include <QRegularExpression>

PasswordValidator::PasswordValidator(QObject *parent)
    : QObject(parent)
{
}

int PasswordValidator::calculateStrength(const QString &password)
{
    if (password.length() == 0) {
        return Weak;
    }
    
    int strength = 0;
    if (password.length() >= 12) strength += 1;
    if (password.length() >= 16) strength += 1;
    if (QRegularExpression("[a-z]").match(password).hasMatch()) strength += 1;
    if (QRegularExpression("[A-Z]").match(password).hasMatch()) strength += 1;
    if (QRegularExpression("[0-9]").match(password).hasMatch()) strength += 1;
    if (QRegularExpression("[^a-zA-Z0-9]").match(password).hasMatch()) strength += 1;
    
    if (strength < 3) {
        return Weak;
    } else if (strength < 5) {
        return Medium;
    } else {
        return Strong;
    }
}

QString PasswordValidator::strengthLabel(int strength)
{
    switch (strength) {
        case Weak:
            return "Weak password";
        case Medium:
            return "Medium strength";
        case Strong:
            return "Strong password";
        default:
            return "Weak password";
    }
}

QString PasswordValidator::strengthColor(int strength)
{
    switch (strength) {
        case Weak:
            return "#ff4444";
        case Medium:
            return "#ffaa00";
        case Strong:
            return "#00cc66";
        default:
            return "#ff4444";
    }
}

bool PasswordValidator::meetsRequirements(const QString &password)
{
    if (password.length() < 12) {
        return false;
    }
    if (!QRegularExpression("[a-z]").match(password).hasMatch()) {
        return false;
    }
    if (!QRegularExpression("[A-Z]").match(password).hasMatch()) {
        return false;
    }
    if (!QRegularExpression("[0-9]").match(password).hasMatch()) {
        return false;
    }
    if (!QRegularExpression("[^a-zA-Z0-9]").match(password).hasMatch()) {
        return false;
    }
    return true;
}

bool PasswordValidator::passwordsMatch(const QString &password1, const QString &password2)
{
    return password1 == password2 && password1.length() > 0;
}

