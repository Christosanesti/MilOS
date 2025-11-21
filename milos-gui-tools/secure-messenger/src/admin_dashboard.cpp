#include "admin_dashboard.h"
#include "user_enrollment.h"
#include "conversation_manager.h"
#include "messaging_core.h"
#include "message_storage.h"
#include "mesh_network.h"
#include "network_health.h"
#include "e2e_encryption.h"
#include "forward_secrecy.h"
#include <QDebug>

AdminDashboard::AdminDashboard(QObject* parent)
    : QObject(parent)
    , m_userEnrollment(nullptr)
    , m_conversationManager(nullptr)
    , m_messagingCore(nullptr)
    , m_messageStorage(nullptr)
    , m_meshNetwork(nullptr)
    , m_healthMonitor(nullptr)
    , m_e2eEncryption(nullptr)
    , m_forwardSecrecy(nullptr)
    , m_startTime(QDateTime::currentDateTime())
{
}

AdminDashboard::~AdminDashboard() {
}

bool AdminDashboard::initialize() {
    updateNetworkOverview();
    updateSystemStatus();
    return true;
}

void AdminDashboard::setUserEnrollment(UserEnrollment* enrollment) {
    m_userEnrollment = enrollment;
}

void AdminDashboard::setConversationManager(ConversationManager* conversationManager) {
    m_conversationManager = conversationManager;
}

void AdminDashboard::setMessagingCore(MessagingCore* messagingCore) {
    m_messagingCore = messagingCore;
}

void AdminDashboard::setMessageStorage(MessageStorage* messageStorage) {
    m_messageStorage = messageStorage;
}

void AdminDashboard::setMeshNetwork(MeshNetwork* meshNetwork) {
    m_meshNetwork = meshNetwork;
}

void AdminDashboard::setNetworkHealthMonitor(NetworkHealthMonitor* healthMonitor) {
    m_healthMonitor = healthMonitor;
}

void AdminDashboard::setE2EEncryption(E2EEncryption* e2eEncryption) {
    m_e2eEncryption = e2eEncryption;
}

void AdminDashboard::setForwardSecrecy(ForwardSecrecy* forwardSecrecy) {
    m_forwardSecrecy = forwardSecrecy;
}

QVariantMap AdminDashboard::getNetworkOverview() const {
    return m_networkOverview;
}

QVariantMap AdminDashboard::getSystemStatus() const {
    return m_systemStatus;
}

QVariantMap AdminDashboard::getDeliveryStatistics() const {
    if (m_messageStorage) {
        // Get real message statistics from message storage
        return m_messageStorage->getMessageStatistics();
    }
    
    // Fallback to empty stats if message storage not available
    QVariantMap stats;
    stats["total_messages"] = 0;
    stats["delivered"] = 0;
    stats["pending"] = 0;
    stats["failed"] = 0;
    stats["delivery_rate"] = 0.0;
    return stats;
}

