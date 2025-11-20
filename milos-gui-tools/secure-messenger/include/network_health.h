#ifndef NETWORK_HEALTH_H
#define NETWORK_HEALTH_H

#include "mesh_network.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Network Health Status
 */
enum class NetworkHealthStatus {
    Healthy,
    Degraded,
    Unhealthy,
    Offline
};

/**
 * @brief Network Health Metrics
 */
struct NetworkHealthMetrics {
    NetworkHealthStatus status;
    int connectedPeers;
    double averageLatency;
    double bandwidthUtilization;
    int packetLoss;
    QDateTime lastUpdate;
};

/**
 * @brief Network Health Monitor
 * 
 * Monitors network health and provides topology visualization data.
 */
class NetworkHealthMonitor : public QObject {
    Q_OBJECT

public:
    explicit NetworkHealthMonitor(QObject* parent = nullptr);
    ~NetworkHealthMonitor();

    /**
     * @brief Initialize network health monitor
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get network health metrics
     * @return Network health metrics
     */
    NetworkHealthMetrics getHealthMetrics() const;

    /**
     * @brief Get network topology
     * @return Topology data (JSON-like structure)
     */
    QVariantMap getTopology() const;

    /**
     * @brief Start monitoring
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop monitoring
     */
    void stop();

Q_SIGNALS:
    /**
     * @brief Emitted when health status changes
     */
    void healthStatusChanged(NetworkHealthStatus status);

    /**
     * @brief Emitted when topology is updated
     */
    void topologyUpdated();

private:
    NetworkHealthMetrics m_metrics;
    QVariantMap m_topology;
    bool m_monitoring;
    
    void updateHealthMetrics();
    void updateTopology();
};

#endif // NETWORK_HEALTH_H

