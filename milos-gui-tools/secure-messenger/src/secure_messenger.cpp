#include "secure_messenger.h"
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
#include <QDebug>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>

SecureMessenger::SecureMessenger(QObject* parent)
    : QObject(parent)
    , m_usbAuth(new USBAuthorization(this))
    , m_keyGen(new KeyGenerator(this))
    , m_keyMgr(new KeyManager(this))
    , m_enrollment(new UserEnrollment(this))
    , m_roleMgr(new RoleManager(this))
    , m_meshNetwork(new MeshNetwork(this))
    , m_peerDiscovery(new PeerDiscovery(this))
    , m_networkManager(new NetworkManager(this))
    , m_healthMonitor(new NetworkHealthMonitor(this))
    , m_enforcement(new EthernetEnforcement(this))
    , m_messagingCore(new MessagingCore(this))
    , m_textMessaging(new TextMessaging(this))
    , m_threading(new MessageThreading(this))
    , m_conversationManager(new ConversationManager(this))
    , m_messageStorage(new MessageStorage(this))
    , m_fileSharing(new FileSharing(this))
    , m_voiceMessaging(new VoiceMessaging(this))
    , m_videoMessaging(new VideoMessaging(this))
    , m_mediaCalls(new MediaCalls(this))
    , m_groupMessaging(new GroupMessaging(this))
    , m_e2eEncryption(new E2EEncryption(this))
    , m_forwardSecrecy(new ForwardSecrecy(this))
    , m_keyExchange(new KeyExchange(this))
    , m_trafficObfuscation(new TrafficObfuscation(this))
    , m_encryptionStorage(new EncryptionStorage(this))
    , m_messageExpiration(new MessageExpiration(this))
    , m_emergencyEject(new EmergencyEject(this))
    , m_dataWipe(new DataWipe(this))
    , m_emergencyShutdown(new EmergencyShutdown(this))
    , m_adminDashboard(new AdminDashboard(this))
    , m_userManager(new UserManager(this))
    , m_systemConfig(new SystemConfig(this))
    , m_dbusInterface(new SecureMessengerDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_keyRotationTimer(new QTimer(this))
    , m_initialized(false)
    , m_running(false)
{
}

SecureMessenger::~SecureMessenger() {
    stop();
}

bool SecureMessenger::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize components
    if (!m_usbAuth->initialize()) {
        qWarning() << "Failed to initialize USB authorization";
        return false;
    }

    if (!m_keyGen->initialize()) {
        qWarning() << "Failed to initialize key generator";
        return false;
    }

    if (!m_keyMgr->initialize()) {
        qWarning() << "Failed to initialize key manager";
        return false;
    }

    if (!m_enrollment->initialize()) {
        qWarning() << "Failed to initialize user enrollment";
        return false;
    }

    if (!m_roleMgr->initialize()) {
        qWarning() << "Failed to initialize role manager";
        return false;
    }

    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }

    // Initialize network components
    if (!m_networkManager->initialize()) {
        qWarning() << "Failed to initialize network manager";
        return false;
    }

    if (!m_enforcement->initialize()) {
        qWarning() << "Failed to initialize ethernet enforcement";
        return false;
    }

    // Get local network address
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QHostAddress localAddress;
    for (const QNetworkInterface& iface : interfaces) {
        if (iface.isUp() && !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for (const QNetworkAddressEntry& entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    localAddress = entry.ip();
                    break;
                }
            }
            if (!localAddress.isNull()) {
                break;
            }
        }
    }

    if (localAddress.isNull()) {
        localAddress = QHostAddress::LocalHost;
    }

    if (!m_meshNetwork->initialize(localAddress, 8888)) {
        qWarning() << "Failed to initialize mesh network";
        return false;
    }

    if (!m_peerDiscovery->initialize("", 8889)) {
        qWarning() << "Failed to initialize peer discovery";
        return false;
    }

    if (!m_healthMonitor->initialize()) {
        qWarning() << "Failed to initialize network health monitor";
        return false;
    }

    // Initialize messaging components
    if (!m_messagingCore->initialize()) {
        qWarning() << "Failed to initialize messaging core";
        return false;
    }

    if (!m_textMessaging->initialize()) {
        qWarning() << "Failed to initialize text messaging";
        return false;
    }

    // Set messaging core for text messaging
    m_textMessaging->setMessagingCore(m_messagingCore);
    m_textMessaging->setE2EEncryption(m_e2eEncryption);
    
    // Set message storage for messaging core (automatic persistence)
    m_messagingCore->setMessageStorage(m_messageStorage);
    
    // Connect media components to messaging core and encryption
    m_fileSharing->setMessagingCore(m_messagingCore);
    m_fileSharing->setE2EEncryption(m_e2eEncryption);
    
    m_voiceMessaging->setMessagingCore(m_messagingCore);
    m_voiceMessaging->setE2EEncryption(m_e2eEncryption);
    
    m_videoMessaging->setMessagingCore(m_messagingCore);
    m_videoMessaging->setE2EEncryption(m_e2eEncryption);
    
    // Connect group messaging and media calls
    m_groupMessaging->setConversationManager(m_conversationManager);
    m_mediaCalls->setMeshNetwork(m_meshNetwork);
    
    // Connect message expiration to storage for automatic deletion
    m_messageExpiration->setMessageStorage(m_messageStorage);

    if (!m_threading->initialize()) {
        qWarning() << "Failed to initialize message threading";
        return false;
    }

    if (!m_conversationManager->initialize()) {
        qWarning() << "Failed to initialize conversation manager";
        return false;
    }

    // Initialize message storage
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/messages.db";
    
    if (!m_messageStorage->initialize(dbPath)) {
        qWarning() << "Failed to initialize message storage";
        return false;
    }

    // Initialize media messaging components
    if (!m_fileSharing->initialize()) {
        qWarning() << "Failed to initialize file sharing";
        return false;
    }

    if (!m_voiceMessaging->initialize()) {
        qWarning() << "Failed to initialize voice messaging";
        return false;
    }

    if (!m_videoMessaging->initialize()) {
        qWarning() << "Failed to initialize video messaging";
        return false;
    }

    if (!m_mediaCalls->initialize()) {
        qWarning() << "Failed to initialize media calls";
        return false;
    }

    if (!m_groupMessaging->initialize()) {
        qWarning() << "Failed to initialize group messaging";
        return false;
    }

    // Initialize security components
    if (!m_e2eEncryption->initialize()) {
        qWarning() << "Failed to initialize E2E encryption";
        return false;
    }

    if (!m_forwardSecrecy->initialize()) {
        qWarning() << "Failed to initialize forward secrecy";
        return false;
    }

    if (!m_keyExchange->initialize()) {
        qWarning() << "Failed to initialize key exchange";
        return false;
    }

    if (!m_trafficObfuscation->initialize()) {
        qWarning() << "Failed to initialize traffic obfuscation";
        return false;
    }

    // Initialize encryption storage
    QString encryptedDbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(encryptedDbPath);
    encryptedDbPath += "/encrypted_messages.db";
    QByteArray encryptionKey = QCryptographicHash::hash("default_key", QCryptographicHash::Sha256);
    if (!m_encryptionStorage->initialize(encryptedDbPath, encryptionKey)) {
        qWarning() << "Failed to initialize encryption storage";
        return false;
    }

    if (!m_messageExpiration->initialize()) {
        qWarning() << "Failed to initialize message expiration";
        return false;
    }

    // Initialize emergency and admin components
    if (!m_emergencyEject->initialize()) {
        qWarning() << "Failed to initialize emergency eject";
        return false;
    }

    if (!m_dataWipe->initialize()) {
        qWarning() << "Failed to initialize data wipe";
        return false;
    }

    if (!m_emergencyShutdown->initialize()) {
        qWarning() << "Failed to initialize emergency shutdown";
        return false;
    }

    if (!m_adminDashboard->initialize()) {
        qWarning() << "Failed to initialize admin dashboard";
        return false;
    }

    if (!m_userManager->initialize()) {
        qWarning() << "Failed to initialize user manager";
        return false;
    }

    if (!m_systemConfig->initialize()) {
        qWarning() << "Failed to initialize system config";
        return false;
    }

    // Connect signals for audit logging
    connect(m_keyGen, &KeyGenerator::keyGenerated, this, [this](const QString& keyId) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        m_auditLogger->logKeyOperation("key_generated", QString(), eventData);
    });

    connect(m_keyGen, &KeyGenerator::keyWrittenToUSB, this, [this](const QString& keyId, const QString& devicePath) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        eventData["device_path"] = devicePath;
        m_auditLogger->logKeyOperation("key_written_to_usb", QString(), eventData);
    });

    connect(m_keyMgr, &KeyManager::keyDistributed, this, [this](const QString& keyId, const QString& devicePath) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        eventData["device_path"] = devicePath;
        m_auditLogger->logKeyOperation("key_distributed", QString(), eventData);
    });

    connect(m_keyMgr, &KeyManager::keyRevoked, this, [this](const QString& keyId) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        m_auditLogger->logKeyOperation("key_revoked", QString(), eventData);
    });

    connect(m_enrollment, &UserEnrollment::userEnrolled, this, [this](const QString& userId) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        m_auditLogger->logKeyOperation("user_enrolled", userId, eventData);
    });

    connect(m_enrollment, &UserEnrollment::enrollmentApproved, this, [this](const QString& userId) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        m_auditLogger->logKeyOperation("enrollment_approved", userId, eventData);
    });

    connect(m_enrollment, &UserEnrollment::enrollmentRejected, this, [this](const QString& userId, const QString& reason) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        eventData["reason"] = reason;
        m_auditLogger->logKeyOperation("enrollment_rejected", userId, eventData);
    });

    connect(m_meshNetwork, &MeshNetwork::peerAdded, this, [this](const QString& peerId) {
        QVariantMap eventData;
        eventData["peer_id"] = peerId;
        m_auditLogger->logKeyOperation("peer_added", QString(), eventData);
    });

    connect(m_meshNetwork, &MeshNetwork::peerRemoved, this, [this](const QString& peerId) {
        QVariantMap eventData;
        eventData["peer_id"] = peerId;
        m_auditLogger->logKeyOperation("peer_removed", QString(), eventData);
    });

    connect(m_peerDiscovery, &PeerDiscovery::peerDiscovered, this, [this](const QString& peerId, const QHostAddress& address) {
        QVariantMap eventData;
        eventData["peer_id"] = peerId;
        eventData["address"] = address.toString();
        m_auditLogger->logKeyOperation("peer_discovered", QString(), eventData);
    });

    connect(m_enforcement, &EthernetEnforcement::unauthorizedInterfaceDetected, this, [this](const QString& interfaceName) {
        QVariantMap eventData;
        eventData["interface_name"] = interfaceName;
        m_auditLogger->logKeyOperation("unauthorized_interface_detected", QString(), eventData);
    });

    connect(m_messagingCore, &MessagingCore::messageSent, this, [this](const QString& messageId) {
        QVariantMap eventData;
        eventData["message_id"] = messageId;
        m_auditLogger->logKeyOperation("message_sent", QString(), eventData);
    });

    connect(m_messagingCore, &MessagingCore::messageReceived, this, [this](const QString& messageId) {
        QVariantMap eventData;
        eventData["message_id"] = messageId;
        m_auditLogger->logKeyOperation("message_received", QString(), eventData);
    });

    connect(m_messagingCore, &MessagingCore::messageStatusUpdated, this, [this](const QString& messageId, MessageStatus status) {
        QVariantMap eventData;
        eventData["message_id"] = messageId;
        eventData["status"] = static_cast<int>(status);
        m_auditLogger->logKeyOperation("message_status_updated", QString(), eventData);
    });

    // Connect encryption operation signals for audit logging
    connect(m_e2eEncryption, &E2EEncryption::encryptionFailed, this, [this](const QString& error) {
        QVariantMap eventData;
        eventData["error"] = error;
        m_auditLogger->logKeyOperation("encryption_failed", QString(), eventData);
    });

    connect(m_e2eEncryption, &E2EEncryption::decryptionFailed, this, [this](const QString& error) {
        QVariantMap eventData;
        eventData["error"] = error;
        m_auditLogger->logKeyOperation("decryption_failed", QString(), eventData);
    });

    connect(m_forwardSecrecy, &ForwardSecrecy::sessionKeyGenerated, this, [this](const QString& sessionId) {
        QVariantMap eventData;
        eventData["session_id"] = sessionId;
        m_auditLogger->logKeyOperation("session_key_generated", QString(), eventData);
    });

    connect(m_forwardSecrecy, &ForwardSecrecy::sessionKeyRotated, this, [this](const QString& oldSessionId, const QString& newSessionId) {
        QVariantMap eventData;
        eventData["old_session_id"] = oldSessionId;
        eventData["new_session_id"] = newSessionId;
        m_auditLogger->logKeyOperation("session_key_rotated", QString(), eventData);
    });

    connect(m_keyExchange, &KeyExchange::keyExchangeInitiated, this, [this](const QString& exchangeId) {
        QVariantMap eventData;
        eventData["exchange_id"] = exchangeId;
        m_auditLogger->logKeyOperation("key_exchange_initiated", QString(), eventData);
    });

    connect(m_keyExchange, &KeyExchange::keyExchangeCompleted, this, [this](const QString& exchangeId) {
        QVariantMap eventData;
        eventData["exchange_id"] = exchangeId;
        m_auditLogger->logKeyOperation("key_exchange_completed", QString(), eventData);
    });

    connect(m_keyExchange, &KeyExchange::keyExchangeFailed, this, [this](const QString& exchangeId, const QString& error) {
        QVariantMap eventData;
        eventData["exchange_id"] = exchangeId;
        eventData["error"] = error;
        m_auditLogger->logKeyOperation("key_exchange_failed", QString(), eventData);
    });

    connect(m_messageExpiration, &MessageExpiration::messageExpired, this, [this](const QString& messageId) {
        QVariantMap eventData;
        eventData["message_id"] = messageId;
        m_auditLogger->logKeyOperation("message_expired", QString(), eventData);
    });

    // Connect message status updates to expiration policies
    connect(m_messagingCore, &MessagingCore::messageStatusUpdated, this, [this](const QString& messageId, MessageStatus status) {
        if (status == MessageStatus::Read) {
            m_messageExpiration->markMessageAsRead(messageId);
        } else if (status == MessageStatus::Delivered) {
            m_messageExpiration->markMessageAsDelivered(messageId);
        }
    });

    // Connect emergency operation signals for audit logging
    connect(m_emergencyEject, &EmergencyEject::ejectInitiated, this, [this]() {
        QVariantMap eventData;
        m_auditLogger->logKeyOperation("emergency_eject_initiated", QString(), eventData);
    });

    connect(m_emergencyEject, &EmergencyEject::ejectCompleted, this, [this]() {
        QVariantMap eventData;
        m_auditLogger->logKeyOperation("emergency_eject_completed", QString(), eventData);
    });

    connect(m_emergencyEject, &EmergencyEject::ejectFailed, this, [this](const QString& error) {
        QVariantMap eventData;
        eventData["error"] = error;
        m_auditLogger->logKeyOperation("emergency_eject_failed", QString(), eventData);
    });

    connect(m_dataWipe, &DataWipe::wipeCompleted, this, [this]() {
        QVariantMap eventData;
        m_auditLogger->logKeyOperation("data_wipe_completed", QString(), eventData);
    });

    connect(m_emergencyShutdown, &EmergencyShutdown::shutdownInitiated, this, [this]() {
        QVariantMap eventData;
        m_auditLogger->logKeyOperation("emergency_shutdown_initiated", QString(), eventData);
    });

    // Set up automatic key rotation timer (every hour)
    m_keyRotationTimer->setInterval(3600000); // 1 hour in milliseconds
    connect(m_keyRotationTimer, &QTimer::timeout, this, &SecureMessenger::onKeyRotationTimer);

    // Initialize D-Bus interface
    m_dbusInterface->setUSBAuthorization(m_usbAuth);
    m_dbusInterface->setKeyGenerator(m_keyGen);
    m_dbusInterface->setKeyManager(m_keyMgr);
    m_dbusInterface->setUserEnrollment(m_enrollment);
    m_dbusInterface->setRoleManager(m_roleMgr);
    m_dbusInterface->setMeshNetwork(m_meshNetwork);
    m_dbusInterface->setPeerDiscovery(m_peerDiscovery);
    m_dbusInterface->setNetworkManager(m_networkManager);
    m_dbusInterface->setNetworkHealthMonitor(m_healthMonitor);
    m_dbusInterface->setEthernetEnforcement(m_enforcement);
    m_dbusInterface->setMessagingCore(m_messagingCore);
    m_dbusInterface->setTextMessaging(m_textMessaging);
    m_dbusInterface->setMessageThreading(m_threading);
    m_dbusInterface->setConversationManager(m_conversationManager);
    m_dbusInterface->setMessageStorage(m_messageStorage);
    m_dbusInterface->setFileSharing(m_fileSharing);
    m_dbusInterface->setVoiceMessaging(m_voiceMessaging);
    m_dbusInterface->setVideoMessaging(m_videoMessaging);
    m_dbusInterface->setMediaCalls(m_mediaCalls);
    m_dbusInterface->setGroupMessaging(m_groupMessaging);
    m_dbusInterface->setE2EEncryption(m_e2eEncryption);
    m_dbusInterface->setForwardSecrecy(m_forwardSecrecy);
    m_dbusInterface->setKeyExchange(m_keyExchange);
    m_dbusInterface->setTrafficObfuscation(m_trafficObfuscation);
    m_dbusInterface->setEncryptionStorage(m_encryptionStorage);
    m_dbusInterface->setMessageExpiration(m_messageExpiration);
    m_dbusInterface->setEmergencyEject(m_emergencyEject);
    m_dbusInterface->setDataWipe(m_dataWipe);
    m_dbusInterface->setEmergencyShutdown(m_emergencyShutdown);
    
    // Connect AdminDashboard to actual components for real data
    m_adminDashboard->setUserEnrollment(m_enrollment);
    m_adminDashboard->setConversationManager(m_conversationManager);
    m_adminDashboard->setMessagingCore(m_messagingCore);
    m_adminDashboard->setMessageStorage(m_messageStorage);
    m_adminDashboard->setMeshNetwork(m_meshNetwork);
    m_adminDashboard->setNetworkHealthMonitor(m_healthMonitor);
    m_adminDashboard->setE2EEncryption(m_e2eEncryption);
    m_adminDashboard->setForwardSecrecy(m_forwardSecrecy);
    
    m_dbusInterface->setAdminDashboard(m_adminDashboard);
    m_dbusInterface->setUserManager(m_userManager);
    m_dbusInterface->setSystemConfig(m_systemConfig);

    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }

    m_initialized = true;
    return true;
}

