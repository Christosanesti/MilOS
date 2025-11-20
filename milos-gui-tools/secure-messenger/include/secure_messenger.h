#ifndef SECURE_MESSENGER_H
#define SECURE_MESSENGER_H

#include "usb_authorization.h"
#include "key_generator.h"
#include "key_manager.h"
#include "user_enrollment.h"
#include "role_manager.h"
#include "mesh_network.h"
#include "peer_discovery.h"
#include "network_manager.h"
#include "network_health.h"
#include "ethernet_enforcement.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>
#include <QString>

class SecureMessenger : public QObject {
    Q_OBJECT

public:
    explicit SecureMessenger(QObject* parent = nullptr);
    ~SecureMessenger();

    /**
     * @brief Initialize secure messenger
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start secure messenger
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop secure messenger
     */
    void stop();

    /**
     * @brief Get USB authorization
     */
    USBAuthorization* getUSBAuthorization() const { return m_usbAuth; }

    /**
     * @brief Get key generator
     */
    KeyGenerator* getKeyGenerator() const { return m_keyGen; }

    /**
     * @brief Get key manager
     */
    KeyManager* getKeyManager() const { return m_keyMgr; }

    /**
     * @brief Get user enrollment
     */
    UserEnrollment* getUserEnrollment() const { return m_enrollment; }

    /**
     * @brief Get role manager
     */
    RoleManager* getRoleManager() const { return m_roleMgr; }

    /**
     * @brief Get mesh network
     */
    MeshNetwork* getMeshNetwork() const { return m_meshNetwork; }

    /**
     * @brief Get network manager
     */
    NetworkManager* getNetworkManager() const { return m_networkManager; }

    /**
     * @brief Get network health monitor
     */
    NetworkHealthMonitor* getNetworkHealthMonitor() const { return m_healthMonitor; }

private:
    USBAuthorization* m_usbAuth;
    KeyGenerator* m_keyGen;
    KeyManager* m_keyMgr;
    UserEnrollment* m_enrollment;
    RoleManager* m_roleMgr;
    MeshNetwork* m_meshNetwork;
    PeerDiscovery* m_peerDiscovery;
    NetworkManager* m_networkManager;
    NetworkHealthMonitor* m_healthMonitor;
    EthernetEnforcement* m_enforcement;
    SecureMessengerDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
    bool m_running;
};

#endif // SECURE_MESSENGER_H

