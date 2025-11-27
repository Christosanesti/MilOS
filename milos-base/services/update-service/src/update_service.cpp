#include "update_service.h"
#include "config_parser.h"
#include "package_manager.h"
#include "signature_verifier.h"
#include "update_applier.h"
#include "rollback_manager.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <systemd/sd-daemon.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <ctime>

UpdateService::UpdateService()
    : m_running(false)
    , m_initialized(false)
{
}

UpdateService::~UpdateService() {
    stop();
}

bool UpdateService::initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        // Load configuration
        if (!loadConfiguration()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize audit logger first (for logging initialization events)
        if (!initializeAuditLogger()) {
            // Continue with graceful degradation
        }

        // Initialize package manager
        if (!initializePackageManager()) {
            // Continue with graceful degradation
        }

        // Initialize signature verifier
        if (!initializeSignatureVerifier()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize rollback manager
        if (!initializeRollbackManager()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize update applier
        if (!initializeUpdateApplier()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize D-Bus interface
        if (!initializeDBusInterface()) {
            // Logging handled by audit logger
            return false;
        }

        m_initialized = true;
        return true;
    } catch (const std::exception& e) {
        // Logging handled by audit logger
        return false;
    }
}

bool UpdateService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    try {
        // Start D-Bus interface
        if (!m_dbusInterface->start()) {
            // Logging handled by audit logger
            return false;
        }

        m_running = true;
        
        // Notify systemd that service is ready
        notifySystemdReady();
        
        // Logging handled by audit logger
        return true;
    } catch (const std::exception& e) {
        // Logging handled by audit logger
        return false;
    }
}

void UpdateService::stop() {
    if (!m_running) {
        return;
    }

    try {
        if (m_dbusInterface) {
            m_dbusInterface->stop();
        }

        m_running = false;
        // Logging handled by audit logger
    } catch (const std::exception& e) {
        // Logging handled by audit logger
    }
}

bool UpdateService::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check if all components are healthy
    if (m_dbusInterface && !m_dbusInterface->isHealthy()) {
        return false;
    }

    // Check if configuration is loaded
    if (!m_configParser || !m_configParser->isLoaded()) {
        return false;
    }

    // Package manager may be unavailable (graceful degradation)
    // Signature verifier should be available
    if (!m_signatureVerifier) {
        return false;
    }

    return true;
}

std::string UpdateService::getHealthStatus() const {
    std::ostringstream json;
    json << "{";
    json << "\"service_running\":" << (m_running ? "true" : "false") << ",";
    json << "\"service_initialized\":" << (m_initialized ? "true" : "false") << ",";
    json << "\"overall_health\":" << (isHealthy() ? "\"healthy\"" : "\"unhealthy\"") << ",";
    json << "\"components\":{";
    
    // Package manager health
    if (m_packageManager) {
        json << "\"package_manager\":{";
        json << "\"initialized\":true,";
        json << "\"available\":" << (m_packageManager->isAvailable() ? "true" : "false");
        json << "},";
    } else {
        json << "\"package_manager\":{\"initialized\":false,\"available\":false},";
    }
    
    // Signature verifier health
    if (m_signatureVerifier) {
        json << "\"signature_verifier\":{";
        json << "\"initialized\":true,";
        json << "\"enabled\":" << (m_signatureVerifier->isEnabled() ? "true" : "false");
        json << "},";
    } else {
        json << "\"signature_verifier\":{\"initialized\":false,\"enabled\":false},";
    }
    
    // D-Bus interface health
    if (m_dbusInterface) {
        json << "\"dbus_interface\":{";
        json << "\"initialized\":" << (m_dbusInterface->isRunning() ? "true" : "false") << ",";
        json << "\"healthy\":" << (m_dbusInterface->isHealthy() ? "true" : "false");
        json << "},";
    } else {
        json << "\"dbus_interface\":{\"initialized\":false,\"healthy\":false},";
    }
    
    // Configuration health
    if (m_configParser) {
        json << "\"configuration\":{";
        json << "\"loaded\":" << (m_configParser->isLoaded() ? "true" : "false");
        json << "}";
    } else {
        json << "\"configuration\":{\"loaded\":false}";
    }
    
    json << "},";
    json << "\"timestamp\":\"" << std::time(nullptr) << "\"";
    json << "}";
    
    return json.str();
}

bool UpdateService::reloadConfiguration() {
    if (!loadConfiguration()) {
        return false;
    }

    // Reload components that depend on configuration
    // Note: Some components may need restart to apply new configuration
    return true;
}

void UpdateService::performHealthCheck() {
    // Perform health check
    bool healthy = isHealthy();
    
    // Update systemd watchdog
    updateWatchdog();
    
    // Log health status if unhealthy
    if (!healthy) {
        // Logging handled by audit logger
    }
}

void UpdateService::updateWatchdog() {
    // Notify systemd watchdog that service is alive
    sd_notify(0, "WATCHDOG=1");
}

bool UpdateService::loadConfiguration() {
    m_configParser = std::make_unique<ConfigParser>();
    return m_configParser->load("/etc/milos/update-service/config.yaml");
}

bool UpdateService::initializePackageManager() {
    m_packageManager = std::make_unique<PackageManager>();
    return m_packageManager->initialize(m_configParser.get());
}

bool UpdateService::initializeSignatureVerifier() {
    m_signatureVerifier = std::make_unique<SignatureVerifier>();
    return m_signatureVerifier->initialize(m_configParser.get());
}

bool UpdateService::initializeRollbackManager() {
    m_rollbackManager = std::make_unique<RollbackManager>();
    return m_rollbackManager->initialize(m_configParser.get(), m_packageManager.get());
}

bool UpdateService::initializeUpdateApplier() {
    m_updateApplier = std::make_unique<UpdateApplier>();
    return m_updateApplier->initialize(
        m_configParser.get(),
        m_packageManager.get(),
        m_signatureVerifier.get(),
        m_rollbackManager.get(),
        m_auditLogger.get()
    );
}

bool UpdateService::initializeAuditLogger() {
    m_auditLogger = std::make_unique<AuditLogger>(nullptr);

    // Get audit service configuration from config parser
    QString auditServiceBus = QString::fromStdString(
        m_configParser->getString("audit.audit_service_bus")
    );
    if (auditServiceBus.isEmpty()) {
        auditServiceBus = "org.milos.AuditService";
    }

    QString auditServicePath = QString::fromStdString(
        m_configParser->getString("audit.audit_service_path")
    );
    if (auditServicePath.isEmpty()) {
        auditServicePath = "/org/milos/AuditService";
    }

    return m_auditLogger->initialize(auditServiceBus, auditServicePath);
}

bool UpdateService::initializeDBusInterface() {
    m_dbusInterface = std::make_unique<DBusInterface>(nullptr);
    return m_dbusInterface->initialize(
        m_configParser.get(),
        m_packageManager.get(),
        m_signatureVerifier.get(),
        m_updateApplier.get(),
        m_rollbackManager.get(),
        m_auditLogger.get()
    );
}

void UpdateService::notifySystemdReady() {
    sd_notify(0, "READY=1\nSTATUS=Update Service is running");
}