bool SecureMessenger::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start network components
    if (!m_peerDiscovery->start()) {
        qWarning() << "Failed to start peer discovery";
        return false;
    }

    if (!m_healthMonitor->start()) {
        qWarning() << "Failed to start network health monitor";
        return false;
    }

    // Enforce ethernet-only operation
    m_enforcement->blockUnauthorizedInterfaces();
    m_enforcement->blockInternetConnectivity();

    // Start automatic key rotation timer
    m_keyRotationTimer->start();

    m_running = true;
    return true;
}

void SecureMessenger::stop() {
    if (!m_running) {
        return;
    }

    // Stop key rotation timer
    if (m_keyRotationTimer) {
        m_keyRotationTimer->stop();
    }

    m_running = false;
}

void SecureMessenger::onKeyRotationTimer() {
    // Expire old session keys
    if (m_forwardSecrecy) {
        m_forwardSecrecy->expireOldSessionKeys();
    }

    // Rotate keys for all active participants from conversations
    QSet<QString> activeParticipants;
    
    if (m_conversationManager) {
        // Get all conversations and collect unique participants
        QList<Conversation> allConversations = m_conversationManager->getAllConversations();
        
        for (const Conversation& conv : allConversations) {
            // Add all participants from this conversation
            for (const QString& participantId : conv.participants) {
                if (!participantId.isEmpty()) {
                    activeParticipants.insert(participantId);
                }
            }
        }
    }
    
    // Rotate session keys for each active participant
    int rotatedCount = 0;
    for (const QString& participantId : activeParticipants) {
        if (m_forwardSecrecy && !participantId.isEmpty()) {
            m_forwardSecrecy->rotateSessionKey(participantId);
            rotatedCount++;
        }
    }
    
    // Log the rotation event
    QVariantMap eventData;
    eventData["rotation_type"] = "automatic";
    eventData["participants_count"] = activeParticipants.size();
    eventData["keys_rotated"] = rotatedCount;
    m_auditLogger->logKeyOperation("automatic_key_rotation", QString(), eventData);
}

