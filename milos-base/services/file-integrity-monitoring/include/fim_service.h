#ifndef FIM_SERVICE_H
#define FIM_SERVICE_H

#include <memory>
#include <string>
#include <vector>

class BaselineManager;
class ChangeDetector;
class IntegrityVerifier;
class FileMonitor;
class RemediationManager;
class SecurityToolsIntegration;
class DBusInterface;
class ConfigParser;
class AuditLogger;

/**
 * @brief File Integrity Monitoring Service
 * 
 * Provides real-time file integrity monitoring, baseline management,
 * change detection, and integrity verification.
 */
class FIMService {
public:
    FIMService();
    ~FIMService();

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

    /**
     * @brief Perform periodic health check
     */
    void performHealthCheck();

private:
    bool m_running;
    bool m_initialized;
    
    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<BaselineManager> m_baselineManager;
    std::unique_ptr<ChangeDetector> m_changeDetector;
    std::unique_ptr<IntegrityVerifier> m_integrityVerifier;
    std::unique_ptr<FileMonitor> m_fileMonitor;
    std::unique_ptr<RemediationManager> m_remediationManager;
    std::unique_ptr<SecurityToolsIntegration> m_securityToolsIntegration;
    std::unique_ptr<DBusInterface> m_dbusInterface;
    std::unique_ptr<AuditLogger> m_auditLogger;
};

#endif // FIM_SERVICE_H

