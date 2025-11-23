#include "tpm_health_monitor.h"
#include "tpm_device.h"
#include <ctime>
#include <chrono>
#include <iostream>

TPMHealthMonitor::TPMHealthMonitor()
    : m_initialized(false)
    , m_monitoring(false)
    , m_running(false)
    , m_tpmDevice(nullptr)
{
    m_healthStatus.is_healthy = false;
    m_healthStatus.device_available = false;
    m_healthStatus.pcr_valid = false;
}

TPMHealthMonitor::~TPMHealthMonitor() {
    stop();
}

bool TPMHealthMonitor::initialize(TPMDevice* tpmDevice) {
    if (m_initialized) {
        return true;
    }

    if (!tpmDevice) {
        return false;
    }

    m_tpmDevice = tpmDevice;
    m_initialized = true;
    return true;
}

bool TPMHealthMonitor::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_monitoring) {
        return true;
    }

    m_running = true;
    m_monitoring = true;

    // Perform initial health check
    performHealthCheck();

    // Start monitor thread
    m_monitorThread = std::make_unique<std::thread>(&TPMHealthMonitor::monitorLoop, this);

    return true;
}

void TPMHealthMonitor::stop() {
    if (!m_monitoring) {
        return;
    }

    m_running = false;
    m_monitoring = false;

    if (m_monitorThread && m_monitorThread->joinable()) {
        m_monitorThread->join();
    }
}

void TPMHealthMonitor::performHealthCheck() {
    if (!m_tpmDevice) {
        m_healthStatus.is_healthy = false;
        m_healthStatus.status_message = "TPM device not available";
        return;
    }

    // Check device availability
    m_healthStatus.device_available = m_tpmDevice->isAvailable();

    // Check PCR validity (read PCR 0 as test)
    if (m_healthStatus.device_available) {
        try {
            auto pcrValues = m_tpmDevice->getPCRValues({0});
            m_healthStatus.pcr_valid = !pcrValues.empty();
        } catch (...) {
            m_healthStatus.pcr_valid = false;
        }
    } else {
        m_healthStatus.pcr_valid = false;
    }

    // Overall health status
    m_healthStatus.is_healthy = m_healthStatus.device_available && m_healthStatus.pcr_valid;

    if (m_healthStatus.is_healthy) {
        m_healthStatus.status_message = "TPM device is healthy";
    } else if (!m_healthStatus.device_available) {
        m_healthStatus.status_message = "TPM device not available";
    } else if (!m_healthStatus.pcr_valid) {
        m_healthStatus.status_message = "PCR read failed";
    } else {
        m_healthStatus.status_message = "TPM health check failed";
    }

    // Format check time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    m_healthStatus.last_check = timeStr;
}

void TPMHealthMonitor::monitorLoop() {
    while (m_running) {
        performHealthCheck();
        
        // Sleep for 60 seconds between checks
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

