#include "ai_security_monitoring_system.h"
#include <QDebug>

AISecurityMonitoringSystem::AISecurityMonitoringSystem(QObject* parent)
    : QObject(parent)
    , m_threatDetector(new ThreatDetector(this))
    , m_incidentResponse(new IncidentResponse(this))
    , m_systemMonitor(new SystemMonitor(this))
    , m_dbusInterface(new AISecurityMonitoringDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
    , m_running(false)
{
}

AISecurityMonitoringSystem::~AISecurityMonitoringSystem() {
    stop();
}

bool AISecurityMonitoringSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize threat detector
    if (!m_threatDetector->initialize()) {
        qWarning() << "Failed to initialize threat detector";
        return false;
    }
    
    // Initialize incident response
    if (!m_incidentResponse->initialize()) {
        qWarning() << "Failed to initialize incident response";
        return false;
    }
    
    // Initialize system monitor
    if (!m_systemMonitor->initialize()) {
        qWarning() << "Failed to initialize system monitor";
        return false;
    }
    
    // Connect signals
    connect(m_threatDetector, &ThreatDetector::threatDetected, this, [this](const DetectedThreat& threat) {
        QString threatTypeStr;
        switch (threat.type) {
            case ThreatType::Intrusion: threatTypeStr = "Intrusion"; break;
            case ThreatType::Malware: threatTypeStr = "Malware"; break;
            case ThreatType::APT: threatTypeStr = "APT"; break;
            case ThreatType::ZeroDay: threatTypeStr = "ZeroDay"; break;
            case ThreatType::InsiderThreat: threatTypeStr = "InsiderThreat"; break;
            case ThreatType::FileIntegrity: threatTypeStr = "FileIntegrity"; break;
            default: threatTypeStr = "Unknown"; break;
        }
        m_auditLogger->logThreatDetected(threat.id, threatTypeStr);
        
        // Auto-respond to threat
        m_incidentResponse->respondToThreat(threat);
    });
    
    connect(m_incidentResponse, &IncidentResponse::threatContained, this, [this](const QString& threatId) {
        m_auditLogger->logThreatContained(threatId);
    });
    
    connect(m_systemMonitor, &SystemMonitor::anomalyDetected, this, [this](const QString& type, const QVariantMap& details) {
        m_auditLogger->logAnomalyDetected(type, details.value("description").toString());
        
        // Analyze anomaly as potential threat
        m_threatDetector->analyzeData(details, "system");
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

bool AISecurityMonitoringSystem::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }
    
    if (m_running) {
        return true;
    }
    
    if (!m_threatDetector->startMonitoring()) {
        return false;
    }
    
    if (!m_systemMonitor->startMonitoring()) {
        m_threatDetector->stopMonitoring();
        return false;
    }
    
    m_running = true;
    return true;
}

void AISecurityMonitoringSystem::stop() {
    if (!m_running) {
        return;
    }
    
    m_threatDetector->stopMonitoring();
    m_systemMonitor->stopMonitoring();
    
    m_running = false;
}

