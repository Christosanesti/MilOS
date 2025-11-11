#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <memory>
#include <string>
#include <vector>

class ConfigParser;
class PolicyManager;
class NetworkEnforcement;

/**
 * @brief D-Bus Interface for Data Transmission Guard Service
 * 
 * Implements org.milos.DataGuard D-Bus interface with methods
 * and signals for service management and monitoring.
 */
class DBusInterface {
public:
    DBusInterface();
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param configParser Configuration parser instance
     * @param policyManager Policy manager instance
     * @param networkEnforcement Network enforcement instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        PolicyManager* policyManager,
        NetworkEnforcement* networkEnforcement
    );

    /**
     * @brief Start D-Bus interface
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop D-Bus interface
     */
    void stop();

    /**
     * @brief Check if D-Bus interface is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Check if D-Bus interface is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

    // D-Bus methods (to be called via D-Bus)
    std::string getTransmissionStatus();
    std::vector<std::string> getBlockedTransmissions();
    bool configurePolicy(const std::string& policy);
    std::string getPolicyStatus(const std::string& policyId);

private:
    bool m_running;
    bool m_initialized;
    
    ConfigParser* m_configParser;
    PolicyManager* m_policyManager;
    NetworkEnforcement* m_networkEnforcement;

    // D-Bus connection (opaque pointer)
    void* m_dbusConnection;

    /**
     * @brief Register D-Bus interface
     * @return true if registration successful, false otherwise
     */
    bool registerInterface();

    /**
     * @brief Unregister D-Bus interface
     */
    void unregisterInterface();
};

#endif // DBUS_INTERFACE_H

