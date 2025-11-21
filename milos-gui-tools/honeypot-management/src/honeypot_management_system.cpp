#include "honeypot_management_system.h"
#include <QDebug>

HoneypotManagementSystem::HoneypotManagementSystem(QObject* parent)
    : QObject(parent)
    , m_honeypotManager(new HoneypotManager(this))
    , m_threatAnalyzer(new ThreatAnalyzer(this))
    , m_dbusInterface(new HoneypotManagementDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

HoneypotManagementSystem::~HoneypotManagementSystem() {
}

bool HoneypotManagementSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize honeypot manager
    if (!m_honeypotManager->initialize()) {
        qWarning() << "Failed to initialize honeypot manager";
        return false;
    }
    
    // Initialize threat analyzer
    if (!m_threatAnalyzer->initialize()) {
        qWarning() << "Failed to initialize threat analyzer";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_honeypotManager, &HoneypotManager::honeypotDeployed, this, [this](const QString& honeypotId) {
        QString type = "Unknown";  // Would get from honeypot config
        m_auditLogger->logHoneypotDeployed(honeypotId, type);
    });
    
    connect(m_honeypotManager, &HoneypotManager::threatDetected, this, [this](const QString& honeypotId, const QVariantMap& threatData) {
        // Analyze threat
        ThreatAnalysisResult result = m_threatAnalyzer->analyzeInteraction(threatData);
        m_auditLogger->logThreatDetected(honeypotId, result.threatType);
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

