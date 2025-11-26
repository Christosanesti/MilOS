#include "fim_service.h"
#include "baseline_manager.h"
#include "change_detector.h"
#include "integrity_verifier.h"
#include "file_monitor.h"
#include "remediation_manager.h"
#include "security_tools_integration.h"
#include "verification_scheduler.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include "audit_logger.h"
#include <iostream>

FIMService::FIMService()
    : m_running(false)
    , m_initialized(false)
{
}

FIMService::~FIMService() {
    stop();
}

bool FIMService::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize configuration parser
    m_configParser = std::make_unique<ConfigParser>();
    if (!m_configParser->loadConfig("/etc/milos/fim/config.yaml")) {
        std::cerr << "Warning: Failed to load configuration, using defaults" << std::endl;
    }

    // Initialize audit logger
    m_auditLogger = std::make_unique<AuditLogger>();
    if (!m_auditLogger->initialize()) {
        std::cerr << "Warning: Failed to initialize audit logger" << std::endl;
    }

    // Initialize baseline manager
    m_baselineManager = std::make_unique<BaselineManager>();
    if (!m_baselineManager->initialize()) {
        std::cerr << "Failed to initialize baseline manager" << std::endl;
        return false;
    }

    // Initialize change detector
    m_changeDetector = std::make_unique<ChangeDetector>();
    if (!m_changeDetector->initialize(m_baselineManager.get())) {
        std::cerr << "Failed to initialize change detector" << std::endl;
        return false;
    }

    // Initialize integrity verifier
    m_integrityVerifier = std::make_unique<IntegrityVerifier>();
    if (!m_integrityVerifier->initialize(m_baselineManager.get())) {
        std::cerr << "Failed to initialize integrity verifier" << std::endl;
        return false;
    }

    // Initialize file monitor
    m_fileMonitor = std::make_unique<FileMonitor>();
    if (!m_fileMonitor->initialize(m_changeDetector.get())) {
        std::cerr << "Failed to initialize file monitor" << std::endl;
        return false;
    }
    
    // Load monitored directories from configuration
    if (m_configParser && m_configParser->isLoaded()) {
        std::vector<std::string> directories = m_configParser->getStringArray("monitoring.monitored_directories");
        for (const auto& dir : directories) {
            m_fileMonitor->addDirectory(dir);
        }
    }

    // Initialize remediation manager
    m_remediationManager = std::make_unique<RemediationManager>();
    if (!m_remediationManager->initialize(m_baselineManager.get(), m_changeDetector.get(), m_auditLogger.get())) {
        std::cerr << "Warning: Failed to initialize remediation manager" << std::endl;
        // Don't fail initialization if remediation is not available
    }

    // Initialize security tools integration
    m_securityToolsIntegration = std::make_unique<SecurityToolsIntegration>();
    if (!m_securityToolsIntegration->initialize(m_changeDetector.get(), m_integrityVerifier.get(), m_auditLogger.get())) {
        std::cerr << "Warning: Failed to initialize security tools integration" << std::endl;
        // Don't fail initialization if security tools integration is not available
    } else {
        // Register callback to forward violations to security tools
        m_changeDetector->registerChangeCallback([this](const FileChange& change) {
            if (m_securityToolsIntegration) {
                std::string changeTypeStr;
                switch (change.change_type) {
                    case ChangeType::MODIFIED: changeTypeStr = "modified"; break;
                    case ChangeType::DELETED: changeTypeStr = "deleted"; break;
                    case ChangeType::CREATED: changeTypeStr = "created"; break;
                    case ChangeType::PERMISSIONS_CHANGED: changeTypeStr = "permissions_changed"; break;
                    case ChangeType::OWNERSHIP_CHANGED: changeTypeStr = "ownership_changed"; break;
                }
                m_securityToolsIntegration->forwardViolation(
                    change.change_id,
                    change.file_path,
                    changeTypeStr,
                    change.severity
                );
            }
        });
    }

    // Initialize verification scheduler
    m_verificationScheduler = std::make_unique<VerificationScheduler>();
    if (!m_verificationScheduler->initialize(m_integrityVerifier.get(), m_baselineManager.get(), m_auditLogger.get())) {
        std::cerr << "Warning: Failed to initialize verification scheduler" << std::endl;
        // Don't fail initialization if scheduler is not available
    }

    // Initialize D-Bus interface
    m_dbusInterface = std::make_unique<DBusInterface>();
    m_dbusInterface->setFIMService(this);
    m_dbusInterface->setBaselineManager(m_baselineManager.get());
    m_dbusInterface->setChangeDetector(m_changeDetector.get());
    m_dbusInterface->setIntegrityVerifier(m_integrityVerifier.get());
    m_dbusInterface->setRemediationManager(m_remediationManager.get());
    m_dbusInterface->setVerificationScheduler(m_verificationScheduler.get());
    if (!m_dbusInterface->initialize()) {
        std::cerr << "Warning: Failed to initialize D-Bus interface" << std::endl;
    }

    m_initialized = true;
    return true;
}

bool FIMService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start file monitoring
    if (!m_fileMonitor->start()) {
        std::cerr << "Failed to start file monitor" << std::endl;
        return false;
    }

    // Start verification scheduler
    if (m_verificationScheduler) {
        m_verificationScheduler->start();
    }

    m_running = true;
    
    if (m_auditLogger) {
        m_auditLogger->logEvent("fim_service_started", "File Integrity Monitoring Service started");
    }

    return true;
}

void FIMService::stop() {
    if (!m_running) {
        return;
    }

    if (m_fileMonitor) {
        m_fileMonitor->stop();
    }

    if (m_verificationScheduler) {
        m_verificationScheduler->stop();
    }

    if (m_auditLogger) {
        m_auditLogger->logEvent("fim_service_stopped", "File Integrity Monitoring Service stopped");
    }

    m_running = false;
}

bool FIMService::isHealthy() const {
    if (!m_initialized) {
        return false;
    }

    if (!m_baselineManager || !m_changeDetector || !m_integrityVerifier || !m_fileMonitor) {
        return false;
    }

    return true;
}

bool FIMService::reloadConfiguration() {
    if (!m_configParser) {
        return false;
    }

    if (!m_configParser->loadConfig("/etc/milos/fim/config.yaml")) {
        return false;
    }

    // Reload components with new configuration
    if (m_fileMonitor) {
        // Get monitored directories from config
        std::vector<std::string> directories = m_configParser->getStringArray("monitoring.monitored_directories");
        m_fileMonitor->reloadConfiguration(directories);
    }

    if (m_auditLogger) {
        m_auditLogger->logEvent("fim_config_reloaded", "Configuration reloaded");
    }

    return true;
}

void FIMService::performHealthCheck() {
    // Perform health checks on all components
    if (!isHealthy()) {
        std::cerr << "Health check failed" << std::endl;
    }
}

