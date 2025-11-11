#ifndef AUDIT_SERVICE_H
#define AUDIT_SERVICE_H

#include <memory>
#include <string>

class LogStorage;
class HashChain;
class EventCollector;
class QueryEngine;
class DBusInterface;
class SocketInterface;
class ConfigParser;

/**
 * @brief Main Audit Service
 * 
 * This service provides tamper-proof audit logging, event collection,
 * and security event correlation.
 */
class AuditService {
public:
    AuditService();
    ~AuditService();

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
     * @brief Get detailed health status information
     * @return JSON string with health status details
     */
    std::string getHealthStatus() const;

    /**
     * @brief Reload configuration
     * @return true if reload successful, false otherwise
     */
    bool reloadConfiguration();

    /**
     * @brief Perform periodic health check and update systemd watchdog
     */
    void performHealthCheck();

private:
    bool m_running;
    bool m_initialized;
    
    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<LogStorage> m_logStorage;
    std::unique_ptr<HashChain> m_hashChain;
    std::unique_ptr<EventCollector> m_eventCollector;
    std::unique_ptr<QueryEngine> m_queryEngine;
    std::unique_ptr<DBusInterface> m_dbusInterface;
    std::unique_ptr<SocketInterface> m_socketInterface;

    /**
     * @brief Load configuration from file
     */
    bool loadConfiguration();

    /**
     * @brief Initialize log storage
     */
    bool initializeLogStorage();

    /**
     * @brief Initialize hash chain
     */
    bool initializeHashChain();

    /**
     * @brief Initialize event collector
     */
    bool initializeEventCollector();

    /**
     * @brief Initialize query engine
     */
    bool initializeQueryEngine();

    /**
     * @brief Initialize D-Bus interface
     */
    bool initializeDBusInterface();

    /**
     * @brief Initialize socket interface
     */
    bool initializeSocketInterface();

    /**
     * @brief Notify systemd that service is ready
     */
    void notifySystemdReady();

    /**
     * @brief Update systemd watchdog
     */
    void updateWatchdog();
};

#endif // AUDIT_SERVICE_H

