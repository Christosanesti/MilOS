#ifndef TPM_HEALTH_MONITOR_H
#define TPM_HEALTH_MONITOR_H

#include <string>
#include <memory>
#include <thread>
#include <atomic>

class TPMDevice;

/**
 * @brief TPM Health Status
 */
struct TPMHealthStatus {
    bool is_healthy;
    bool device_available;
    bool pcr_valid;
    std::string status_message;
    std::string last_check;
};

/**
 * @brief TPM Health Monitor
 * 
 * Monitors TPM device health and status.
 */
class TPMHealthMonitor {
public:
    TPMHealthMonitor();
    ~TPMHealthMonitor();

    /**
     * @brief Initialize health monitor
     * @param tpmDevice TPM device instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(TPMDevice* tpmDevice);

    /**
     * @brief Start health monitoring
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop health monitoring
     */
    void stop();

    /**
     * @brief Perform health check
     */
    void performHealthCheck();

    /**
     * @brief Get current health status
     * @return Health status
     */
    TPMHealthStatus getHealthStatus() const { return m_healthStatus; }

    /**
     * @brief Check if monitor is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const { return m_healthStatus.is_healthy; }

private:
    bool m_initialized;
    bool m_monitoring;
    std::atomic<bool> m_running;
    TPMDevice* m_tpmDevice;
    TPMHealthStatus m_healthStatus;
    std::unique_ptr<std::thread> m_monitorThread;

    /**
     * @brief Monitor thread function
     */
    void monitorLoop();
};

#endif // TPM_HEALTH_MONITOR_H

