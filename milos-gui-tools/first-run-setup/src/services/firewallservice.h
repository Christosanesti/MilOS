#ifndef FIREWALLSERVICE_H
#define FIREWALLSERVICE_H

#include <QObject>
#include <QString>

class FirewallService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit FirewallService(QObject *parent = nullptr);
    
    QString status() const { return m_status; }
    
    Q_INVOKABLE void checkStatus();
    Q_INVOKABLE bool isConfigured();
    Q_INVOKABLE bool isActive();

signals:
    void statusChanged();

private:
    QString m_status; // not_configured, inactive, active
    
    // TODO: Integrate with iptables/nftables or firewall service
};

#endif // FIREWALLSERVICE_H

