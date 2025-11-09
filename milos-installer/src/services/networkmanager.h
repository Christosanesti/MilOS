#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>

class NetworkManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantList networkInterfaces READ networkInterfaces NOTIFY networkInterfacesChanged)

public:
    explicit NetworkManager(QObject *parent = nullptr);
    
    bool scanning() const { return m_scanning; }
    QVariantList networkInterfaces() const { return m_networkInterfaces; }
    
    Q_INVOKABLE void scanInterfaces();
    Q_INVOKABLE void configureDHCP(const QString &interface);
    Q_INVOKABLE void configureManual(const QString &interface, const QString &ip, const QString &subnet, const QString &gateway, const QString &dns);
    Q_INVOKABLE bool testConnection();

signals:
    void scanningChanged();
    void networkInterfacesChanged();
    void configurationComplete();
    void configurationError(const QString &error);

private:
    bool m_scanning;
    QVariantList m_networkInterfaces;
    
    // TODO: Integrate with systemd-networkd or NetworkManager
};

#endif // NETWORKMANAGER_H

