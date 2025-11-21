#include "satellite_communication_system.h"
#include <QDebug>

SatelliteCommunicationSystem::SatelliteCommunicationSystem(QObject* parent)
    : QObject(parent)
    , m_bandwidthOptimizer(new BandwidthOptimizer(this))
    , m_dbusInterface(new SatelliteCommunicationDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

SatelliteCommunicationSystem::~SatelliteCommunicationSystem() {
}

bool SatelliteCommunicationSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize bandwidth optimizer
    if (!m_bandwidthOptimizer->initialize()) {
        qWarning() << "Failed to initialize bandwidth optimizer";
        return false;
    }
    
    // Initialize D-Bus interface
    m_dbusInterface->setSystem(this);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

bool SatelliteCommunicationSystem::addProtocol(SatComProtocol* protocol) {
    if (!protocol) {
        return false;
    }
    
    m_protocols.append(protocol);
    
    // Connect signals for audit logging
    connect(protocol, &SatComProtocol::messageReceived, this, [this](const SatComMessage& message) {
        m_auditLogger->logMessageSent(message.id, message.protocol);
    });
    
    return true;
}

