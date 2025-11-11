#ifndef DATAGUARD_SERVICE_H
#define DATAGUARD_SERVICE_H

#include <memory>
#include <string>
#include <vector>

class NetworkEnforcement;
class PolicyManager;
class DBusInterface;
class ConfigParser;

/**
 * @brief Main Data Transmission Guard Service
 * 
 * This service enforces encryption at the network layer, monitors
 * unencrypted data attempts, and blocks unencrypted transmissions.
 */
class DataGuardService {
public:
    DataGuardService();
    ~DataGuardService();

    /**
     * @brief Initialize the service
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start the service
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop the service
     */
    void stop();

    /**
     * @brief Check if service is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Get service health status
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

    /**
     * @brief Reload configuration
     * @return true if reload successful, false otherwise
     */
    bool reloadConfiguration();

private:
    bool m_running;
    bool m_initialized;
    
    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<PolicyManager> m_policyManager;
    std::unique_ptr<NetworkEnforcement> m_networkEnforcement;
    std::unique_ptr<DBusInterface> m_dbusInterface;

    /**
     * @brief Load configuration from file
     */
    bool loadConfiguration();

    /**
     * @brief Initialize network enforcement
     */
    bool initializeNetworkEnforcement();

    /**
     * @brief Initialize D-Bus interface
     */
    bool initializeDBusInterface();

    /**
     * @brief Notify systemd that service is ready
     */
    void notifySystemdReady();
};

#endif // DATAGUARD_SERVICE_H

