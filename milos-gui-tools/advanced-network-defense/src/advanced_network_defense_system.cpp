#include "advanced_network_defense_system.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

AdvancedNetworkDefenseSystem::AdvancedNetworkDefenseSystem(QObject* parent)
    : QObject(parent)
    , m_mlDetector(new MLThreatDetector(this))
    , m_forensics(new NetworkForensics(this))
    , m_packetAnalyzer(new PacketAnalyzer(this))
    , m_idsEngine(new IDSRulesEngine(this))
    , m_incidentWorkflow(new IncidentResponseWorkflow(this))
    , m_dbusInterface(new AdvancedNetworkDefenseDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

AdvancedNetworkDefenseSystem::~AdvancedNetworkDefenseSystem() {
}

bool AdvancedNetworkDefenseSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize ML threat detector
    if (!m_mlDetector->initialize()) {
        qWarning() << "Failed to initialize ML threat detector";
        return false;
    }
    
    // Initialize network forensics
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/network_forensics.db";
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!m_forensics->initialize(dbPath)) {
        qWarning() << "Failed to initialize network forensics";
        return false;
    }
    
    // Initialize packet analyzer
    if (!m_packetAnalyzer->initialize()) {
        qWarning() << "Failed to initialize packet analyzer";
        return false;
    }
    
    // Initialize IDS rules engine
    if (!m_idsEngine->initialize()) {
        qWarning() << "Failed to initialize IDS rules engine";
        return false;
    }
    
    // Initialize incident response workflow
    if (!m_incidentWorkflow->initialize()) {
        qWarning() << "Failed to initialize incident response workflow";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_mlDetector, &MLThreatDetector::threatDetected, this, [this](const MLThreatResult& threat) {
        QString typeStr;
        switch (threat.type) {
            case MLThreatType::Intrusion: typeStr = "Intrusion"; break;
            case MLThreatType::Malware: typeStr = "Malware"; break;
            case MLThreatType::APT: typeStr = "APT"; break;
            case MLThreatType::DDoS: typeStr = "DDoS"; break;
            case MLThreatType::PortScan: typeStr = "PortScan"; break;
            case MLThreatType::Anomaly: typeStr = "Anomaly"; break;
            default: typeStr = "Unknown"; break;
        }
        m_auditLogger->logThreatDetected(threat.id, typeStr);
    });
    
    connect(m_forensics, &NetworkForensics::evidenceCaptured, this, [this](const QString& evidenceId, const QString& type) {
        m_auditLogger->logForensicCapture(evidenceId, type);
    });
    
    connect(m_incidentWorkflow, &IncidentResponseWorkflow::workflowCompleted, this, [this](const QString& workflowId, bool success) {
        // Log workflow execution
        QString threatId = "unknown";  // Would get from workflow context
        m_auditLogger->logWorkflowExecuted(workflowId, threatId);
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

