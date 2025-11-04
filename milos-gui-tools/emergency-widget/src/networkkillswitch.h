#ifndef NETWORKKILLSWITCH_H
#define NETWORKKILLSWITCH_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingReply>

/**
 * @brief Network kill switch implementation
 * 
 * This class provides functionality to disable all network interfaces
 * via NetworkManager or systemd-networkd D-Bus interface.
 */
class NetworkKillSwitch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkEnabled READ isNetworkEnabled NOTIFY networkStatusChanged)

public:
    explicit NetworkKillSwitch(QObject *parent = nullptr);
    ~NetworkKillSwitch() = default;

    /**
     * @brief Check if network is currently enabled
     * @return True if network is enabled
     */
    bool isNetworkEnabled() const { return m_networkEnabled; }

    /**
     * @brief Check if NetworkManager is available
     * @return True if NetworkManager service is available
     */
    Q_INVOKABLE bool isNetworkManagerAvailable() const;

    /**
     * @brief Check if systemd-networkd is available
     * @return True if systemd-networkd service is available
     */
    Q_INVOKABLE bool isSystemdNetworkdAvailable() const;

public slots:
    /**
     * @brief Disable all network interfaces (kill switch)
     * @return True if operation successful
     */
    Q_INVOKABLE bool disableNetwork();

    /**
     * @brief Check current network status
     */
    Q_INVOKABLE void checkNetworkStatus();

    /**
     * @brief Log network kill switch action to audit service
     * @param actionType Action type identifier
     * @param eventData Event data dictionary
     */
    Q_INVOKABLE void logAuditEvent(const QString &actionType, const QVariantMap &eventData);

signals:
    /**
     * @brief Emitted when network status changes
     * @param enabled True if network is enabled
     */
    void networkStatusChanged(bool enabled);

    /**
     * @brief Emitted when network disable operation completes
     * @param success True if operation successful
     * @param errorMessage Error message if failed
     */
    void networkDisableCompleted(bool success, const QString &errorMessage);

private:
    /**
     * @brief Disable network via NetworkManager
     * @return True if successful
     */
    bool disableNetworkViaNetworkManager();

    /**
     * @brief Disable network via systemd-networkd
     * @return True if successful
     */
    bool disableNetworkViaSystemd();

    /**
     * @brief Check network status via NetworkManager
     * @return True if network is enabled
     */
    bool checkNetworkStatusViaNetworkManager();

    /**
     * @brief Check network status via systemd-networkd
     * @return True if network is enabled
     */
    bool checkNetworkStatusViaSystemd();

    bool m_networkEnabled;
    QDBusConnection m_dbusConnection;
};

#endif // NETWORKKILLSWITCH_H

