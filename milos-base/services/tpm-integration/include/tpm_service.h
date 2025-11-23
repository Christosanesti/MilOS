#ifndef TPM_SERVICE_H
#define TPM_SERVICE_H

#include <memory>
#include <string>

class TPMDevice;
class TPMKeyManager;
class TPMAttestation;
class TPMHealthMonitor;
class SecureBootManager;
class DBusInterface;

/**
 * @brief TPM Integration Service
 * 
 * Provides TPM 2.0 integration for hardware-rooted trust,
 * secure boot, key storage, and attestation.
 */
class TPMService {
public:
    TPMService();
    ~TPMService();

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
    TPMDevice* getTPMDevice() const { return m_tpmDevice.get(); }
    TPMKeyManager* getKeyManager() const { return m_keyManager.get(); }
    TPMAttestation* getAttestation() const { return m_attestation.get(); }
    TPMHealthMonitor* getHealthMonitor() const { return m_healthMonitor.get(); }
    SecureBootManager* getSecureBootManager() const { return m_secureBootManager.get(); }

private:
    bool m_running;
    bool m_initialized;
    
    std::unique_ptr<TPMDevice> m_tpmDevice;
    std::unique_ptr<TPMKeyManager> m_keyManager;
    std::unique_ptr<TPMAttestation> m_attestation;
    std::unique_ptr<TPMHealthMonitor> m_healthMonitor;
    std::unique_ptr<SecureBootManager> m_secureBootManager;
    std::unique_ptr<DBusInterface> m_dbusInterface;
};

#endif // TPM_SERVICE_H