QVariantMap AdminDashboard::getConnectivityStatus() const {
    QVariantMap status;
    
    if (m_meshNetwork) {
        // Get peer count from mesh network
        QList<QString> peers = m_meshNetwork->getAllPeers();
        status["connected_peers"] = peers.size();
    } else {
        status["connected_peers"] = 0;
    }
    
    if (m_healthMonitor) {
        NetworkHealthMetrics metrics = m_healthMonitor->getHealthMetrics();
        QString healthStatus = "Unknown";
        if (metrics.latencyMs < 50 && metrics.packetLossPercent < 1.0) {
            healthStatus = "Healthy";
        } else if (metrics.latencyMs < 100 && metrics.packetLossPercent < 5.0) {
            healthStatus = "Degraded";
        } else {
            healthStatus = "Unhealthy";
        }
        status["network_health"] = healthStatus;
        status["network_latency"] = metrics.latencyMs;
    } else {
        status["network_health"] = "Unknown";
        status["network_latency"] = 0;
    }
    
    status["last_update"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return status;
}

void AdminDashboard::updateNetworkOverview() {
    if (m_meshNetwork) {
        QList<QString> peers = m_meshNetwork->getAllPeers();
        m_networkOverview["total_peers"] = peers.size();
        m_networkOverview["active_connections"] = peers.size();
    } else {
        m_networkOverview["total_peers"] = 0;
        m_networkOverview["active_connections"] = 0;
    }
    
    if (m_healthMonitor) {
        NetworkHealthMetrics metrics = m_healthMonitor->getHealthMetrics();
        QString status = "Unknown";
        if (metrics.latencyMs < 50 && metrics.packetLossPercent < 1.0) {
            status = "Healthy";
        } else if (metrics.latencyMs < 100 && metrics.packetLossPercent < 5.0) {
            status = "Degraded";
        } else {
            status = "Unhealthy";
        }
        m_networkOverview["network_status"] = status;
    } else {
        m_networkOverview["network_status"] = "Unknown";
    }
    
    m_networkOverview["last_update"] = QDateTime::currentDateTime().toString(Qt::ISODate);
}

void AdminDashboard::updateSystemStatus() {
    m_systemStatus["status"] = "Running";
    
    // Calculate uptime
    qint64 uptimeSeconds = m_startTime.secsTo(QDateTime::currentDateTime());
    m_systemStatus["uptime"] = uptimeSeconds;
    
    // System resource usage would be obtained from system monitoring
    // For now, use placeholder values
    m_systemStatus["memory_usage"] = 0;
    m_systemStatus["cpu_usage"] = 0.0;
    
    m_systemStatus["last_update"] = QDateTime::currentDateTime().toString(Qt::ISODate);
}

QVariantMap AdminDashboard::getDashboardData() const {
    QVariantMap data;
    
    // Combine network overview and system status
    QVariantMap network = getNetworkOverview();
    QVariantMap system = getSystemStatus();
    QVariantMap delivery = getDeliveryStatistics();
    QVariantMap connectivity = getConnectivityStatus();
    
    // Active users
    if (m_userEnrollment) {
        // Get all enrolled and approved users
        QList<UserInfo> allUsers = m_userEnrollment->getAllUsers();
        int activeCount = 0;
        for (const UserInfo& user : allUsers) {
            if (user.status == EnrollmentStatus::Approved || user.status == EnrollmentStatus::Active) {
                activeCount++;
            }
        }
        data["activeUsers"] = activeCount;
    } else {
        data["activeUsers"] = 0;
    }
    
    // Total messages (from delivery statistics)
    data["totalMessages"] = delivery.value("total_messages", 0);
    
    // Network peers (from connectivity)
    data["networkPeers"] = connectivity.value("connected_peers", 0);
    
    // Active conversations
    if (m_conversationManager) {
        // Get all conversations count
        QList<Conversation> allConversations = m_conversationManager->getAllConversations();
        data["activeConversations"] = allConversations.size();
    } else {
        data["activeConversations"] = 0;
    }
    
    // Network status
    data["networkStatus"] = connectivity.value("network_health", "Unknown");
    data["networkLatency"] = connectivity.value("network_latency", 0);
    
    // Security status
    data["e2eEnabled"] = (m_e2eEncryption != nullptr);
    data["forwardSecrecyEnabled"] = (m_forwardSecrecy != nullptr);
    
    return data;
}

QVariantMap AdminDashboard::getSystemStatistics() const {
    QVariantMap stats;
    
    QVariantMap network = getNetworkOverview();
    QVariantMap system = getSystemStatus();
    QVariantMap delivery = getDeliveryStatistics();
    QVariantMap connectivity = getConnectivityStatus();
    
    // System metrics
    stats["status"] = system.value("status", "Unknown");
    stats["uptime"] = system.value("uptime", 0);
    stats["memory_usage"] = system.value("memory_usage", 0);
    stats["cpu_usage"] = system.value("cpu_usage", 0.0);
    
    // Network metrics
    stats["total_peers"] = network.value("total_peers", 0);
    stats["active_connections"] = network.value("active_connections", 0);
    stats["network_status"] = network.value("network_status", "Unknown");
    
    // Message metrics
    stats["total_messages"] = delivery.value("total_messages", 0);
    stats["delivered"] = delivery.value("delivered", 0);
    stats["pending"] = delivery.value("pending", 0);
    stats["failed"] = delivery.value("failed", 0);
    stats["delivery_rate"] = delivery.value("delivery_rate", 0.0);
    
    // Connectivity
    stats["connected_peers"] = connectivity.value("connected_peers", 0);
    stats["network_health"] = connectivity.value("network_health", "Unknown");
    
    stats["last_update"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return stats;
}

