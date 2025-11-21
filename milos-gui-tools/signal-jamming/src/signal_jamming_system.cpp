#include "signal_jamming_system.h"
#include <QDebug>

SignalJammingSystem::SignalJammingSystem(QObject* parent)
    : QObject(parent)
    , m_manager(new JammingManager(this))
    , m_dbusInterface(new SignalJammingDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

SignalJammingSystem::~SignalJammingSystem() {
}

bool SignalJammingSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize manager
    if (!m_manager->initialize()) {
        qWarning() << "Failed to initialize jamming manager";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_manager, &JammingManager::jammingStarted, this, [this](const QString& deviceId, const QString& patternId) {
        m_auditLogger->logJammingStarted(deviceId, patternId);
    });
    
    connect(m_manager, &JammingManager::jammingStopped, this, [this](const QString& deviceId) {
        m_auditLogger->logJammingStopped(deviceId);
    });
    
    connect(m_manager, &JammingManager::deviceAdded, this, [this](const QString& deviceId) {
        m_auditLogger->logDeviceConnected(deviceId);
    });
    
    connect(m_manager, &JammingManager::deviceRemoved, this, [this](const QString& deviceId) {
        m_auditLogger->logDeviceDisconnected(deviceId);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setManager(m_manager);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

