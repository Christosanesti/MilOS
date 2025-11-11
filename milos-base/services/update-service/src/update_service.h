#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include <memory>
#include <string>

class ConfigParser;
class PackageManager;
class SignatureVerifier;
class UpdateApplier;
class RollbackManager;
class DBusInterface;
class AuditLogger;

/**
 * @brief Main Update Service
 * 
 * This service manages security updates with signed package verification,
 * rollback capability, and support for online/offline updates.
 */
class UpdateService {
public:
    UpdateService();
    ~UpdateService();

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
    std::unique_ptr<PackageManager> m_packageManager;
    std::unique_ptr<SignatureVerifier> m_signatureVerifier;
    std::unique_ptr<UpdateApplier> m_updateApplier;
    std::unique_ptr<RollbackManager> m_rollbackManager;
    std::unique_ptr<DBusInterface> m_dbusInterface;
    std::unique_ptr<AuditLogger> m_auditLogger;

    /**
     * @brief Load configuration from file
     */
    bool loadConfiguration();

    /**
     * @brief Initialize package manager
     */
    bool initializePackageManager();

    /**
     * @brief Initialize signature verifier
     */
    bool initializeSignatureVerifier();

    /**
     * @brief Initialize rollback manager
     */
    bool initializeRollbackManager();

    /**
     * @brief Initialize update applier
     */
    bool initializeUpdateApplier();

    /**
     * @brief Initialize audit logger
     */
    bool initializeAuditLogger();

    /**
     * @brief Initialize D-Bus interface
     */
    bool initializeDBusInterface();

    /**
     * @brief Notify systemd that service is ready
     */
    void notifySystemdReady();

    /**
     * @brief Update systemd watchdog
     */
    void updateWatchdog();
};

#endif // UPDATE_SERVICE_H

