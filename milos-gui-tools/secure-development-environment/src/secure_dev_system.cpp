#include "secure_dev_system.h"
#include <QDebug>

SecureDevSystem::SecureDevSystem(QObject* parent)
    : QObject(parent)
    , m_codeAnalyzer(new CodeAnalyzer(this))
    , m_supplyChainManager(new SupplyChainManager(this))
    , m_dbusInterface(new SecureDevDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

SecureDevSystem::~SecureDevSystem() {
}

bool SecureDevSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize code analyzer
    if (!m_codeAnalyzer->initialize()) {
        qWarning() << "Failed to initialize code analyzer";
        return false;
    }
    
    // Initialize supply chain manager
    if (!m_supplyChainManager->initialize()) {
        qWarning() << "Failed to initialize supply chain manager";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_codeAnalyzer, &CodeAnalyzer::securityIssueFound, this, [this](const SecurityIssue& issue) {
        m_auditLogger->logSecurityIssue(issue.id, issue.severity);
    });
    
    connect(m_supplyChainManager, &SupplyChainManager::vulnerabilityFound, this, [this](const Dependency& dep) {
        m_auditLogger->logVulnerabilityFound(dep.name, dep.version);
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





