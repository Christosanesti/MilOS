#ifndef SECUREMODE_H
#define SECUREMODE_H

#include <QObject>
#include <QStringList>

class SecureMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

public:
    explicit SecureMode(QObject *parent = nullptr);
    
    bool active() const { return m_active; }
    
    Q_INVOKABLE void activateSecureMode();
    Q_INVOKABLE void deactivateSecureMode();
    Q_INVOKABLE QStringList getNonEssentialServices();

signals:
    void activeChanged();
    void secureModeActivated();
    void secureModeDeactivated();
    void error(const QString &errorMessage);

private:
    bool m_active;
    QStringList m_stoppedServices;
    
    bool stopService(const QString &serviceName);
    bool startService(const QString &serviceName);
    QStringList getNonEssentialServiceList();
};

#endif // SECUREMODE_H

