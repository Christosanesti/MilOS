#ifndef USERACCOUNTMANAGER_H
#define USERACCOUNTMANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>

class UserAccountManager : public QObject
{
    Q_OBJECT

public:
    explicit UserAccountManager(QObject *parent = nullptr);
    
    Q_INVOKABLE bool validateUsername(const QString &username);
    Q_INVOKABLE bool createUserAccount(const QString &username, const QString &password, const QString &fullName);
    Q_INVOKABLE bool usernameAvailable(const QString &username);

signals:
    void userAccountCreated();
    void userAccountError(const QString &error);

private:
    QString hashPassword(const QString &password);
    QString generateSalt();
};

#endif // USERACCOUNTMANAGER_H

