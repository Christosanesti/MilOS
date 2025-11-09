#ifndef PASSWORDVALIDATOR_H
#define PASSWORDVALIDATOR_H

#include <QObject>
#include <QString>

class PasswordValidator : public QObject
{
    Q_OBJECT

public:
    explicit PasswordValidator(QObject *parent = nullptr);
    
    enum PasswordStrength {
        Weak = 0,
        Medium = 1,
        Strong = 2
    };
    Q_ENUM(PasswordStrength)
    
    Q_INVOKABLE int calculateStrength(const QString &password);
    Q_INVOKABLE QString strengthLabel(int strength);
    Q_INVOKABLE QString strengthColor(int strength);
    Q_INVOKABLE bool meetsRequirements(const QString &password);
    Q_INVOKABLE bool passwordsMatch(const QString &password1, const QString &password2);

signals:
    void strengthCalculated(int strength);
};

#endif // PASSWORDVALIDATOR_H

