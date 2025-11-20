#include "dbus_interface.h"
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
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SecureMessengerDBusInterface::SecureMessengerDBusInterface(QObject* parent)
    : QObject(parent)
    , m_usbAuth(nullptr)
    , m_keyGen(nullptr)
    , m_keyMgr(nullptr)
    , m_enrollment(nullptr)
    , m_roleMgr(nullptr)
    , m_meshNetwork(nullptr)
    , m_peerDiscovery(nullptr)
    , m_networkManager(nullptr)
    , m_healthMonitor(nullptr)
    , m_enforcement(nullptr)
    , m_messagingCore(nullptr)
    , m_textMessaging(nullptr)
    , m_threading(nullptr)
    , m_conversationManager(nullptr)
    , m_messageStorage(nullptr)
    , m_initialized(false)
{
}

SecureMessengerDBusInterface::~SecureMessengerDBusInterface() {
}

void SecureMessengerDBusInterface::setUSBAuthorization(USBAuthorization* usbAuth) {
    m_usbAuth = usbAuth;
}

void SecureMessengerDBusInterface::setKeyGenerator(KeyGenerator* keyGen) {
    m_keyGen = keyGen;
}

void SecureMessengerDBusInterface::setKeyManager(KeyManager* keyMgr) {
    m_keyMgr = keyMgr;
}

void SecureMessengerDBusInterface::setUserEnrollment(UserEnrollment* enrollment) {
    m_enrollment = enrollment;
}

void SecureMessengerDBusInterface::setRoleManager(RoleManager* roleMgr) {
    m_roleMgr = roleMgr;
}

void SecureMessengerDBusInterface::setMeshNetwork(MeshNetwork* meshNetwork) {
    m_meshNetwork = meshNetwork;
}

void SecureMessengerDBusInterface::setPeerDiscovery(PeerDiscovery* peerDiscovery) {
    m_peerDiscovery = peerDiscovery;
}

void SecureMessengerDBusInterface::setNetworkManager(NetworkManager* networkManager) {
    m_networkManager = networkManager;
}

void SecureMessengerDBusInterface::setNetworkHealthMonitor(NetworkHealthMonitor* healthMonitor) {
    m_healthMonitor = healthMonitor;
}

void SecureMessengerDBusInterface::setEthernetEnforcement(EthernetEnforcement* enforcement) {
    m_enforcement = enforcement;
}

void SecureMessengerDBusInterface::setMessagingCore(MessagingCore* messagingCore) {
    m_messagingCore = messagingCore;
}

void SecureMessengerDBusInterface::setTextMessaging(TextMessaging* textMessaging) {
    m_textMessaging = textMessaging;
}

void SecureMessengerDBusInterface::setMessageThreading(MessageThreading* threading) {
    m_threading = threading;
}

void SecureMessengerDBusInterface::setConversationManager(ConversationManager* conversationManager) {
    m_conversationManager = conversationManager;
}

void SecureMessengerDBusInterface::setMessageStorage(MessageStorage* messageStorage) {
    m_messageStorage = messageStorage;
}

bool SecureMessengerDBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }

    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SecureMessenger", this,
                                                       QDBusConnection::ExportAllSlots |
                                                       QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }

    if (!QDBusConnection::sessionBus().registerService("org.milos.SecureMessenger")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }

    m_initialized = true;
    return true;
}

