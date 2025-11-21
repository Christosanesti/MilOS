#include "military_radio_system.h"
#include <QDebug>

MilitaryRadioSystem::MilitaryRadioSystem(QObject* parent)
    : QObject(parent)
    , m_radioManager(new RadioManager(this))
    , m_radioEncryption(new RadioEncryption(this))
    , m_dbusInterface(new MilitaryRadioDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

MilitaryRadioSystem::~MilitaryRadioSystem() {
}

bool MilitaryRadioSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize radio encryption
    if (!m_radioEncryption->initialize()) {
        qWarning() << "Failed to initialize radio encryption";
        return false;
    }
    
    // Initialize radio manager
    if (!m_radioManager->initialize()) {
        qWarning() << "Failed to initialize radio manager";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_radioManager, &RadioManager::messageReceived, this, [this](const RadioMessage& message) {
        m_auditLogger->logMessageReceived(message.id, message.protocol);
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

