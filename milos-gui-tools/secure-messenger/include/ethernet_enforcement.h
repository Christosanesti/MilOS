#ifndef ETHERNET_ENFORCEMENT_H
#define ETHERNET_ENFORCEMENT_H

#include <QObject>
#include <QString>
#include <QList>

/**
 * @brief Ethernet Enforcement
 * 
 * Enforces ethernet-only operation (no WiFi, no cellular, no internet).
 */
class EthernetEnforcement : public QObject {
    Q_OBJECT

public:
    explicit EthernetEnforcement(QObject* parent = nullptr);
    ~EthernetEnforcement();

    /**
     * @brief Initialize ethernet enforcement
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if network interface is authorized
     * @param interfaceName Network interface name
     * @return true if authorized (ethernet), false otherwise
     */
    bool isInterfaceAuthorized(const QString& interfaceName) const;

    /**
     * @brief Block unauthorized interfaces
     * @return true if blocking successful, false otherwise
     */
    bool blockUnauthorizedInterfaces();

    /**
     * @brief Check internet connectivity
     * @return true if internet is accessible, false otherwise
     */
    bool checkInternetConnectivity() const;

    /**
     * @brief Block internet connectivity
     * @return true if blocking successful, false otherwise
     */
    bool blockInternetConnectivity();

Q_SIGNALS:
    /**
     * @brief Emitted when unauthorized interface is detected
     */
    void unauthorizedInterfaceDetected(const QString& interfaceName);

    /**
     * @brief Emitted when internet connectivity is detected
     */
    void internetConnectivityDetected();

private:
    QList<QString> m_authorizedInterfaces;
    
    bool isEthernetInterface(const QString& interfaceName) const;
    bool isWiFiInterface(const QString& interfaceName) const;
    bool isCellularInterface(const QString& interfaceName) const;
};

#endif // ETHERNET_ENFORCEMENT_H

