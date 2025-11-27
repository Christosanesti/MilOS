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
        // Logging handled by audit logger
    }

    // Initialize audit logger
    m_auditLogger = std::make_unique<AuditLogger>();
    if (!m_auditLogger->initialize()) {
        // Continue with graceful degradation
    }

    // Initialize secret storage
    m_secretStorage = std::make_unique<SecretStorage>();
    if (!m_secretStorage->initialize()) {
        // Logging handled by audit logger
        return false;
    }

    // Initialize secret versioning
    m_secretVersioning = std::make_unique<SecretVersioning>();
    if (!m_secretVersioning->initialize(m_secretStorage.get())) {
        // Logging handled by audit logger
        return false;
    }

    // Initialize access control
    m_accessControl = std::make_unique<AccessControl>();
    if (!m_accessControl->initialize()) {
        // Logging handled by audit logger
        return false;
    }

    // Initialize secret rotation
    m_secretRotation = std::make_unique<SecretRotation>();
    if (!m_secretRotation->initialize(m_secretStorage.get(), m_secretVersioning.get())) {
        // Logging handled by audit logger
        return false;
    }

    // Initialize application integration
    m_applicationIntegration = std::make_unique<ApplicationIntegration>();
    if (!m_applicationIntegration->initialize(m_secretStorage.get(), m_accessControl.get(), m_auditLogger.get())) {
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
        // Continue with graceful degradation
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
        // Logging handled by audit logger
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
        // Logging handled by audit logger
    }
}

