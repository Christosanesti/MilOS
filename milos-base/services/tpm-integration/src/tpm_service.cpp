#include "tpm_service.h"
#include "tpm_device.h"
#include "tpm_key_manager.h"
#include "tpm_attestation.h"
#include "tpm_health_monitor.h"
#include "secure_boot_manager.h"
#include "dbus_interface.h"
#include <iostream>

TPMService::TPMService()
    : m_running(false)
    , m_initialized(false)
{
}

TPMService::~TPMService() {
    stop();
}

bool TPMService::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize TPM device
    m_tpmDevice = std::make_unique<TPMDevice>();
    if (!m_tpmDevice->initialize()) {
        std::cerr << "Failed to initialize TPM device" << std::endl;
        return false;
    }

    // Initialize key manager
    m_keyManager = std::make_unique<TPMKeyManager>();
    if (!m_keyManager->initialize(m_tpmDevice.get())) {
        std::cerr << "Failed to initialize TPM key manager" << std::endl;
        return false;
    }

    // Initialize attestation
    m_attestation = std::make_unique<TPMAttestation>();
    if (!m_attestation->initialize(m_tpmDevice.get())) {
        std::cerr << "Failed to initialize TPM attestation" << std::endl;
        return false;
    }

    // Initialize health monitor
    m_healthMonitor = std::make_unique<TPMHealthMonitor>();
    if (!m_healthMonitor->initialize(m_tpmDevice.get())) {
        std::cerr << "Failed to initialize TPM health monitor" << std::endl;
        return false;
    }

    // Initialize secure boot manager
    m_secureBootManager = std::make_unique<SecureBootManager>();
    if (!m_secureBootManager->initialize(m_tpmDevice.get(), m_attestation.get())) {
        std::cerr << "Warning: Failed to initialize secure boot manager" << std::endl;
        // Don't fail initialization if secure boot is not available
    }

    // Initialize D-Bus interface
    m_dbusInterface = std::make_unique<DBusInterface>();
    m_dbusInterface->setTPMService(this);
    m_dbusInterface->setTPMDevice(m_tpmDevice.get());
    m_dbusInterface->setKeyManager(m_keyManager.get());
    m_dbusInterface->setAttestation(m_attestation.get());
    m_dbusInterface->setSecureBootManager(m_secureBootManager.get());
    if (!m_dbusInterface->initialize()) {
        std::cerr << "Warning: Failed to initialize D-Bus interface" << std::endl;
    }

    m_initialized = true;
    return true;
}

bool TPMService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start health monitoring
    if (!m_healthMonitor->start()) {
        std::cerr << "Failed to start health monitor" << std::endl;
        return false;
    }

    m_running = true;
    return true;
}

void TPMService::stop() {
    if (!m_running) {
        return;
    }

    if (m_healthMonitor) {
        m_healthMonitor->stop();
    }

    m_running = false;
}

bool TPMService::isHealthy() const {
    if (!m_initialized) {
        return false;
    }

    if (!m_tpmDevice || !m_keyManager || !m_attestation || !m_healthMonitor) {
        return false;
    }

    return m_tpmDevice->isAvailable() && m_healthMonitor->isHealthy();
}

void TPMService::performHealthCheck() {
    if (m_healthMonitor) {
        m_healthMonitor->performHealthCheck();
    }
}

