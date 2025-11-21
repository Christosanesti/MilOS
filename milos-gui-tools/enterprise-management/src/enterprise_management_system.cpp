#include "enterprise_management_system.h"
#include <QDebug>

EnterpriseManagementSystem::EnterpriseManagementSystem(QObject* parent)
    : QObject(parent)
    , m_configManager(new ConfigManager(this))
    , m_deploymentOrchestrator(new DeploymentOrchestrator(this))
    , m_multiSiteManager(new MultiSiteManager(this))
    , m_dbusInterface(new EnterpriseManagementDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

EnterpriseManagementSystem::~EnterpriseManagementSystem() {
}

bool EnterpriseManagementSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize config manager
    if (!m_configManager->initialize()) {
        qWarning() << "Failed to initialize config manager";
        return false;
    }
    
    // Initialize deployment orchestrator
    if (!m_deploymentOrchestrator->initialize()) {
        qWarning() << "Failed to initialize deployment orchestrator";
        return false;
    }
    
    // Initialize multi-site manager
    if (!m_multiSiteManager->initialize()) {
        qWarning() << "Failed to initialize multi-site manager";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_configManager, &ConfigManager::configChanged, this, [this](const QString& key, const QVariant& value) {
        QVariant oldValue = m_configManager->getConfig(key);
        m_auditLogger->logConfigChange(key, oldValue.toString(), value.toString());
    });
    
    connect(m_deploymentOrchestrator, &DeploymentOrchestrator::deploymentStatusChanged, this, [this](const QString& deploymentId, DeploymentStatus status) {
        if (status == DeploymentStatus::InProgress) {
            QVariantMap deployment = m_deploymentOrchestrator->getDeploymentStatus(deploymentId);
            m_auditLogger->logDeploymentExecuted(deploymentId);
        }
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setSystem(this);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

