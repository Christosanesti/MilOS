#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <iostream>
#include <string>

class SecureMessengerClient {
public:
    SecureMessengerClient() {
        m_connection = QDBusConnection::sessionBus();
        m_interface = new QDBusInterface(
            "org.milos.SecureMessenger",
            "/org/milos/SecureMessenger",
            "org.milos.SecureMessenger",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QString detectUSBDevices() {
        QDBusReply<QString> reply = m_interface->call("DetectUSBDevices");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString validateUSBKey(const QString& devicePath) {
        QDBusReply<QString> reply = m_interface->call("ValidateUSBKey", devicePath);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString generateKey(const QString& userId, int role, int expirationDays) {
        QDBusReply<QString> reply = m_interface->call("GenerateKey", userId, role, expirationDays);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool writeKeyToUSB(const QString& keyId, const QString& devicePath) {
        QDBusReply<bool> reply = m_interface->call("WriteKeyToUSB", keyId, devicePath);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool distributeKey(const QString& keyId, const QString& devicePath) {
        QDBusReply<bool> reply = m_interface->call("DistributeKey", keyId, devicePath);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool revokeKey(const QString& keyId) {
        QDBusReply<bool> reply = m_interface->call("RevokeKey", keyId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getKeyInfo(const QString& keyId) {
        QDBusReply<QString> reply = m_interface->call("GetKeyInfo", keyId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getAllKeys(const QString& userId) {
        QDBusReply<QString> reply = m_interface->call("GetAllKeys", userId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString enrollUser(const QString& username, const QString& email, int role, const QString& enrolledBy) {
        QDBusReply<QString> reply = m_interface->call("EnrollUser", username, email, role, enrolledBy);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool approveEnrollment(const QString& userId, const QString& approverId) {
        QDBusReply<bool> reply = m_interface->call("ApproveEnrollment", userId, approverId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool rejectEnrollment(const QString& userId, const QString& reason) {
        QDBusReply<bool> reply = m_interface->call("RejectEnrollment", userId, reason);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getUserInfo(const QString& userId) {
        QDBusReply<QString> reply = m_interface->call("GetUserInfo", userId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getAllUsers(int role) {
        QDBusReply<QString> reply = m_interface->call("GetAllUsers", role);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool checkPermission(const QString& userId, int permission) {
        QDBusReply<bool> reply = m_interface->call("CheckPermission", userId, permission);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getNetworkInterfaces() {
        QDBusReply<QString> reply = m_interface->call("GetNetworkInterfaces");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getAuthorizedInterfaces() {
        QDBusReply<QString> reply = m_interface->call("GetAuthorizedInterfaces");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString detectSegmentation() {
        QDBusReply<QString> reply = m_interface->call("DetectSegmentation");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString discoverPeers() {
        QDBusReply<QString> reply = m_interface->call("DiscoverPeers");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getAllPeers() {
        QDBusReply<QString> reply = m_interface->call("GetAllPeers");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getNetworkHealthMetrics() {
        QDBusReply<QString> reply = m_interface->call("GetNetworkHealthMetrics");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getNetworkTopology() {
        QDBusReply<QString> reply = m_interface->call("GetNetworkTopology");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool connectVPN(const QString& vpnConfig) {
        QDBusReply<bool> reply = m_interface->call("ConnectVPN", vpnConfig);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool disconnectVPN() {
        QDBusReply<bool> reply = m_interface->call("DisconnectVPN");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool blockUnauthorizedInterfaces() {
        QDBusReply<bool> reply = m_interface->call("BlockUnauthorizedInterfaces");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString sendTextMessage(const QString& conversationId, const QString& recipientId, const QString& text, int formatType) {
        QDBusReply<QString> reply = m_interface->call("SendTextMessage", conversationId, recipientId, text, formatType);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getMessage(const QString& messageId) {
        QDBusReply<QString> reply = m_interface->call("GetMessage", messageId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getMessagesForConversation(const QString& conversationId, int limit, int offset) {
        QDBusReply<QString> reply = m_interface->call("GetMessagesForConversation", conversationId, limit, offset);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool markMessageAsRead(const QString& messageId) {
        QDBusReply<bool> reply = m_interface->call("MarkMessageAsRead", messageId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString createConversation(int type, const QString& participants, const QString& title) {
        QDBusReply<QString> reply = m_interface->call("CreateConversation", type, participants, title);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getConversation(const QString& conversationId) {
        QDBusReply<QString> reply = m_interface->call("GetConversation", conversationId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getConversationsForUser(const QString& userId) {
        QDBusReply<QString> reply = m_interface->call("GetConversationsForUser", userId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString searchConversations(const QString& query, const QString& userId) {
        QDBusReply<QString> reply = m_interface->call("SearchConversations", query, userId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString createThread(const QString& conversationId, const QString& title) {
        QDBusReply<QString> reply = m_interface->call("CreateThread", conversationId, title);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getThreadsForConversation(const QString& conversationId) {
        QDBusReply<QString> reply = m_interface->call("GetThreadsForConversation", conversationId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

private:
    QDBusConnection m_connection;
    QDBusInterface* m_interface;
};

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    SecureMessengerClient client;
    if (!client.isConnected()) {
        std::cerr << "Failed to connect to Secure Messenger D-Bus service" << std::endl;
        return 1;
    }

    CLI::App cliApp("milos-messenger", "MilOS Secure Messenger CLI Tool");

    // Detect USB devices command
    auto* detectCmd = cliApp.add_subcommand("detect-usb", "Detect USB devices");
    detectCmd->callback([&]() {
        QString jsonDevices = client.detectUSBDevices();
        if (!jsonDevices.isEmpty()) {
            std::cout << jsonDevices.toStdString() << std::endl;
        } else {
            std::cout << "No USB devices detected" << std::endl;
        }
    });

    // Validate USB key command
    auto* validateCmd = cliApp.add_subcommand("validate-key", "Validate USB key");
    std::string devicePath;
    validateCmd->add_option("--device", devicePath, "USB device path")->required();
    validateCmd->callback([&]() {
        QString jsonKey = client.validateUSBKey(QString::fromStdString(devicePath));
        if (!jsonKey.isEmpty()) {
            std::cout << jsonKey.toStdString() << std::endl;
        } else {
            std::cerr << "Key validation failed" << std::endl;
        }
    });

    // Generate key command
    auto* generateCmd = cliApp.add_subcommand("generate-key", "Generate USB authorization key");
    std::string userId;
    int role = 0;
    int expirationDays = 0;
    generateCmd->add_option("--user", userId, "User ID")->required();
    generateCmd->add_option("--role", role, "Role (0=CommandingOfficer, 1=TechPersonnel, 2=User)")->required();
    generateCmd->add_option("--expiration", expirationDays, "Expiration in days (0 for no expiration)");
    generateCmd->callback([&]() {
        QString keyId = client.generateKey(QString::fromStdString(userId), role, expirationDays);
        if (!keyId.isEmpty()) {
            std::cout << "Key generated: " << keyId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to generate key" << std::endl;
        }
    });

    // Write key to USB command
    auto* writeCmd = cliApp.add_subcommand("write-key", "Write key to USB device");
    std::string writeKeyId;
    std::string writeDevicePath;
    writeCmd->add_option("--key", writeKeyId, "Key ID")->required();
    writeCmd->add_option("--device", writeDevicePath, "USB device path")->required();
    writeCmd->callback([&]() {
        bool success = client.writeKeyToUSB(QString::fromStdString(writeKeyId), QString::fromStdString(writeDevicePath));
        if (success) {
            std::cout << "Key written to USB device" << std::endl;
        } else {
            std::cerr << "Failed to write key to USB device" << std::endl;
        }
    });

    // Distribute key command
    auto* distributeCmd = cliApp.add_subcommand("distribute-key", "Distribute key");
    std::string distKeyId;
    std::string distDevicePath;
    distributeCmd->add_option("--key", distKeyId, "Key ID")->required();
    distributeCmd->add_option("--device", distDevicePath, "USB device path")->required();
    distributeCmd->callback([&]() {
        bool success = client.distributeKey(QString::fromStdString(distKeyId), QString::fromStdString(distDevicePath));
        if (success) {
            std::cout << "Key distributed" << std::endl;
        } else {
            std::cerr << "Failed to distribute key" << std::endl;
        }
    });

    // Revoke key command
    auto* revokeCmd = cliApp.add_subcommand("revoke-key", "Revoke key");
    std::string revokeKeyId;
    revokeCmd->add_option("--key", revokeKeyId, "Key ID")->required();
    revokeCmd->callback([&]() {
        bool success = client.revokeKey(QString::fromStdString(revokeKeyId));
        if (success) {
            std::cout << "Key revoked" << std::endl;
        } else {
            std::cerr << "Failed to revoke key" << std::endl;
        }
    });

    // Get key info command
    auto* keyInfoCmd = cliApp.add_subcommand("key-info", "Get key information");
    std::string keyInfoId;
    keyInfoCmd->add_option("--key", keyInfoId, "Key ID")->required();
    keyInfoCmd->callback([&]() {
        QString jsonKey = client.getKeyInfo(QString::fromStdString(keyInfoId));
        if (!jsonKey.isEmpty()) {
            std::cout << jsonKey.toStdString() << std::endl;
        } else {
            std::cout << "Key not found" << std::endl;
        }
    });

    // Get all keys command
    auto* allKeysCmd = cliApp.add_subcommand("list-keys", "List all keys");
    std::string allKeysUserId;
    allKeysCmd->add_option("--user", allKeysUserId, "User ID filter");
    allKeysCmd->callback([&]() {
        QString jsonKeys = client.getAllKeys(QString::fromStdString(allKeysUserId));
        if (!jsonKeys.isEmpty()) {
            std::cout << jsonKeys.toStdString() << std::endl;
        } else {
            std::cout << "No keys found" << std::endl;
        }
    });

    // Enroll user command
    auto* enrollCmd = cliApp.add_subcommand("enroll", "Enroll user");
    std::string username;
    std::string email;
    int enrollRole = 0;
    std::string enrolledBy;
    enrollCmd->add_option("--username", username, "Username")->required();
    enrollCmd->add_option("--email", email, "Email address")->required();
    enrollCmd->add_option("--role", enrollRole, "Role (0=CommandingOfficer, 1=TechPersonnel, 2=User)")->required();
    enrollCmd->add_option("--enrolled-by", enrolledBy, "Enrolling officer user ID")->required();
    enrollCmd->callback([&]() {
        QString userId = client.enrollUser(QString::fromStdString(username), QString::fromStdString(email), enrollRole, QString::fromStdString(enrolledBy));
        if (!userId.isEmpty()) {
            std::cout << "User enrolled: " << userId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to enroll user" << std::endl;
        }
    });

    // Approve enrollment command
    auto* approveCmd = cliApp.add_subcommand("approve", "Approve enrollment");
    std::string approveUserId;
    std::string approverId;
    approveCmd->add_option("--user", approveUserId, "User ID")->required();
    approveCmd->add_option("--approver", approverId, "Approver user ID")->required();
    approveCmd->callback([&]() {
        bool success = client.approveEnrollment(QString::fromStdString(approveUserId), QString::fromStdString(approverId));
        if (success) {
            std::cout << "Enrollment approved" << std::endl;
        } else {
            std::cerr << "Failed to approve enrollment" << std::endl;
        }
    });

    // Reject enrollment command
    auto* rejectCmd = cliApp.add_subcommand("reject", "Reject enrollment");
    std::string rejectUserId;
    std::string reason;
    rejectCmd->add_option("--user", rejectUserId, "User ID")->required();
    rejectCmd->add_option("--reason", reason, "Rejection reason")->required();
    rejectCmd->callback([&]() {
        bool success = client.rejectEnrollment(QString::fromStdString(rejectUserId), QString::fromStdString(reason));
        if (success) {
            std::cout << "Enrollment rejected" << std::endl;
        } else {
            std::cerr << "Failed to reject enrollment" << std::endl;
        }
    });

    // Get user info command
    auto* userInfoCmd = cliApp.add_subcommand("user-info", "Get user information");
    std::string userInfoId;
    userInfoCmd->add_option("--user", userInfoId, "User ID")->required();
    userInfoCmd->callback([&]() {
        QString jsonUser = client.getUserInfo(QString::fromStdString(userInfoId));
        if (!jsonUser.isEmpty()) {
            std::cout << jsonUser.toStdString() << std::endl;
        } else {
            std::cout << "User not found" << std::endl;
        }
    });

    // Get all users command
    auto* allUsersCmd = cliApp.add_subcommand("list-users", "List all users");
    int allUsersRole = -1;
    allUsersCmd->add_option("--role", allUsersRole, "Role filter (-1 for all)");
    allUsersCmd->callback([&]() {
        QString jsonUsers = client.getAllUsers(allUsersRole);
        if (!jsonUsers.isEmpty()) {
            std::cout << jsonUsers.toStdString() << std::endl;
        } else {
            std::cout << "No users found" << std::endl;
        }
    });

    // Check permission command
    auto* checkPermCmd = cliApp.add_subcommand("check-permission", "Check permission");
    std::string checkPermUserId;
    int checkPermPermission = 0;
    checkPermCmd->add_option("--user", checkPermUserId, "User ID")->required();
    checkPermCmd->add_option("--permission", checkPermPermission, "Permission (0=GenerateKeys, 1=DistributeKeys, 2=RevokeKeys, 3=EnrollUsers, 4=ApproveEnrollments, 5=ManageNetwork, 6=ViewLogs)")->required();
    checkPermCmd->callback([&]() {
        bool hasPermission = client.checkPermission(QString::fromStdString(checkPermUserId), checkPermPermission);
        std::cout << "Permission: " << (hasPermission ? "Granted" : "Denied") << std::endl;
    });

    // Get network interfaces command
    auto* interfacesCmd = cliApp.add_subcommand("interfaces", "List network interfaces");
    interfacesCmd->callback([&]() {
        QString jsonInterfaces = client.getNetworkInterfaces();
        if (!jsonInterfaces.isEmpty()) {
            std::cout << jsonInterfaces.toStdString() << std::endl;
        } else {
            std::cout << "No network interfaces found" << std::endl;
        }
    });

    // Get authorized interfaces command
    auto* authorizedCmd = cliApp.add_subcommand("authorized-interfaces", "List authorized network interfaces");
    authorizedCmd->callback([&]() {
        QString jsonInterfaces = client.getAuthorizedInterfaces();
        if (!jsonInterfaces.isEmpty()) {
            std::cout << jsonInterfaces.toStdString() << std::endl;
        } else {
            std::cout << "No authorized interfaces found" << std::endl;
        }
    });

    // Detect segmentation command
    auto* segmentationCmd = cliApp.add_subcommand("segmentation", "Detect network segmentation");
    segmentationCmd->callback([&]() {
        QString jsonSubnets = client.detectSegmentation();
        if (!jsonSubnets.isEmpty()) {
            std::cout << jsonSubnets.toStdString() << std::endl;
        } else {
            std::cout << "No network segments detected" << std::endl;
        }
    });

    // Discover peers command
    auto* discoverCmd = cliApp.add_subcommand("discover-peers", "Discover peers");
    discoverCmd->callback([&]() {
        QString jsonPeers = client.discoverPeers();
        if (!jsonPeers.isEmpty()) {
            std::cout << jsonPeers.toStdString() << std::endl;
        } else {
            std::cout << "No peers discovered" << std::endl;
        }
    });

    // Get all peers command
    auto* peersCmd = cliApp.add_subcommand("peers", "List all peers");
    peersCmd->callback([&]() {
        QString jsonPeers = client.getAllPeers();
        if (!jsonPeers.isEmpty()) {
            std::cout << jsonPeers.toStdString() << std::endl;
        } else {
            std::cout << "No peers found" << std::endl;
        }
    });

    // Get network health metrics command
    auto* healthCmd = cliApp.add_subcommand("network-health", "Get network health metrics");
    healthCmd->callback([&]() {
        QString jsonHealth = client.getNetworkHealthMetrics();
        if (!jsonHealth.isEmpty()) {
            std::cout << jsonHealth.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to get network health metrics" << std::endl;
        }
    });

    // Get network topology command
    auto* topologyCmd = cliApp.add_subcommand("topology", "Get network topology");
    topologyCmd->callback([&]() {
        QString jsonTopology = client.getNetworkTopology();
        if (!jsonTopology.isEmpty()) {
            std::cout << jsonTopology.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to get network topology" << std::endl;
        }
    });

    // Connect VPN command
    auto* vpnConnectCmd = cliApp.add_subcommand("vpn-connect", "Connect VPN");
    std::string vpnConfig;
    vpnConnectCmd->add_option("--config", vpnConfig, "VPN configuration (JSON string)")->required();
    vpnConnectCmd->callback([&]() {
        bool success = client.connectVPN(QString::fromStdString(vpnConfig));
        if (success) {
            std::cout << "VPN connected" << std::endl;
        } else {
            std::cerr << "Failed to connect VPN" << std::endl;
        }
    });

    // Disconnect VPN command
    auto* vpnDisconnectCmd = cliApp.add_subcommand("vpn-disconnect", "Disconnect VPN");
    vpnDisconnectCmd->callback([&]() {
        bool success = client.disconnectVPN();
        if (success) {
            std::cout << "VPN disconnected" << std::endl;
        } else {
            std::cerr << "Failed to disconnect VPN" << std::endl;
        }
    });

    // Block unauthorized interfaces command
    auto* blockCmd = cliApp.add_subcommand("block-interfaces", "Block unauthorized interfaces");
    blockCmd->callback([&]() {
        bool success = client.blockUnauthorizedInterfaces();
        if (success) {
            std::cout << "Unauthorized interfaces blocked" << std::endl;
        } else {
            std::cerr << "Failed to block unauthorized interfaces" << std::endl;
        }
    });

    // Send text message command
    auto* sendMsgCmd = cliApp.add_subcommand("send", "Send text message");
    std::string sendConversationId;
    std::string sendRecipientId;
    std::string sendText;
    int sendFormatType = 0;
    sendMsgCmd->add_option("--conversation", sendConversationId, "Conversation ID")->required();
    sendMsgCmd->add_option("--recipient", sendRecipientId, "Recipient ID")->required();
    sendMsgCmd->add_option("--text", sendText, "Message text")->required();
    sendMsgCmd->add_option("--format", sendFormatType, "Format type (0=Plain, 1=Markdown, 2=HTML)");
    sendMsgCmd->callback([&]() {
        QString messageId = client.sendTextMessage(QString::fromStdString(sendConversationId),
                                                   QString::fromStdString(sendRecipientId),
                                                   QString::fromStdString(sendText),
                                                   sendFormatType);
        if (!messageId.isEmpty()) {
            std::cout << "Message sent: " << messageId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to send message" << std::endl;
        }
    });

    // Get message command
    auto* getMsgCmd = cliApp.add_subcommand("message", "Get message");
    std::string getMsgId;
    getMsgCmd->add_option("--id", getMsgId, "Message ID")->required();
    getMsgCmd->callback([&]() {
        QString jsonMessage = client.getMessage(QString::fromStdString(getMsgId));
        if (!jsonMessage.isEmpty()) {
            std::cout << jsonMessage.toStdString() << std::endl;
        } else {
            std::cout << "Message not found" << std::endl;
        }
    });

    // Get messages for conversation command
    auto* getMsgsCmd = cliApp.add_subcommand("messages", "Get messages for conversation");
    std::string getMsgsConversationId;
    int getMsgsLimit = 100;
    int getMsgsOffset = 0;
    getMsgsCmd->add_option("--conversation", getMsgsConversationId, "Conversation ID")->required();
    getMsgsCmd->add_option("--limit", getMsgsLimit, "Maximum number of messages");
    getMsgsCmd->add_option("--offset", getMsgsOffset, "Offset for pagination");
    getMsgsCmd->callback([&]() {
        QString jsonMessages = client.getMessagesForConversation(QString::fromStdString(getMsgsConversationId),
                                                                  getMsgsLimit,
                                                                  getMsgsOffset);
        if (!jsonMessages.isEmpty()) {
            std::cout << jsonMessages.toStdString() << std::endl;
        } else {
            std::cout << "No messages found" << std::endl;
        }
    });

    // Mark message as read command
    auto* markReadCmd = cliApp.add_subcommand("mark-read", "Mark message as read");
    std::string markReadMsgId;
    markReadCmd->add_option("--id", markReadMsgId, "Message ID")->required();
    markReadCmd->callback([&]() {
        bool success = client.markMessageAsRead(QString::fromStdString(markReadMsgId));
        if (success) {
            std::cout << "Message marked as read" << std::endl;
        } else {
            std::cerr << "Failed to mark message as read" << std::endl;
        }
    });

    // Create conversation command
    auto* createConvCmd = cliApp.add_subcommand("create-conversation", "Create conversation");
    int createConvType = 0;
    std::string createConvParticipants;
    std::string createConvTitle;
    createConvCmd->add_option("--type", createConvType, "Conversation type (0=Direct, 1=Group)")->required();
    createConvCmd->add_option("--participants", createConvParticipants, "Participant IDs (JSON array)")->required();
    createConvCmd->add_option("--title", createConvTitle, "Conversation title");
    createConvCmd->callback([&]() {
        QString conversationId = client.createConversation(createConvType,
                                                          QString::fromStdString(createConvParticipants),
                                                          QString::fromStdString(createConvTitle));
        if (!conversationId.isEmpty()) {
            std::cout << "Conversation created: " << conversationId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to create conversation" << std::endl;
        }
    });

    // Get conversation command
    auto* getConvCmd = cliApp.add_subcommand("conversation", "Get conversation");
    std::string getConvId;
    getConvCmd->add_option("--id", getConvId, "Conversation ID")->required();
    getConvCmd->callback([&]() {
        QString jsonConversation = client.getConversation(QString::fromStdString(getConvId));
        if (!jsonConversation.isEmpty()) {
            std::cout << jsonConversation.toStdString() << std::endl;
        } else {
            std::cout << "Conversation not found" << std::endl;
        }
    });

    // Get conversations for user command
    auto* getConvsCmd = cliApp.add_subcommand("conversations", "Get conversations for user");
    std::string getConvsUserId;
    getConvsCmd->add_option("--user", getConvsUserId, "User ID")->required();
    getConvsCmd->callback([&]() {
        QString jsonConversations = client.getConversationsForUser(QString::fromStdString(getConvsUserId));
        if (!jsonConversations.isEmpty()) {
            std::cout << jsonConversations.toStdString() << std::endl;
        } else {
            std::cout << "No conversations found" << std::endl;
        }
    });

    // Search conversations command
    auto* searchConvsCmd = cliApp.add_subcommand("search-conversations", "Search conversations");
    std::string searchQuery;
    std::string searchUserId;
    searchConvsCmd->add_option("--query", searchQuery, "Search query")->required();
    searchConvsCmd->add_option("--user", searchUserId, "User ID filter");
    searchConvsCmd->callback([&]() {
        QString jsonConversations = client.searchConversations(QString::fromStdString(searchQuery),
                                                               searchUserId.empty() ? QString() : QString::fromStdString(searchUserId));
        if (!jsonConversations.isEmpty()) {
            std::cout << jsonConversations.toStdString() << std::endl;
        } else {
            std::cout << "No conversations found" << std::endl;
        }
    });

    // Create thread command
    auto* createThreadCmd = cliApp.add_subcommand("create-thread", "Create thread");
    std::string createThreadConversationId;
    std::string createThreadTitle;
    createThreadCmd->add_option("--conversation", createThreadConversationId, "Conversation ID")->required();
    createThreadCmd->add_option("--title", createThreadTitle, "Thread title")->required();
    createThreadCmd->callback([&]() {
        QString threadId = client.createThread(QString::fromStdString(createThreadConversationId),
                                              QString::fromStdString(createThreadTitle));
        if (!threadId.isEmpty()) {
            std::cout << "Thread created: " << threadId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to create thread" << std::endl;
        }
    });

    // Get threads for conversation command
    auto* getThreadsCmd = cliApp.add_subcommand("threads", "Get threads for conversation");
    std::string getThreadsConversationId;
    getThreadsCmd->add_option("--conversation", getThreadsConversationId, "Conversation ID")->required();
    getThreadsCmd->callback([&]() {
        QString jsonThreads = client.getThreadsForConversation(QString::fromStdString(getThreadsConversationId));
        if (!jsonThreads.isEmpty()) {
            std::cout << jsonThreads.toStdString() << std::endl;
        } else {
            std::cout << "No threads found" << std::endl;
        }
    });

    CLI11_PARSE(cliApp, argc, argv);
    return 0;
}

