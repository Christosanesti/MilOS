#ifndef NETWORK_DASHBOARD_H
#define NETWORK_DASHBOARD_H

#include <QObject>
#include <memory>
#include <string>

// Forward declarations
class PacketCapture;
class PacketParser;
class PacketStatistics;
class PacketSanitizer;
class SocketInterface;
class DBusInterface;
class ConfigParser;
class NetworkMonitor;
class NetworkTopologyManager;

/**
 * @brief Network Dashboard
 * 
 * Main application class for Network Defense Dashboard.
 */
class NetworkDashboard : public QObject {
    Q_OBJECT

public:
    NetworkDashboard(QObject* parent = nullptr);
    ~NetworkDashboard();

    /**
     * @brief Initialize dashboard
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start dashboard
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop dashboard
     */
    void stop();

    /**
     * @brief Check if dashboard is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Get Network Monitor instance
     * @return Network Monitor instance
     */
    NetworkMonitor* getNetworkMonitor() const { return m_networkMonitor.get(); }

    /**
     * @brief Get Network Topology Manager instance
     * @return Network Topology Manager instance
     */
    NetworkTopologyManager* getNetworkTopology() const { return m_networkTopology.get(); }

private:
    bool m_initialized;
    bool m_running;

    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<PacketCapture> m_packetCapture;
    std::unique_ptr<PacketParser> m_packetParser;
    std::unique_ptr<PacketStatistics> m_packetStatistics;
    std::unique_ptr<PacketSanitizer> m_packetSanitizer;
    std::unique_ptr<SocketInterface> m_socketInterface;
    std::unique_ptr<DBusInterface> m_dbusInterface;

    /**
     * @brief Initialize components
     */
    bool initializeComponents();

    /**
     * @brief Setup packet capture callback
     */
    void setupPacketCaptureCallback();
};

#endif // NETWORK_DASHBOARD_H

