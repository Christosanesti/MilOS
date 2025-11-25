#ifndef NETWORKKILLSWITCH_H
#define NETWORKKILLSWITCH_H

#include <QObject>
#include <QString>
#include <QStringList>

class NetworkKillSwitch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkEnabled READ networkEnabled NOTIFY networkEnabledChanged)
    Q_PROPERTY(bool requiresConfirmation READ requiresConfirmation WRITE setRequiresConfirmation NOTIFY requiresConfirmationChanged)

public:
    explicit NetworkKillSwitch(QObject *parent = nullptr);
    
    bool networkEnabled() const { return m_networkEnabled; }
    bool requiresConfirmation() const { return m_requiresConfirmation; }
    void setRequiresConfirmation(bool requires);
    
    Q_INVOKABLE void disableNetwork();
    Q_INVOKABLE void enableNetwork();
    Q_INVOKABLE void checkNetworkStatus();
    Q_INVOKABLE QStringList getNetworkInterfaces();

signals:
    void networkEnabledChanged();
    void requiresConfirmationChanged();
    void networkDisabled();
    void networkEnabled();
    void error(const QString &errorMessage);

private:
    bool m_networkEnabled;
    bool m_requiresConfirmation;
    QStringList m_disabledInterfaces;
    
    // NetworkManager and systemd-networkd D-Bus integration
    bool disableNetworkManager();
    bool disableSystemdNetworkd();
    bool enableNetworkManager();
    bool enableSystemdNetworkd();
};

#endif // NETWORKKILLSWITCH_H

