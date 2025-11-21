#include "air_gap_system.h"
#include <QDebug>

AirGapSystem::AirGapSystem(QObject* parent)
    : QObject(parent)
    , m_detector(new AirGapDetector(this))
    , m_enforcement(new AirGapEnforcement(this))
    , m_dbusInterface(new AirGapDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
    , m_running(false)
{
}

AirGapSystem::~AirGapSystem() {
    stop();
}

bool AirGapSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize detector
    if (!m_detector->initialize()) {
        qWarning() << "Failed to initialize air-gap detector";
        return false;
    }
    
    // Initialize enforcement
    if (!m_enforcement->initialize(m_detector)) {
        qWarning() << "Failed to initialize air-gap enforcement";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_detector, &AirGapDetector::statusChanged, this, [this](AirGapStatus status) {
        QString statusStr = (status == AirGapStatus::AirGapped) ? "AirGapped" : "Connected";
        m_auditLogger->logStatusChange("Unknown", statusStr);
    });
    
    connect(m_enforcement, &AirGapEnforcement::policyLevelChanged, this, [this](SecurityPolicyLevel level) {
        QString levelStr;
        switch (level) {
            case SecurityPolicyLevel::Normal:
                levelStr = "Normal";
                break;
            case SecurityPolicyLevel::Enhanced:
                levelStr = "Enhanced";
                break;
            case SecurityPolicyLevel::Maximum:
                levelStr = "Maximum";
                break;
        }
        m_auditLogger->logPolicyChange("Unknown", levelStr);
    });
    
    connect(m_enforcement, &AirGapEnforcement::operationBlocked, this, [this](const QString& operation, const QString& reason) {
        m_auditLogger->logBlockedOperation(operation, reason);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setDetector(m_detector);
    m_dbusInterface->setEnforcement(m_enforcement);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

bool AirGapSystem::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }
    
    if (m_running) {
        return true;
    }
    
    m_running = true;
    return true;
}

void AirGapSystem::stop() {
    if (!m_running) {
        return;
    }
    
    m_running = false;
}

