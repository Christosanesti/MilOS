#include "useraccountmanager.h"
#include <QRegularExpression>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <unistd.h>
#include <pwd.h>
#include <crypt.h>
#include <cstdlib>
#include <ctime>

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
    // Check if username is available (not already in use)
    // Check /etc/passwd
    struct passwd *pwd = getpwnam(username.toLocal8Bit().constData());
    if (pwd != nullptr) {
        return false; // Username already exists
    }
    
    // Also check using getpwnam via QProcess as fallback
    QProcess process;
    process.start("id", QStringList() << "-u" << username);
    process.waitForFinished(1000);
    if (process.exitCode() == 0) {
        return false; // User exists
    }
    
    return true; // Username is available
}

bool UserAccountManager::createUserAccount(const QString &username, const QString &password, const QString &fullName)
{
    if (!validateUsername(username)) {
        emit userAccountError("Invalid username");
        return false;
    }
    
    if (!usernameAvailable(username)) {
        emit userAccountError("Username already exists");
        return false;
    }
    
    // Hash the password securely using crypt
    QString hashedPassword = hashPassword(password);
    if (hashedPassword.isEmpty()) {
        emit userAccountError("Failed to hash password");
        return false;
    }
    
    // Create user account using useradd command
    QProcess process;
    QStringList args;
    args << "-m"; // Create home directory
    args << "-s" << "/bin/bash"; // Set shell
    if (!fullName.isEmpty()) {
        args << "-c" << fullName; // Comment (full name)
    }
    args << username;
    
    process.start("useradd", args);
    process.waitForFinished(5000);
    
    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        emit userAccountError(QString("Failed to create user: %1").arg(error));
        return false;
    }
    
    // Set password using chpasswd or passwd
    QProcess passwdProcess;
    passwdProcess.start("chpasswd");
    passwdProcess.waitForStarted();
    
    if (passwdProcess.state() == QProcess::Running) {
        QString passwordLine = QString("%1:%2\n").arg(username).arg(password);
        passwdProcess.write(passwordLine.toLocal8Bit());
        passwdProcess.closeWriteChannel();
        passwdProcess.waitForFinished(5000);
        
        if (passwdProcess.exitCode() != 0) {
            // Fallback: use passwd in non-interactive mode
            QProcess fallbackProcess;
            fallbackProcess.start("passwd", QStringList() << username);
            fallbackProcess.waitForStarted();
            if (fallbackProcess.state() == QProcess::Running) {
                fallbackProcess.write((password + "\n").toLocal8Bit());
                fallbackProcess.write((password + "\n").toLocal8Bit());
                fallbackProcess.closeWriteChannel();
                fallbackProcess.waitForFinished(5000);
            }
        }
    }
    
    // Add user to default groups (wheel for sudo, audio, video, etc.)
    QProcess groupProcess;
    groupProcess.start("usermod", QStringList() << "-aG" << "wheel,audio,video" << username);
    groupProcess.waitForFinished(2000);
    
    emit userAccountCreated();
    return true;
}

QString UserAccountManager::hashPassword(const QString &password)
{
    // Generate a random salt and hash the password
    // Using SHA-512 (id 6) for better security
    QString salt = generateSalt();
    if (salt.isEmpty()) {
        return QString();
    }
    
    // Use crypt to hash the password
    char *hashed = crypt(password.toLocal8Bit().constData(), 
                        QString("$6$%1$").arg(salt).toLocal8Bit().constData());
    if (hashed == nullptr) {
        return QString();
    }
    
    return QString::fromLocal8Bit(hashed);
}

QString UserAccountManager::generateSalt()
{
    // Generate a random 16-character salt
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    const int saltLength = 16;
    QString salt;
    
    QFile urandom("/dev/urandom");
    if (urandom.open(QIODevice::ReadOnly)) {
        QByteArray randomBytes = urandom.read(saltLength);
        for (int i = 0; i < saltLength && i < randomBytes.size(); ++i) {
            unsigned char byte = static_cast<unsigned char>(randomBytes[i]);
            salt += charset[byte % (sizeof(charset) - 1)];
        }
        urandom.close();
    } else {
        // Fallback: use current time as seed (less secure)
        qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
        for (int i = 0; i < saltLength; ++i) {
            salt += charset[qrand() % (sizeof(charset) - 1)];
        }
    }
    
    return salt;
}

