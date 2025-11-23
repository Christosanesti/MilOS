#ifndef SECRETS_SERVICE_H
#define SECRETS_SERVICE_H

#include <memory>
#include <string>

class SecretStorage;
class SecretRotation;
class SecretVersioning;
class AccessControl;
class ApplicationIntegration;
class DBusInterface;
class ConfigParser;
class AuditLogger;

/**
 * @brief Secrets Management Service
 * 
 * Provides centralized secrets management with encryption,
 * rotation, versioning, and access control.
 */
class SecretsService {
public:
    SecretsService();
    ~SecretsService();

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
     * @brief Perform periodic health check
     */
    void performHealthCheck();

    // Accessors for components
    SecretStorage* getSecretStorage() const { return m_secretStorage.get(); }
    SecretRotation* getSecretRotation() const { return m_secretRotation.get(); }
    SecretVersioning* getSecretVersioning() const { return m_secretVersioning.get(); }
    AccessControl* getAccessControl() const { return m_accessControl.get(); }
    ApplicationIntegration* getApplicationIntegration() const { return m_applicationIntegration.get(); }

private:
    bool m_running;
    bool m_initialized;
    
    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<SecretStorage> m_secretStorage;
    std::unique_ptr<SecretRotation> m_secretRotation;
    std::unique_ptr<SecretVersioning> m_secretVersioning;
    std::unique_ptr<AccessControl> m_accessControl;
    std::unique_ptr<ApplicationIntegration> m_applicationIntegration;
    std::unique_ptr<DBusInterface> m_dbusInterface;
    std::unique_ptr<AuditLogger> m_auditLogger;
};

#endif // SECRETS_SERVICE_H

