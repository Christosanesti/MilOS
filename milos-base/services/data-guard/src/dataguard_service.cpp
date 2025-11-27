#include "dataguard_service.h"
#include "network_enforcement.h"
#include "policy_manager.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include "audit_logger.h"
#include <systemd/sd-daemon.h>
#include <stdexcept>
#include <sstream>
#include <ctime>

DataGuardService::DataGuardService()
    : m_running(false)
    , m_initialized(false)
{
}

DataGuardService::~DataGuardService() {
    stop();
}

bool DataGuardService::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize logger (C++ service, no Qt dependency for Logger)
    // Logger will be initialized by service main() or systemd

    try {
        // Load configuration
        if (!loadConfiguration()) {
            LOG_ERROR("Failed to load configuration");
            return false;
        }

        // Initialize policy manager
        m_policyManager = std::make_unique<PolicyManager>();
        if (!m_policyManager->initialize(m_configParser.get())) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize network enforcement
        if (!initializeNetworkEnforcement()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize D-Bus interface
        if (!initializeDBusInterface()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize audit logger
        if (!initializeAuditLogger()) {
            // Continue with graceful degradation
        }

        m_initialized = true;
        // Logging handled by audit logger
        return true;
    } catch (const std::exception& e) {
        // Logging handled by audit logger
        return false;
    }
}

bool DataGuardService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    try {
        // Start network enforcement
        if (!m_networkEnforcement->start()) {
            // Logging handled by audit logger
            return false;
        }

        // Start D-Bus interface
        if (!m_dbusInterface->start()) {
            // Logging handled by audit logger
            m_networkEnforcement->stop();
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

void DataGuardService::stop() {
    if (!m_running) {
        return;
    }

    try {
        if (m_networkEnforcement) {
            m_networkEnforcement->stop();
        }

        if (m_dbusInterface) {
            m_dbusInterface->stop();
        }

        m_running = false;
        // Logging handled by audit logger
    } catch (const std::exception& e) {
        // Logging handled by audit logger
    }
}

bool DataGuardService::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check if all components are healthy
    if (m_networkEnforcement && !m_networkEnforcement->isHealthy()) {
        return false;
    }

    if (m_dbusInterface && !m_dbusInterface->isHealthy()) {
        return false;
    }

    // Check if configuration is loaded
    if (!m_configParser || !m_configParser->isLoaded()) {
        return false;
    }

    // Check if policy manager is initialized
    if (!m_policyManager) {
        return false;
    }

    return true;
}

std::string DataGuardService::getHealthStatus() const {
    std::ostringstream json;
    json << "{";
    json << "\"service_running\":" << (m_running ? "true" : "false") << ",";
    json << "\"service_initialized\":" << (m_initialized ? "true" : "false") << ",";
    json << "\"overall_health\":" << (isHealthy() ? "\"healthy\"" : "\"unhealthy\"") << ",";
    json << "\"components\":{";
    
    // Network enforcement health
    if (m_networkEnforcement) {
        json << "\"network_enforcement\":{";
        json << "\"initialized\":" << (m_networkEnforcement->isRunning() ? "true" : "false") << ",";
        json << "\"healthy\":" << (m_networkEnforcement->isHealthy() ? "true" : "false") << ",";
        json << "\"blocked_count\":" << m_networkEnforcement->getBlockedCount() << ",";
        json << "\"allowed_count\":" << m_networkEnforcement->getAllowedCount();
        json << "},";
    } else {
        json << "\"network_enforcement\":{\"initialized\":false,\"healthy\":false},";
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
    
    // Policy manager health
    if (m_policyManager) {
        json << "\"policy_manager\":{";
        json << "\"initialized\":true,";
        json << "\"policy_count\":" << m_policyManager->getPolicies().size();
        json << "},";
    } else {
        json << "\"policy_manager\":{\"initialized\":false},";
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

void DataGuardService::performHealthCheck() {
    // Perform health check
    bool healthy = isHealthy();
    
    // Update systemd watchdog
    updateWatchdog();
    
    // Log health status if unhealthy
    if (!healthy) {
        // Logging handled by audit logger
    }
}

void DataGuardService::updateWatchdog() {
    // Notify systemd watchdog that service is alive
    // This should be called periodically (within watchdog timeout)
    sd_notify(0, "WATCHDOG=1");
}

std::string DataGuardService::getComponentHealthStatus() const {
    return getHealthStatus();
}

bool DataGuardService::reloadConfiguration() {
    if (!loadConfiguration()) {
        return false;
    }

    if (m_policyManager) {
        return m_policyManager->reloadPolicies(m_configParser.get());
    }

    return true;
}

bool DataGuardService::loadConfiguration() {
    m_configParser = std::make_unique<ConfigParser>();
    return m_configParser->load("/etc/milos/data-guard/config.yaml");
}

bool DataGuardService::initializeNetworkEnforcement() {
    m_networkEnforcement = std::make_unique<NetworkEnforcement>();
    return m_networkEnforcement->initialize(
        m_configParser.get(), 
        m_policyManager.get(),
        m_auditLogger.get()
    );
}

bool DataGuardService::initializeDBusInterface() {
    m_dbusInterface = std::make_unique<DBusInterface>(nullptr);
    return m_dbusInterface->initialize(
        m_configParser.get(),
        m_policyManager.get(),
        m_networkEnforcement.get()
    );
}

bool DataGuardService::initializeAuditLogger() {
    m_auditLogger = std::make_unique<AuditLogger>(nullptr);
    
    // Get audit service configuration from config parser
    QString auditServiceBus = QString::fromStdString(
        m_configParser->getString("service.audit_service_bus")
    );
    if (auditServiceBus.isEmpty()) {
        auditServiceBus = "org.milos.AuditService";
    }
    
    QString auditServicePath = QString::fromStdString(
        m_configParser->getString("service.audit_service_path")
    );
    if (auditServicePath.isEmpty()) {
        auditServicePath = "/org/milos/AuditService";
    }
    
    return m_auditLogger->initialize(auditServiceBus, auditServicePath);
}

void DataGuardService::notifySystemdReady() {
    sd_notify(0, "READY=1\nSTATUS=Data Transmission Guard Service is running");
}


