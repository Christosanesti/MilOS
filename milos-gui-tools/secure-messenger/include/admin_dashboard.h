#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>

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
     * @brief Get network overview
     * @return Network overview data
     */
    QVariantMap getNetworkOverview() const;

    /**
     * @brief Get system status
     * @return System status data
     */
    QVariantMap getSystemStatus() const;

    /**
     * @brief Get message delivery statistics
     * @return Delivery statistics
     */
    QVariantMap getDeliveryStatistics() const;

    /**
     * @brief Get network connectivity status
     * @return Connectivity status
     */
    QVariantMap getConnectivityStatus() const;

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
};

#endif // ADMIN_DASHBOARD_H

