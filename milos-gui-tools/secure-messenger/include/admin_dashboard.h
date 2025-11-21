#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>

class UserEnrollment;
class ConversationManager;
class MessagingCore;
class MessageStorage;
class MeshNetwork;
class NetworkHealthMonitor;
class E2EEncryption;
class ForwardSecrecy;

/**
 * @brief Admin Dashboard
 * 
 * Provides commanding officer dashboard for network oversight and management.
 */
class AdminDashboard : public QObject {
    Q_OBJECT

public:
    explicit AdminDashboard(QObject* parent = nullptr);
    ~AdminDashboard();

    /**
     * @brief Initialize admin dashboard
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Set user enrollment (for user statistics)
     */
    void setUserEnrollment(UserEnrollment* enrollment);

    /**
     * @brief Set conversation manager (for conversation statistics)
     */
    void setConversationManager(ConversationManager* conversationManager);

    /**
     * @brief Set messaging core (for message statistics)
     */
    void setMessagingCore(MessagingCore* messagingCore);

    /**
     * @brief Set message storage (for message statistics)
     */
    void setMessageStorage(MessageStorage* messageStorage);

    /**
     * @brief Set mesh network (for network statistics)
     */
    void setMeshNetwork(MeshNetwork* meshNetwork);

    /**
     * @brief Set network health monitor (for health metrics)
     */
    void setNetworkHealthMonitor(NetworkHealthMonitor* healthMonitor);

    /**
     * @brief Set E2E encryption (for security status)
     */
    void setE2EEncryption(E2EEncryption* e2eEncryption);

    /**
     * @brief Set forward secrecy (for security status)
     */
    void setForwardSecrecy(ForwardSecrecy* forwardSecrecy);

    /**
     * @brief Get network overview
     * @return Network overview data
     */
    Q_INVOKABLE QVariantMap getNetworkOverview() const;

    /**
     * @brief Get system status
     * @return System status data
     */
    Q_INVOKABLE QVariantMap getSystemStatus() const;

    /**
     * @brief Get message delivery statistics
     * @return Delivery statistics
     */
    Q_INVOKABLE QVariantMap getDeliveryStatistics() const;

    /**
     * @brief Get network connectivity status
     * @return Connectivity status
     */
    Q_INVOKABLE QVariantMap getConnectivityStatus() const;

    /**
     * @brief Get dashboard data (combined overview)
     * @return Dashboard data map
     */
    Q_INVOKABLE QVariantMap getDashboardData() const;

    /**
     * @brief Get system statistics
     * @return System statistics map
     */
    Q_INVOKABLE QVariantMap getSystemStatistics() const;

Q_SIGNALS:
    /**
     * @brief Emitted when network status changes
     */
    void networkStatusChanged();

    /**
     * @brief Emitted when system status changes
     */
    void systemStatusChanged();

private:
    void updateNetworkOverview();
    void updateSystemStatus();
    QVariantMap m_networkOverview;
    QVariantMap m_systemStatus;
    
    UserEnrollment* m_userEnrollment;
    ConversationManager* m_conversationManager;
    MessagingCore* m_messagingCore;
    MessageStorage* m_messageStorage;
    MeshNetwork* m_meshNetwork;
    NetworkHealthMonitor* m_healthMonitor;
    E2EEncryption* m_e2eEncryption;
    ForwardSecrecy* m_forwardSecrecy;
    QDateTime m_startTime;
};

#endif // ADMIN_DASHBOARD_H

