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
class MessagingCore;
class TextMessaging;
class MessageThreading;
class ConversationManager;
class MessageStorage;
class FileSharing;
class VoiceMessaging;
class VideoMessaging;
class MediaCalls;
class GroupMessaging;
class E2EEncryption;
class ForwardSecrecy;
class KeyExchange;
class TrafficObfuscation;
class EncryptionStorage;
class MessageExpiration;
class EmergencyEject;
class DataWipe;
class EmergencyShutdown;
class AdminDashboard;
class UserManager;
class SystemConfig;

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
     * @brief Set messaging core
     */
    void setMessagingCore(MessagingCore* messagingCore);

    /**
     * @brief Set text messaging
     */
    void setTextMessaging(TextMessaging* textMessaging);

    /**
     * @brief Set message threading
     */
    void setMessageThreading(MessageThreading* threading);

    /**
     * @brief Set conversation manager
     */
    void setConversationManager(ConversationManager* conversationManager);

    /**
     * @brief Set message storage
     */
    void setMessageStorage(MessageStorage* messageStorage);

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

    /**
     * @brief Send text message
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param text Text content
     * @param formatType Format type (0=Plain, 1=Markdown, 2=HTML)
     * @return Message ID or empty string if failed
     */
    QString SendTextMessage(const QString& conversationId, const QString& recipientId, const QString& text, int formatType);

    /**
     * @brief Get message
     * @param messageId Message ID
     * @return JSON string with message information
     */
    QString GetMessage(const QString& messageId);

    /**
     * @brief Get messages for conversation
     * @param conversationId Conversation ID
     * @param limit Maximum number of messages
     * @param offset Offset for pagination
     * @return JSON string with list of messages
     */
    QString GetMessagesForConversation(const QString& conversationId, int limit, int offset);

    /**
     * @brief Mark message as read
     * @param messageId Message ID
     * @return true if mark successful, false otherwise
     */
    bool MarkMessageAsRead(const QString& messageId);

    /**
     * @brief Create conversation
     * @param type Conversation type (0=Direct, 1=Group)
     * @param participants List of participant IDs (JSON array string)
     * @param title Conversation title
     * @return Conversation ID or empty string if failed
     */
    QString CreateConversation(int type, const QString& participants, const QString& title);

    /**
     * @brief Get conversation
     * @param conversationId Conversation ID
     * @return JSON string with conversation information
     */
    QString GetConversation(const QString& conversationId);

    /**
     * @brief Get conversations for user
     * @param userId User ID
     * @return JSON string with list of conversations
     */
    QString GetConversationsForUser(const QString& userId);

    /**
     * @brief Search conversations
     * @param query Search query
     * @param userId Optional user ID filter
     * @return JSON string with list of matching conversations
     */
    QString SearchConversations(const QString& query, const QString& userId);

    /**
     * @brief Create thread
     * @param conversationId Conversation ID
     * @param title Thread title
     * @return Thread ID or empty string if failed
     */
    QString CreateThread(const QString& conversationId, const QString& title);

    /**
     * @brief Get threads for conversation
     * @param conversationId Conversation ID
     * @return JSON string with list of threads
     */
    QString GetThreadsForConversation(const QString& conversationId);

    /**
     * @brief Set file sharing
     */
    void setFileSharing(FileSharing* fileSharing);

    /**
     * @brief Set voice messaging
     */
    void setVoiceMessaging(VoiceMessaging* voiceMessaging);

    /**
     * @brief Set video messaging
     */
    void setVideoMessaging(VideoMessaging* videoMessaging);

    /**
     * @brief Set media calls
     */
    void setMediaCalls(MediaCalls* mediaCalls);

    /**
     * @brief Set group messaging
     */
    void setGroupMessaging(GroupMessaging* groupMessaging);

    /**
     * @brief Send file
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param filePath File path
     * @return Transfer ID or empty string if failed
     */
    QString SendFile(const QString& conversationId, const QString& recipientId, const QString& filePath);

    /**
     * @brief Get file transfer info
     * @param transferId Transfer ID
     * @return JSON string with transfer information
     */
    QString GetFileTransferInfo(const QString& transferId);

    /**
     * @brief Send voice message
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param audioFilePath Audio file path
     * @return Message ID or empty string if failed
     */
    QString SendVoiceMessage(const QString& conversationId, const QString& recipientId, const QString& audioFilePath);

    /**
     * @brief Send video message
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param videoFilePath Video file path
     * @return Message ID or empty string if failed
     */
    QString SendVideoMessage(const QString& conversationId, const QString& recipientId, const QString& videoFilePath);

    /**
     * @brief Start call
     * @param type Call type (0=Voice, 1=Video)
     * @param recipientId Recipient ID
     * @return Call ID or empty string if failed
     */
    QString StartCall(int type, const QString& recipientId);

    /**
     * @brief Accept call
     * @param callId Call ID
     * @return true if accept successful, false otherwise
     */
    bool AcceptCall(const QString& callId);

    /**
     * @brief Reject call
     * @param callId Call ID
     * @return true if reject successful, false otherwise
     */
    bool RejectCall(const QString& callId);

    /**
     * @brief End call
     * @param callId Call ID
     * @return true if end successful, false otherwise
     */
    bool EndCall(const QString& callId);

    /**
     * @brief Create group
     * @param name Group name
     * @param creatorId Creator user ID
     * @param participants List of participant IDs (JSON array string)
     * @return Conversation ID or empty string if failed
     */
    QString CreateGroup(const QString& name, const QString& creatorId, const QString& participants);

    /**
     * @brief Add participant to group
     * @param conversationId Conversation ID
     * @param participantId Participant ID
     * @return true if add successful, false otherwise
     */
    bool AddGroupParticipant(const QString& conversationId, const QString& participantId);

    /**
     * @brief Remove participant from group
     * @param conversationId Conversation ID
     * @param participantId Participant ID
     * @return true if remove successful, false otherwise
     */
    bool RemoveGroupParticipant(const QString& conversationId, const QString& participantId);

    /**
     * @brief Get group participants
     * @param conversationId Conversation ID
     * @return JSON string with list of participant IDs
     */
    QString GetGroupParticipants(const QString& conversationId);

    /**
     * @brief Set E2E encryption
     */
    void setE2EEncryption(E2EEncryption* encryption);

    /**
     * @brief Set forward secrecy
     */
    void setForwardSecrecy(ForwardSecrecy* forwardSecrecy);

    /**
     * @brief Set key exchange
     */
    void setKeyExchange(KeyExchange* keyExchange);

    /**
     * @brief Set traffic obfuscation
     */
    void setTrafficObfuscation(TrafficObfuscation* obfuscation);

    /**
     * @brief Set encryption storage
     */
    void setEncryptionStorage(EncryptionStorage* storage);

    /**
     * @brief Set message expiration
     */
    void setMessageExpiration(MessageExpiration* expiration);

    /**
     * @brief Initiate key exchange
     * @param participantId Participant ID
     * @return Exchange ID or empty string if failed
     */
    QString InitiateKeyExchange(const QString& participantId);

    /**
     * @brief Complete key exchange
     * @param exchangeId Exchange ID
     * @param publicKey Public key (base64 encoded)
     * @return true if completion successful, false otherwise
     */
    bool CompleteKeyExchange(const QString& exchangeId, const QString& publicKey);

    /**
     * @brief Set message expiration policy
     * @param messageId Message ID
     * @param policy Expiration policy (0=Never, 1=AfterRead, 2=AfterTime, 3=AfterDelivery)
     * @param expirationSeconds Expiration time in seconds (for AfterTime policy)
     * @return true if set successful, false otherwise
     */
    bool SetMessageExpirationPolicy(const QString& messageId, int policy, int expirationSeconds);

    /**
     * @brief Check if message is expired
     * @param messageId Message ID
     * @return true if expired, false otherwise
     */
    bool IsMessageExpired(const QString& messageId);

    /**
     * @brief Delete expired messages
     * @return Number of messages deleted
     */
    int DeleteExpiredMessages();

    /**
     * @brief Enable/disable encrypted storage
     * @param enabled Enabled state
     * @return true if set successful, false otherwise
     */
    bool SetEncryptionStorageEnabled(bool enabled);

    /**
     * @brief Set emergency eject
     */
    void setEmergencyEject(EmergencyEject* emergencyEject);

    /**
     * @brief Set data wipe
     */
    void setDataWipe(DataWipe* dataWipe);

    /**
     * @brief Set emergency shutdown
     */
    void setEmergencyShutdown(EmergencyShutdown* emergencyShutdown);

    /**
     * @brief Set admin dashboard
     */
    void setAdminDashboard(AdminDashboard* adminDashboard);

    /**
     * @brief Set user manager
     */
    void setUserManager(UserManager* userManager);

    /**
     * @brief Set system config
     */
    void setSystemConfig(SystemConfig* systemConfig);

    /**
     * @brief Execute emergency eject
     * @param confirmationCode Confirmation code
     * @return true if eject successful, false otherwise
     */
    bool ExecuteEmergencyEject(const QString& confirmationCode);

    /**
     * @brief Execute secure data wipe
     * @param wipeType Wipe type (0=Quick, 1=Secure, 2=Cryptographic)
     * @return true if wipe successful, false otherwise
     */
    bool ExecuteDataWipe(int wipeType);

    /**
     * @brief Execute emergency shutdown
     * @param reason Shutdown reason
     * @return true if shutdown successful, false otherwise
     */
    bool ExecuteEmergencyShutdown(const QString& reason);

    /**
     * @brief Get admin dashboard data
     * @return JSON string with dashboard data
     */
    QString GetAdminDashboardData();

    /**
     * @brief Get system statistics
     * @return JSON string with system statistics
     */
    QString GetSystemStatistics();

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
    bool m_initialized;
};

#endif // DBUS_INTERFACE_H

