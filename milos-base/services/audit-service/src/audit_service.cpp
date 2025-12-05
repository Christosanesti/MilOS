#include "audit_service.h"
#include "log_storage.h"
#include "hash_chain.h"
#include "event_collector.h"
#include "query_engine.h"
#include "dbus_interface.h"
#include "socket_interface.h"
#include "config_parser.h"
#include <milos/logging/logger.h>
#include <systemd/sd-daemon.h>
#include <stdexcept>
#include <sstream>
#include <ctime>

AuditService::AuditService()
    : m_running(false)
    , m_initialized(false)
{
}

AuditService::~AuditService() {
    stop();
}

bool AuditService::initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        // Load configuration
        if (!loadConfiguration()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize log storage
        if (!initializeLogStorage()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize hash chain
        if (!initializeHashChain()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize query engine
        if (!initializeQueryEngine()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize event collector
        if (!initializeEventCollector()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize D-Bus interface
        if (!initializeDBusInterface()) {
            // Logging handled by audit logger
            return false;
        }

        // Initialize socket interface
        if (!initializeSocketInterface()) {
            // Continue with graceful degradation
        }

        m_initialized = true;
        return true;
    } catch (const std::exception& e) {
        // Logging handled by audit logger
        return false;
    }
}

bool AuditService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    try {
        // Start event collector
        if (m_eventCollector && !m_eventCollector->start()) {
            // Logging handled by audit logger
            return false;
        }

        // Start socket interface
        if (m_socketInterface && !m_socketInterface->start()) {
            // Continue with graceful degradation
        }

        // Start D-Bus interface
        if (!m_dbusInterface->start()) {
            // Logging handled by audit logger
            if (m_eventCollector) {
                m_eventCollector->stop();
            }
            if (m_socketInterface) {
                m_socketInterface->stop();
            }
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

void AuditService::stop() {
    if (!m_running) {
        return;
    }

    try {
        if (m_eventCollector) {
            m_eventCollector->stop();
        }

        if (m_socketInterface) {
            m_socketInterface->stop();
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

bool AuditService::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check if all components are healthy
    if (m_logStorage && !m_logStorage->isHealthy()) {
        return false;
    }

    if (m_hashChain && !m_hashChain->isHealthy()) {
        return false;
    }

    if (m_dbusInterface && !m_dbusInterface->isHealthy()) {
        return false;
    }

    // Check if configuration is loaded
    if (!m_configParser || !m_configParser->isLoaded()) {
        return false;
    }

    return true;
}

std::string AuditService::getHealthStatus() const {
    std::ostringstream json;
    json << "{";
    json << "\"service_running\":" << (m_running ? "true" : "false") << ",";
    json << "\"service_initialized\":" << (m_initialized ? "true" : "false") << ",";
    json << "\"overall_health\":" << (isHealthy() ? "\"healthy\"" : "\"unhealthy\"") << ",";
    json << "\"components\":{";
    
    // Log storage health
    if (m_logStorage) {
        json << "\"log_storage\":{";
        json << "\"initialized\":true,";
        json << "\"healthy\":" << (m_logStorage->isHealthy() ? "true" : "false");
        json << "},";
    } else {
        json << "\"log_storage\":{\"initialized\":false,\"healthy\":false},";
    }
    
    // Hash chain health
    if (m_hashChain) {
        json << "\"hash_chain\":{";
        json << "\"initialized\":true,";
        json << "\"healthy\":" << (m_hashChain->isHealthy() ? "true" : "false");
        json << "},";
    } else {
        json << "\"hash_chain\":{\"initialized\":false,\"healthy\":false},";
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

bool AuditService::reloadConfiguration() {
    if (!loadConfiguration()) {
        return false;
    }

    // Reload components that depend on configuration
    // Note: Some components may need restart to apply new configuration
    return true;
}

void AuditService::performHealthCheck() {
    // Perform health check
    bool healthy = isHealthy();
    
    // Update systemd watchdog
    updateWatchdog();
    
    // Log health status if unhealthy
    if (!healthy) {
        // Logging handled by audit logger
    }
}

void AuditService::updateWatchdog() {
    // Notify systemd watchdog that service is alive
    sd_notify(0, "WATCHDOG=1");
}

bool AuditService::loadConfiguration() {
    m_configParser = std::make_unique<ConfigParser>();
    return m_configParser->load("/etc/milos/audit-service/config.yaml");
}

bool AuditService::initializeLogStorage() {
    m_logStorage = std::make_unique<LogStorage>();
    return m_logStorage->initialize(m_configParser.get());
}

bool AuditService::initializeHashChain() {
    m_hashChain = std::make_unique<HashChain>();
    return m_hashChain->initialize(m_configParser.get(), m_logStorage.get());
}

bool AuditService::initializeEventCollector() {
    m_eventCollector = std::make_unique<EventCollector>();
    return m_eventCollector->initialize(
        m_configParser.get(),
        m_logStorage.get(),
        m_hashChain.get()
    );
}

bool AuditService::initializeQueryEngine() {
    m_queryEngine = std::make_unique<QueryEngine>();
    return m_queryEngine->initialize(m_configParser.get(), m_logStorage.get());
}

bool AuditService::initializeDBusInterface() {
    m_dbusInterface = std::make_unique<DBusInterface>(nullptr);
    return m_dbusInterface->initialize(
        m_configParser.get(),
        m_logStorage.get(),
        m_hashChain.get(),
        m_queryEngine.get(),
        m_eventCollector.get()
    );
}

bool AuditService::initializeSocketInterface() {
    m_socketInterface = std::make_unique<SocketInterface>();
    return m_socketInterface->initialize(
        m_configParser.get(),
        m_logStorage.get(),
        m_hashChain.get()
    );
}

void AuditService::notifySystemdReady() {
    sd_notify(0, "READY=1\nSTATUS=Audit Service is running");
}

