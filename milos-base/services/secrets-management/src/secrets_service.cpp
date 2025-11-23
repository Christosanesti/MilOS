#include "secrets_service.h"
#include "secret_storage.h"
#include "secret_rotation.h"
#include "secret_versioning.h"
#include "access_control.h"
#include "application_integration.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include "audit_logger.h"
#include <iostream>

SecretsService::SecretsService()
    : m_running(false)
    , m_initialized(false)
{
}

SecretsService::~SecretsService() {
    stop();
}

bool SecretsService::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize configuration parser
    m_configParser = std::make_unique<ConfigParser>();
    if (!m_configParser->loadConfig("/etc/milos/secrets/config.yaml")) {
        std::cerr << "Warning: Failed to load configuration, using defaults" << std::endl;
    }

    // Initialize audit logger
    m_auditLogger = std::make_unique<AuditLogger>();
    if (!m_auditLogger->initialize()) {
        std::cerr << "Warning: Failed to initialize audit logger" << std::endl;
    }

    // Initialize secret storage
    m_secretStorage = std::make_unique<SecretStorage>();
    if (!m_secretStorage->initialize()) {
        std::cerr << "Failed to initialize secret storage" << std::endl;
        return false;
    }

    // Initialize secret versioning
    m_secretVersioning = std::make_unique<SecretVersioning>();
    if (!m_secretVersioning->initialize(m_secretStorage.get())) {
        std::cerr << "Failed to initialize secret versioning" << std::endl;
        return false;
    }

    // Initialize access control
    m_accessControl = std::make_unique<AccessControl>();
    if (!m_accessControl->initialize()) {
        std::cerr << "Failed to initialize access control" << std::endl;
        return false;
    }

    // Initialize secret rotation
    m_secretRotation = std::make_unique<SecretRotation>();
    if (!m_secretRotation->initialize(m_secretStorage.get(), m_secretVersioning.get())) {
        std::cerr << "Failed to initialize secret rotation" << std::endl;
        return false;
    }

    // Initialize application integration
    m_applicationIntegration = std::make_unique<ApplicationIntegration>();
    if (!m_applicationIntegration->initialize(m_secretStorage.get(), m_accessControl.get(), m_auditLogger.get())) {
        std::cerr << "Warning: Failed to initialize application integration" << std::endl;
        // Don't fail initialization if application integration is not available
    }

    // Initialize D-Bus interface
    m_dbusInterface = std::make_unique<DBusInterface>();
    m_dbusInterface->setSecretsService(this);
    m_dbusInterface->setSecretStorage(m_secretStorage.get());
    m_dbusInterface->setSecretRotation(m_secretRotation.get());
    m_dbusInterface->setSecretVersioning(m_secretVersioning.get());
    m_dbusInterface->setAccessControl(m_accessControl.get());
    m_dbusInterface->setApplicationIntegration(m_applicationIntegration.get());
    if (!m_dbusInterface->initialize()) {
        std::cerr << "Warning: Failed to initialize D-Bus interface" << std::endl;
    }

    m_initialized = true;
    return true;
}

bool SecretsService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start rotation scheduler
    if (!m_secretRotation->start()) {
        std::cerr << "Failed to start secret rotation" << std::endl;
        return false;
    }

    m_running = true;
    
    if (m_auditLogger) {
        m_auditLogger->logEvent("secrets_service_started", "Secrets Management Service started");
    }

    return true;
}

void SecretsService::stop() {
    if (!m_running) {
        return;
    }

    if (m_secretRotation) {
        m_secretRotation->stop();
    }

    if (m_auditLogger) {
        m_auditLogger->logEvent("secrets_service_stopped", "Secrets Management Service stopped");
    }

    m_running = false;
}

bool SecretsService::isHealthy() const {
    if (!m_initialized) {
        return false;
    }

    if (!m_secretStorage || !m_secretRotation || !m_secretVersioning || !m_accessControl) {
        return false;
    }

    return true;
}

void SecretsService::performHealthCheck() {
    // Perform health checks on all components
    if (!isHealthy()) {
        std::cerr << "Health check failed" << std::endl;
    }
}

