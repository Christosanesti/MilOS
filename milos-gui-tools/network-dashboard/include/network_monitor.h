#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <vector>
#include <map>
#include <cstdint>

// Forward declarations
class PacketCapture;
class PacketStatistics;
class QTimer;

/**
 * @brief Connection Information
 */
struct ConnectionInfo {
    QString source_address;
    QString destination_address;
    uint16_t source_port;
    uint16_t destination_port;
    QString protocol;
    uint64_t packet_count;
    uint64_t byte_count;
    uint64_t start_time;
    uint64_t last_seen;
    bool is_active;
};

/**
 * @brief Network Monitor
 * 
 * Backend class for real-time network monitoring and visualization.
 * Provides data models and signals for QML UI.
 */
class NetworkMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(uint64_t totalPackets READ totalPackets NOTIFY statisticsUpdated)
    Q_PROPERTY(uint64_t totalBytes READ totalBytes NOTIFY statisticsUpdated)
    Q_PROPERTY(QVariantMap protocolStats READ protocolStats NOTIFY statisticsUpdated)
    Q_PROPERTY(double currentBandwidth READ currentBandwidth NOTIFY bandwidthUpdated)
    Q_PROPERTY(QVariantList activeConnections READ activeConnections NOTIFY connectionsUpdated)
    Q_PROPERTY(bool running READ isRunning NOTIFY networkStatusChanged)

public:
    explicit NetworkMonitor(QObject* parent = nullptr);
    ~NetworkMonitor();

    /**
     * @brief Initialize network monitor
     * @param packetCapture Packet capture instance
     * @param packetStatistics Packet statistics instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(PacketCapture* packetCapture, PacketStatistics* packetStatistics);

    /**
     * @brief Start monitoring
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop monitoring
     */
    void stop();

    // Property getters
    uint64_t totalPackets() const { return m_totalPackets; }
    uint64_t totalBytes() const { return m_totalBytes; }
    QVariantMap protocolStats() const { return m_protocolStats; }
    double currentBandwidth() const { return m_currentBandwidth; }
    QVariantList activeConnections() const;

    /**
     * @brief Get connection details
     * @param connectionId Connection ID
     * @return Connection information as JSON
     */
    Q_INVOKABLE QJsonObject getConnectionDetails(const QString& connectionId) const;

    /**
     * @brief Get threat information
     * @return Threat information as JSON array
     */
    Q_INVOKABLE QJsonArray getThreats() const;

    /**
     * @brief Check if monitor is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

Q_SIGNALS:
    /**
     * @brief Emitted when statistics are updated
     */
    void statisticsUpdated();

    /**
     * @brief Emitted when bandwidth is updated
     */
    void bandwidthUpdated();

    /**
     * @brief Emitted when connections are updated
     */
    void connectionsUpdated();

    /**
     * @brief Emitted when network status changes
     */
    void networkStatusChanged(const QString& status);

    /**
     * @brief Emitted when new threat is detected
     */
    void threatDetected(const QJsonObject& threat);

private slots:
    /**
     * @brief Update statistics from packet statistics
     */
    void updateStatistics();

    /**
     * @brief Update bandwidth calculation
     */
    void updateBandwidth();

    /**
     * @brief Process captured packet
     */
    void processPacket(const PacketData& packet);

private:
    bool m_initialized;
    bool m_running;
    PacketCapture* m_packetCapture;
    PacketStatistics* m_packetStatistics;
    
    QTimer* m_statisticsTimer;
    QTimer* m_bandwidthTimer;

    uint64_t m_totalPackets;
    uint64_t m_totalBytes;
    QVariantMap m_protocolStats;
    double m_currentBandwidth;  // MB/s
    
    std::map<QString, ConnectionInfo> m_connections;
    uint64_t m_lastUpdateTime;
    uint64_t m_lastByteCount;

    /**
     * @brief Generate connection ID
     */
    QString generateConnectionId(const PacketData& packet) const;

    /**
     * @brief Update connection tracking
     */
    void updateConnectionTracking(const PacketData& packet);

    /**
     * @brief Cleanup inactive connections
     */
    void cleanupInactiveConnections();
};

#endif // NETWORK_MONITOR_H

