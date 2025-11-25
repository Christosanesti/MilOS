#ifndef ANONYMITY_MANAGER_H
#define ANONYMITY_MANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDBusConnection>
#include <QDBusInterface>

/**
 * @brief Anonymity Layer Manager
 * 
 * Manages Tor and I2P connections with status monitoring and bandwidth tracking
 */
class AnonymityManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool torEnabled READ torEnabled NOTIFY torEnabledChanged)
    Q_PROPERTY(bool i2pEnabled READ i2pEnabled NOTIFY i2pEnabledChanged)
    Q_PROPERTY(QString torStatus READ torStatus NOTIFY torStatusChanged)
    Q_PROPERTY(QString i2pStatus READ i2pStatus NOTIFY i2pStatusChanged)
    Q_PROPERTY(QVariantMap torBandwidth READ torBandwidth NOTIFY torBandwidthChanged)
    Q_PROPERTY(QVariantMap i2pBandwidth READ i2pBandwidth NOTIFY i2pBandwidthChanged)
    Q_PROPERTY(QVariantMap routingInfo READ routingInfo NOTIFY routingInfoChanged)

public:
    explicit AnonymityManager(QObject *parent = nullptr);
    
    bool torEnabled() const { return m_torEnabled; }
    bool i2pEnabled() const { return m_i2pEnabled; }
    QString torStatus() const { return m_torStatus; }
    QString i2pStatus() const { return m_i2pStatus; }
    QVariantMap torBandwidth() const { return m_torBandwidth; }
    QVariantMap i2pBandwidth() const { return m_i2pBandwidth; }
    QVariantMap routingInfo() const { return m_routingInfo; }
    
    Q_INVOKABLE void toggleTor();
    Q_INVOKABLE void toggleI2P();
    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();
    Q_INVOKABLE void refreshStatus();

signals:
    void torEnabledChanged();
    void i2pEnabledChanged();
    void torStatusChanged();
    void i2pStatusChanged();
    void torBandwidthChanged();
    void i2pBandwidthChanged();
    void routingInfoChanged();
    void error(const QString &errorMessage);

private slots:
    void updateStatus();
    void updateBandwidth();

private:
    bool checkTorService();
    bool checkI2PService();
    void updateTorStatus();
    void updateI2PStatus();
    void updateTorBandwidth();
    void updateI2PBandwidth();
    void updateRoutingInfo();
    
    bool m_torEnabled;
    bool m_i2pEnabled;
    QString m_torStatus; // "disconnected", "connecting", "connected"
    QString m_i2pStatus;
    QVariantMap m_torBandwidth;
    QVariantMap m_i2pBandwidth;
    QVariantMap m_routingInfo;
    QTimer* m_statusTimer;
    QTimer* m_bandwidthTimer;
    QDBusInterface* m_torInterface;
    QDBusInterface* m_i2pInterface;
};

#endif // ANONYMITY_MANAGER_H

