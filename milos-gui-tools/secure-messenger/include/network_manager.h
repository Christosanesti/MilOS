#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QList>
#include <QVariantMap>

/**
 * @brief Network Interface Type
 */
enum class NetworkInterfaceType {
    Ethernet,
    WiFi,
    Cellular,
    VPN,
    Bridge
};

/**
 * @brief Network Interface Information
 */
struct NetworkInterfaceInfo {
    QString name;
    NetworkInterfaceType type;
    QHostAddress address;
    QString subnet;
    bool isActive;
    bool isAuthorized;
};

/**
 * @brief Network Manager
 * 
 * Manages network interfaces, segmentation, and VPN/bridge connections.
 */
class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    /**
     * @brief Initialize network manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get network interfaces
     * @return List of network interface information
     */
    QList<NetworkInterfaceInfo> getNetworkInterfaces() const;

    /**
     * @brief Get authorized network interfaces
     * @return List of authorized network interfaces
     */
    QList<NetworkInterfaceInfo> getAuthorizedInterfaces() const;

    /**
     * @brief Detect network segmentation
     * @return List of detected subnets
     */
    QList<QString> detectSegmentation() const;

    /**
     * @brief Connect VPN
     * @param vpnConfig VPN configuration
     * @return true if connection successful, false otherwise
     */
    bool connectVPN(const QVariantMap& vpnConfig);

    /**
     * @brief Disconnect VPN
     * @return true if disconnection successful, false otherwise
     */
    bool disconnectVPN();

    /**
     * @brief Create bridge
     * @param bridgeConfig Bridge configuration
     * @return true if creation successful, false otherwise
     */
    bool createBridge(const QVariantMap& bridgeConfig);

    /**
     * @brief Remove bridge
     * @param bridgeName Bridge name
     * @return true if removal successful, false otherwise
     */
    bool removeBridge(const QString& bridgeName);

Q_SIGNALS:
    /**
     * @brief Emitted when network interface is detected
     */
    void interfaceDetected(const QString& interfaceName);

    /**
     * @brief Emitted when VPN is connected
     */
    void vpnConnected();

    /**
     * @brief Emitted when VPN is disconnected
     */
    void vpnDisconnected();

private:
    QList<NetworkInterfaceInfo> m_interfaces;
    bool m_vpnConnected;
    
    void scanNetworkInterfaces();
    bool isEthernetInterface(const QString& interfaceName) const;
};

#endif // NETWORK_MANAGER_H

