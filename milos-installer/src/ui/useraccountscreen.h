#ifndef USERACCOUNTSCREEN_H
#define USERACCOUNTSCREEN_H

#include <QObject>
#include <QQuickItem>

class UserAccountScreen : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(bool usernameValid READ usernameValid NOTIFY usernameValidChanged)

public:
    explicit UserAccountScreen(QQuickItem *parent = nullptr);
    
    QString username() const { return m_username; }
    void setUsername(const QString &username);
    
    QString password() const { return m_password; }
    void setPassword(const QString &password);
    
    QString fullName() const { return m_fullName; }
    void setFullName(const QString &fullName);
    
    bool usernameValid() const { return m_usernameValid; }

signals:
    void usernameChanged();
    void usernameValidChanged();
    void passwordChanged();
    void fullNameChanged();
    void backClicked();
    void nextClicked();

private:
    void validateUsername();
    
    QString m_username;
    QString m_password;
    QString m_fullName;
    bool m_usernameValid;
};

#endif // USERACCOUNTSCREEN_H

