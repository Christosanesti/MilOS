#include "secure_communication_hub_system.h"
#include <QDebug>

SecureCommunicationHubSystem::SecureCommunicationHubSystem(QObject* parent)
    : QObject(parent)
    , m_hub(new CommunicationHub(this))
    , m_expirationManager(new MessageExpirationManager(this))
    , m_dbusInterface(new SecureCommunicationHubDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

SecureCommunicationHubSystem::~SecureCommunicationHubSystem() {
}

bool SecureCommunicationHubSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize hub
    if (!m_hub->initialize()) {
        qWarning() << "Failed to initialize communication hub";
        return false;
    }
    
    // Initialize expiration manager
    if (!m_expirationManager->initialize(m_hub)) {
        qWarning() << "Failed to initialize message expiration manager";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_hub, &CommunicationHub::messageReceived, this, [this](const UnifiedMessage& message) {
        QString typeStr;
        switch (message.type) {
            case CommunicationType::Email: typeStr = "email"; break;
            case CommunicationType::Chat: typeStr = "chat"; break;
            case CommunicationType::Voice: typeStr = "voice"; break;
            case CommunicationType::Video: typeStr = "video"; break;
        }
        m_auditLogger->logMessageReceived(message.id, typeStr);
    });
    
    connect(m_expirationManager, &MessageExpirationManager::messageExpired, this, [this](const QString& messageId) {
        m_auditLogger->logMessageExpired(messageId);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setHub(m_hub);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

