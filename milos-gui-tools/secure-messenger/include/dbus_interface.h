#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QDBusContext>

class USBAuthorization;
class KeyGenerator;
class KeyManager;
class UserEnrollment;
class RoleManager;
class MeshNetwork;
class PeerDiscovery;
class NetworkManager;
class NetworkHealthMonitor;
class EthernetEnforcement;

/**
 * @brief D-Bus Interface for Secure Messenger
 * 
 * Provides D-Bus interface for USB authorization and key management operations.
 */
class SecureMessengerDBusInterface : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SecureMessenger")

public:
    explicit SecureMessengerDBusInterface(QObject* parent = nullptr);
    ~SecureMessengerDBusInterface();

    /**
     * @brief Set USB authorization
     */
    void setUSBAuthorization(USBAuthorization* usbAuth);

    /**
     * @brief Set key generator
     */
    void setKeyGenerator(KeyGenerator* keyGen);

    /**
     * @brief Set key manager
     */
    void setKeyManager(KeyManager* keyMgr);

    /**
     * @brief Set user enrollment
     */
    void setUserEnrollment(UserEnrollment* enrollment);

    /**
     * @brief Set role manager
     */
    void setRoleManager(RoleManager* roleMgr);

    /**
     * @brief Set mesh network
     */
    void setMeshNetwork(MeshNetwork* meshNetwork);

    /**
     * @brief Set peer discovery
     */
    void setPeerDiscovery(PeerDiscovery* peerDiscovery);

    /**
     * @brief Set network manager
     */
    void setNetworkManager(NetworkManager* networkManager);

    /**
     * @brief Set network health monitor
     */
    void setNetworkHealthMonitor(NetworkHealthMonitor* healthMonitor);

    /**
     * @brief Set ethernet enforcement
     */
    void setEthernetEnforcement(EthernetEnforcement* enforcement);

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Detect USB devices
     * @return JSON string with list of USB device paths
     */
    QString DetectUSBDevices();

    /**
     * @brief Validate USB key
     * @param devicePath USB device path
     * @return JSON string with key information or empty if invalid
     */
    QString ValidateUSBKey(const QString& devicePath);

    /**
     * @brief Generate key
     * @param userId User ID
     * @param role Role (0=CommandingOfficer, 1=TechPersonnel, 2=User)
     * @param expirationDays Expiration in days (0 for no expiration)
     * @return Key ID or empty string if failed
     */
    QString GenerateKey(const QString& userId, int role, int expirationDays);

    /**
     * @brief Write key to USB
     * @param keyId Key ID
     * @param devicePath USB device path
     * @return true if write successful, false otherwise
     */
    bool WriteKeyToUSB(const QString& keyId, const QString& devicePath);

    /**
     * @brief Distribute key
     * @param keyId Key ID
     * @param devicePath USB device path
     * @return true if distribution successful, false otherwise
     */
    bool DistributeKey(const QString& keyId, const QString& devicePath);

    /**
     * @brief Revoke key
     * @param keyId Key ID
     * @return true if revocation successful, false otherwise
     */
    bool RevokeKey(const QString& keyId);

    /**
     * @brief Get key information
     * @param keyId Key ID
     * @return JSON string with key information
     */
    QString GetKeyInfo(const QString& keyId);

    /**
     * @brief Get all keys
     * @param userId Optional user ID filter
     * @return JSON string with list of keys
     */
    QString GetAllKeys(const QString& userId);

    /**
     * @brief Enroll user
     * @param username Username
     * @param email Email address
     * @param role Role (0=CommandingOfficer, 1=TechPersonnel, 2=User)
     * @param enrolledBy User ID of enrolling officer
     * @return User ID or empty string if failed
     */
    QString EnrollUser(const QString& username, const QString& email, int role, const QString& enrolledBy);

    /**
     * @brief Approve enrollment
     * @param userId User ID
     * @param approverId Approver user ID
     * @return true if approval successful, false otherwise
     */
    bool ApproveEnrollment(const QString& userId, const QString& approverId);

    /**
     * @brief Reject enrollment
     * @param userId User ID
     * @param reason Rejection reason
     * @return true if rejection successful, false otherwise
     */
    bool RejectEnrollment(const QString& userId, const QString& reason);

    /**
     * @brief Get user information
     * @param userId User ID
     * @return JSON string with user information
     */
    QString GetUserInfo(const QString& userId);

    /**
     * @brief Get all users
     * @param role Optional role filter (-1 for all)
     * @return JSON string with list of users
     */
    QString GetAllUsers(int role);

    /**
     * @brief Check permission
     * @param userId User ID
     * @param permission Permission (0=GenerateKeys, 1=DistributeKeys, 2=RevokeKeys, 3=EnrollUsers, 4=ApproveEnrollments, 5=ManageNetwork, 6=ViewLogs)
     * @return true if permission granted, false otherwise
     */
    bool CheckPermission(const QString& userId, int permission);

    /**
     * @brief Get network interfaces
     * @return JSON string with list of network interfaces
     */
    QString GetNetworkInterfaces();

    /**
     * @brief Get authorized network interfaces
     * @return JSON string with list of authorized interfaces
     */
    QString GetAuthorizedInterfaces();

    /**
     * @brief Detect network segmentation
     * @return JSON string with list of detected subnets
     */
    QString DetectSegmentation();

    /**
     * @brief Discover peers
     * @return JSON string with list of discovered peer IDs
     */
    QString DiscoverPeers();

    /**
     * @brief Get all peers
     * @return JSON string with list of peer information
     */
    QString GetAllPeers();

    /**
     * @brief Get network health metrics
     * @return JSON string with network health metrics
     */
    QString GetNetworkHealthMetrics();

    /**
     * @brief Get network topology
     * @return JSON string with network topology data
     */
    QString GetNetworkTopology();

    /**
     * @brief Connect VPN
     * @param vpnConfig VPN configuration (JSON string)
     * @return true if connection successful, false otherwise
     */
    bool ConnectVPN(const QString& vpnConfig);

    /**
     * @brief Disconnect VPN
     * @return true if disconnection successful, false otherwise
     */
    bool DisconnectVPN();

    /**
     * @brief Block unauthorized interfaces
     * @return true if blocking successful, false otherwise
     */
    bool BlockUnauthorizedInterfaces();

Q_SIGNALS:
    /**
     * @brief Emitted when USB device is detected
     */
    void usbDeviceDetected(const QString& devicePath);

    /**
     * @brief Emitted when key is generated
     */
    void keyGenerated(const QString& keyId);

    /**
     * @brief Emitted when key is revoked
     */
    void keyRevoked(const QString& keyId);

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
    bool m_initialized;
};

#endif // DBUS_INTERFACE_H

