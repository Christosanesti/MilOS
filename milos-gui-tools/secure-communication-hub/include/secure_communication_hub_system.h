#ifndef SECURE_COMMUNICATION_HUB_SYSTEM_H
#define SECURE_COMMUNICATION_HUB_SYSTEM_H

#include "communication_hub.h"
#include "message_expiration.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Secure Communication Hub System
 * 
 * Main system for unified secure communication.
 */
class SecureCommunicationHubSystem : public QObject {
    Q_OBJECT

public:
    explicit SecureCommunicationHubSystem(QObject* parent = nullptr);
    ~SecureCommunicationHubSystem();

    /**
     * @brief Initialize secure communication hub system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get communication hub
     * @return Communication hub
     */
    CommunicationHub* getHub() const { return m_hub; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    SecureCommunicationHubDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    CommunicationHub* m_hub;
    MessageExpirationManager* m_expirationManager;
    SecureCommunicationHubDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // SECURE_COMMUNICATION_HUB_SYSTEM_H