QString SecureMessengerDBusInterface::DetectUSBDevices() {
    if (!m_usbAuth) {
        return QString();
    }

    QList<QString> devices = m_usbAuth->detectUSBDevices();
    
    QJsonArray jsonArray;
    for (const QString& device : devices) {
        jsonArray.append(device);
    }
    
    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::ValidateUSBKey(const QString& devicePath) {
    if (!m_usbAuth) {
        return QString();
    }

    USBKeyInfo keyInfo = m_usbAuth->validateUSBKey(devicePath);
    if (keyInfo.keyId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["key_id"] = keyInfo.keyId;
    obj["user_id"] = keyInfo.userId;
    obj["status"] = static_cast<int>(keyInfo.status);
    obj["created_at"] = keyInfo.createdAt.toString(Qt::ISODate);
    if (keyInfo.expiresAt.isValid()) {
        obj["expires_at"] = keyInfo.expiresAt.toString(Qt::ISODate);
    }
    obj["role"] = static_cast<int>(keyInfo.role);

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GenerateKey(const QString& userId, int role, int expirationDays) {
    if (!m_keyGen) {
        return QString();
    }

    USBKeyInfo keyInfo = m_keyGen->generateKey(userId, static_cast<UserRole>(role), expirationDays);
    return keyInfo.keyId;
}

bool SecureMessengerDBusInterface::WriteKeyToUSB(const QString& keyId, const QString& devicePath) {
    if (!m_keyGen || !m_keyMgr) {
        return false;
    }

    USBKeyInfo keyInfo = m_keyMgr->getKeyInfo(keyId);
    if (keyInfo.keyId.isEmpty()) {
        return false;
    }

    return m_keyGen->writeKeyToUSB(keyInfo, devicePath);
}

bool SecureMessengerDBusInterface::DistributeKey(const QString& keyId, const QString& devicePath) {
    if (!m_keyMgr) {
        return false;
    }

    USBKeyInfo keyInfo = m_keyMgr->getKeyInfo(keyId);
    if (keyInfo.keyId.isEmpty()) {
        return false;
    }

    return m_keyMgr->distributeKey(keyInfo, devicePath);
}

bool SecureMessengerDBusInterface::RevokeKey(const QString& keyId) {
    if (!m_keyMgr) {
        return false;
    }

    return m_keyMgr->revokeKey(keyId);
}

QString SecureMessengerDBusInterface::GetKeyInfo(const QString& keyId) {
    if (!m_keyMgr) {
        return QString();
    }

    USBKeyInfo keyInfo = m_keyMgr->getKeyInfo(keyId);
    if (keyInfo.keyId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["key_id"] = keyInfo.keyId;
    obj["user_id"] = keyInfo.userId;
    obj["status"] = static_cast<int>(keyInfo.status);
    obj["created_at"] = keyInfo.createdAt.toString(Qt::ISODate);
    if (keyInfo.expiresAt.isValid()) {
        obj["expires_at"] = keyInfo.expiresAt.toString(Qt::ISODate);
    }
    obj["role"] = static_cast<int>(keyInfo.role);

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetAllKeys(const QString& userId) {
    if (!m_keyMgr) {
        return QString();
    }

    QList<USBKeyInfo> keys = m_keyMgr->getAllKeys(userId.isEmpty() ? QString() : userId);
    
    QJsonArray jsonArray;
    for (const USBKeyInfo& key : keys) {
        QJsonObject obj;
        obj["key_id"] = key.keyId;
        obj["user_id"] = key.userId;
        obj["status"] = static_cast<int>(key.status);
        obj["created_at"] = key.createdAt.toString(Qt::ISODate);
        if (key.expiresAt.isValid()) {
            obj["expires_at"] = key.expiresAt.toString(Qt::ISODate);
        }
        obj["role"] = static_cast<int>(key.role);
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::EnrollUser(const QString& username, const QString& email, int role, const QString& enrolledBy) {
    if (!m_enrollment) {
        return QString();
    }

    UserInfo user = m_enrollment->enrollUser(username, email, static_cast<UserRole>(role), enrolledBy);
    return user.userId;
}

bool SecureMessengerDBusInterface::ApproveEnrollment(const QString& userId, const QString& approverId) {
    if (!m_enrollment) {
        return false;
    }

    return m_enrollment->approveEnrollment(userId, approverId);
}

bool SecureMessengerDBusInterface::RejectEnrollment(const QString& userId, const QString& reason) {
    if (!m_enrollment) {
        return false;
    }

    return m_enrollment->rejectEnrollment(userId, reason);
}

QString SecureMessengerDBusInterface::GetUserInfo(const QString& userId) {
    if (!m_enrollment) {
        return QString();
    }

    UserInfo user = m_enrollment->getUserInfo(userId);
    if (user.userId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["user_id"] = user.userId;
    obj["username"] = user.username;
    obj["email"] = user.email;
    obj["role"] = static_cast<int>(user.role);
    obj["status"] = static_cast<int>(user.status);
    obj["enrolled_at"] = user.enrolledAt.toString(Qt::ISODate);
    obj["enrolled_by"] = user.enrolledBy;

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetAllUsers(int role) {
    if (!m_enrollment) {
        return QString();
    }

    QList<UserInfo> users = m_enrollment->getAllUsers(role < 0 ? UserRole::User : static_cast<UserRole>(role));
    
    QJsonArray jsonArray;
    for (const UserInfo& user : users) {
        QJsonObject obj;
        obj["user_id"] = user.userId;
        obj["username"] = user.username;
        obj["email"] = user.email;
        obj["role"] = static_cast<int>(user.role);
        obj["status"] = static_cast<int>(user.status);
        obj["enrolled_at"] = user.enrolledAt.toString(Qt::ISODate);
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

bool SecureMessengerDBusInterface::CheckPermission(const QString& userId, int permission) {
    if (!m_roleMgr) {
        return false;
    }

    return m_roleMgr->checkPermission(userId, static_cast<Permission>(permission));
}

QString SecureMessengerDBusInterface::GetNetworkInterfaces() {
    if (!m_networkManager) {
        return QString();
    }

    QList<NetworkInterfaceInfo> interfaces = m_networkManager->getNetworkInterfaces();
    
    QJsonArray jsonArray;
    for (const NetworkInterfaceInfo& iface : interfaces) {
        QJsonObject obj;
        obj["name"] = iface.name;
        obj["type"] = static_cast<int>(iface.type);
        obj["address"] = iface.address.toString();
        obj["subnet"] = iface.subnet;
        obj["is_active"] = iface.isActive;
        obj["is_authorized"] = iface.isAuthorized;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetAuthorizedInterfaces() {
    if (!m_networkManager) {
        return QString();
    }

    QList<NetworkInterfaceInfo> interfaces = m_networkManager->getAuthorizedInterfaces();
    
    QJsonArray jsonArray;
    for (const NetworkInterfaceInfo& iface : interfaces) {
        QJsonObject obj;
        obj["name"] = iface.name;
        obj["type"] = static_cast<int>(iface.type);
        obj["address"] = iface.address.toString();
        obj["subnet"] = iface.subnet;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::DetectSegmentation() {
    if (!m_networkManager) {
        return QString();
    }

    QList<QString> subnets = m_networkManager->detectSegmentation();
    
    QJsonArray jsonArray;
    for (const QString& subnet : subnets) {
        jsonArray.append(subnet);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::DiscoverPeers() {
    if (!m_peerDiscovery) {
        return QString();
    }

    QList<QString> peers = m_peerDiscovery->discoverPeers();
    
    QJsonArray jsonArray;
    for (const QString& peerId : peers) {
        jsonArray.append(peerId);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetAllPeers() {
    if (!m_meshNetwork) {
        return QString();
    }

    QList<PeerInfo> peers = m_meshNetwork->getAllPeers();
    
    QJsonArray jsonArray;
    for (const PeerInfo& peer : peers) {
        QJsonObject obj;
        obj["peer_id"] = peer.peerId;
        obj["address"] = peer.address.toString();
        obj["port"] = peer.port;
        obj["status"] = static_cast<int>(peer.status);
        obj["user_id"] = peer.userId;
        obj["last_seen"] = peer.lastSeen.toString(Qt::ISODate);
        obj["hop_count"] = peer.hopCount;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetNetworkHealthMetrics() {
    if (!m_healthMonitor) {
        return QString();
    }

    NetworkHealthMetrics metrics = m_healthMonitor->getHealthMetrics();
    
    QJsonObject obj;
    obj["status"] = static_cast<int>(metrics.status);
    obj["connected_peers"] = metrics.connectedPeers;
    obj["average_latency"] = metrics.averageLatency;
    obj["bandwidth_utilization"] = metrics.bandwidthUtilization;
    obj["packet_loss"] = metrics.packetLoss;
    obj["last_update"] = metrics.lastUpdate.toString(Qt::ISODate);

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetNetworkTopology() {
    if (!m_healthMonitor) {
        return QString();
    }

    QVariantMap topology = m_healthMonitor->getTopology();
    
    QJsonDocument doc(QJsonObject::fromVariantMap(topology));
    return QString::fromUtf8(doc.toJson());
}

bool SecureMessengerDBusInterface::ConnectVPN(const QString& vpnConfig) {
    if (!m_networkManager) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(vpnConfig.toUtf8());
    if (doc.isNull()) {
        return false;
    }

    QVariantMap config = doc.object().toVariantMap();
    return m_networkManager->connectVPN(config);
}

bool SecureMessengerDBusInterface::DisconnectVPN() {
    if (!m_networkManager) {
        return false;
    }

    return m_networkManager->disconnectVPN();
}

bool SecureMessengerDBusInterface::BlockUnauthorizedInterfaces() {
    if (!m_enforcement) {
        return false;
    }

    return m_enforcement->blockUnauthorizedInterfaces();
}

QString SecureMessengerDBusInterface::SendTextMessage(const QString& conversationId, const QString& recipientId, const QString& text, int formatType) {
    if (!m_textMessaging) {
        return QString();
    }

    return m_textMessaging->sendTextMessage(conversationId, recipientId, text, static_cast<TextFormatType>(formatType));
}

QString SecureMessengerDBusInterface::GetMessage(const QString& messageId) {
    if (!m_messagingCore) {
        return QString();
    }

    Message message = m_messagingCore->getMessage(messageId);
    if (message.messageId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["message_id"] = message.messageId;
    obj["conversation_id"] = message.conversationId;
    obj["sender_id"] = message.senderId;
    obj["recipient_id"] = message.recipientId;
    obj["type"] = static_cast<int>(message.type);
    obj["content"] = message.content;
    obj["status"] = static_cast<int>(message.status);
    obj["timestamp"] = message.timestamp.toString(Qt::ISODate);
    if (message.deliveredAt.isValid()) {
        obj["delivered_at"] = message.deliveredAt.toString(Qt::ISODate);
    }
    if (message.readAt.isValid()) {
        obj["read_at"] = message.readAt.toString(Qt::ISODate);
    }

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetMessagesForConversation(const QString& conversationId, int limit, int offset) {
    if (!m_messageStorage) {
        return QString();
    }

    QList<Message> messages = m_messageStorage->getMessagesForConversation(conversationId, limit, offset);
    
    QJsonArray jsonArray;
    for (const Message& message : messages) {
        QJsonObject obj;
        obj["message_id"] = message.messageId;
        obj["conversation_id"] = message.conversationId;
        obj["sender_id"] = message.senderId;
        obj["recipient_id"] = message.recipientId;
        obj["type"] = static_cast<int>(message.type);
        obj["content"] = message.content;
        obj["status"] = static_cast<int>(message.status);
        obj["timestamp"] = message.timestamp.toString(Qt::ISODate);
        if (message.deliveredAt.isValid()) {
            obj["delivered_at"] = message.deliveredAt.toString(Qt::ISODate);
        }
        if (message.readAt.isValid()) {
            obj["read_at"] = message.readAt.toString(Qt::ISODate);
        }
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

bool SecureMessengerDBusInterface::MarkMessageAsRead(const QString& messageId) {
    if (!m_messagingCore) {
        return false;
    }

    return m_messagingCore->updateMessageStatus(messageId, MessageStatus::Read);
}

QString SecureMessengerDBusInterface::CreateConversation(int type, const QString& participants, const QString& title) {
    if (!m_conversationManager) {
        return QString();
    }

    QJsonDocument doc = QJsonDocument::fromJson(participants.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        return QString();
    }

    QList<QString> participantList;
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        participantList.append(value.toString());
    }

    return m_conversationManager->createConversation(static_cast<ConversationType>(type), participantList, title);
}

QString SecureMessengerDBusInterface::GetConversation(const QString& conversationId) {
    if (!m_conversationManager) {
        return QString();
    }

    Conversation conv = m_conversationManager->getConversation(conversationId);
    if (conv.conversationId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["conversation_id"] = conv.conversationId;
    obj["type"] = static_cast<int>(conv.type);
    QJsonArray participantsArray;
    for (const QString& participant : conv.participants) {
        participantsArray.append(participant);
    }
    obj["participants"] = participantsArray;
    obj["title"] = conv.title;
    obj["created_at"] = conv.createdAt.toString(Qt::ISODate);
    obj["last_message_at"] = conv.lastMessageAt.toString(Qt::ISODate);
    obj["unread_count"] = conv.unreadCount;

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetConversationsForUser(const QString& userId) {
    if (!m_conversationManager) {
        return QString();
    }

    QList<Conversation> conversations = m_conversationManager->getConversationsForUser(userId);
    
    QJsonArray jsonArray;
    for (const Conversation& conv : conversations) {
        QJsonObject obj;
        obj["conversation_id"] = conv.conversationId;
        obj["type"] = static_cast<int>(conv.type);
        QJsonArray participantsArray;
        for (const QString& participant : conv.participants) {
            participantsArray.append(participant);
        }
        obj["participants"] = participantsArray;
        obj["title"] = conv.title;
        obj["created_at"] = conv.createdAt.toString(Qt::ISODate);
        obj["last_message_at"] = conv.lastMessageAt.toString(Qt::ISODate);
        obj["unread_count"] = conv.unreadCount;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::SearchConversations(const QString& query, const QString& userId) {
    if (!m_conversationManager) {
        return QString();
    }

    QList<Conversation> conversations = m_conversationManager->searchConversations(query, userId);
    
    QJsonArray jsonArray;
    for (const Conversation& conv : conversations) {
        QJsonObject obj;
        obj["conversation_id"] = conv.conversationId;
        obj["type"] = static_cast<int>(conv.type);
        obj["title"] = conv.title;
        obj["created_at"] = conv.createdAt.toString(Qt::ISODate);
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::CreateThread(const QString& conversationId, const QString& title) {
    if (!m_threading) {
        return QString();
    }

    return m_threading->createThread(conversationId, title);
}

QString SecureMessengerDBusInterface::GetThreadsForConversation(const QString& conversationId) {
    if (!m_threading) {
        return QString();
    }

    QList<MessageThread> threads = m_threading->getThreadsForConversation(conversationId);
    
    QJsonArray jsonArray;
    for (const MessageThread& thread : threads) {
        QJsonObject obj;
        obj["thread_id"] = thread.threadId;
        obj["conversation_id"] = thread.conversationId;
        obj["title"] = thread.title;
        obj["created_at"] = thread.createdAt.toString(Qt::ISODate);
        obj["last_message_at"] = thread.lastMessageAt.toString(Qt::ISODate);
        obj["unread_count"] = thread.unreadCount;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

