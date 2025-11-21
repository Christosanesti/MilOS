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
#include "messaging_core.h"
#include "text_messaging.h"
#include "message_threading.h"
#include "conversation_manager.h"
#include "message_storage.h"
#include "file_sharing.h"
#include "voice_messaging.h"
#include "video_messaging.h"
#include "media_calls.h"
#include "group_messaging.h"
#include "e2e_encryption.h"
#include "forward_secrecy.h"
#include "key_exchange.h"
#include "traffic_obfuscation.h"
#include "encryption_storage.h"
#include "message_expiration.h"
#include "emergency_eject.h"
#include "data_wipe.h"
#include "emergency_shutdown.h"
#include "admin_dashboard.h"
#include "user_manager.h"
#include "system_config.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>
#include <QString>
#include <QTimer>

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

    /**
     * @brief Get messaging core
     */
    MessagingCore* getMessagingCore() const { return m_messagingCore; }

    /**
     * @brief Get text messaging
     */
    TextMessaging* getTextMessaging() const { return m_textMessaging; }

    /**
     * @brief Get conversation manager
     */
    ConversationManager* getConversationManager() const { return m_conversationManager; }

    /**
     * @brief Get E2E encryption
     */
    E2EEncryption* getE2EEncryption() const { return m_e2eEncryption; }

    /**
     * @brief Get forward secrecy
     */
    ForwardSecrecy* getForwardSecrecy() const { return m_forwardSecrecy; }

    /**
     * @brief Get emergency eject
     */
    EmergencyEject* getEmergencyEject() const { return m_emergencyEject; }

    /**
     * @brief Get admin dashboard
     */
    AdminDashboard* getAdminDashboard() const { return m_adminDashboard; }

private slots:
    /**
     * @brief Handle automatic key rotation
     */
    void onKeyRotationTimer();

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
    MessagingCore* m_messagingCore;
    TextMessaging* m_textMessaging;
    MessageThreading* m_threading;
    ConversationManager* m_conversationManager;
    MessageStorage* m_messageStorage;
    FileSharing* m_fileSharing;
    VoiceMessaging* m_voiceMessaging;
    VideoMessaging* m_videoMessaging;
    MediaCalls* m_mediaCalls;
    GroupMessaging* m_groupMessaging;
    E2EEncryption* m_e2eEncryption;
    ForwardSecrecy* m_forwardSecrecy;
    KeyExchange* m_keyExchange;
    TrafficObfuscation* m_trafficObfuscation;
    EncryptionStorage* m_encryptionStorage;
    MessageExpiration* m_messageExpiration;
    EmergencyEject* m_emergencyEject;
    DataWipe* m_dataWipe;
    EmergencyShutdown* m_emergencyShutdown;
    AdminDashboard* m_adminDashboard;
    UserManager* m_userManager;
    SystemConfig* m_systemConfig;
    SecureMessengerDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    QTimer* m_keyRotationTimer;
    bool m_initialized;
    bool m_running;
};

#endif // SECURE_MESSENGER_H

