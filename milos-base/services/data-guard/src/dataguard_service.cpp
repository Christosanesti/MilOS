#include "dataguard_service.h"
#include "network_enforcement.h"
#include "policy_manager.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include "audit_logger.h"
#include <systemd/sd-daemon.h>
#include <iostream>
#include <stdexcept>

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

    try {
        // Load configuration
        if (!loadConfiguration()) {
            std::cerr << "Failed to load configuration" << std::endl;
            return false;
        }

        // Initialize policy manager
        m_policyManager = std::make_unique<PolicyManager>();
        if (!m_policyManager->initialize(m_configParser.get())) {
            std::cerr << "Failed to initialize policy manager" << std::endl;
            return false;
        }

        // Initialize network enforcement
        if (!initializeNetworkEnforcement()) {
            std::cerr << "Failed to initialize network enforcement" << std::endl;
            return false;
        }

        // Initialize D-Bus interface
        if (!initializeDBusInterface()) {
            std::cerr << "Failed to initialize D-Bus interface" << std::endl;
            return false;
        }

        // Initialize audit logger
        if (!initializeAuditLogger()) {
            std::cerr << "Failed to initialize audit logger (continuing with graceful degradation)" << std::endl;
            // Continue with graceful degradation
        }

        m_initialized = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
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
            std::cerr << "Failed to start network enforcement" << std::endl;
            return false;
        }

        // Start D-Bus interface
        if (!m_dbusInterface->start()) {
            std::cerr << "Failed to start D-Bus interface" << std::endl;
            m_networkEnforcement->stop();
            return false;
        }

        m_running = true;
        
        // Notify systemd that service is ready
        notifySystemdReady();
        
        std::cout << "Data Transmission Guard Service started successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception during start: " << e.what() << std::endl;
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
        std::cout << "Data Transmission Guard Service stopped" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception during stop: " << e.what() << std::endl;
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

    return true;
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

