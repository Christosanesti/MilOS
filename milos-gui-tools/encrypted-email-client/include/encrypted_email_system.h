#ifndef ENCRYPTED_EMAIL_SYSTEM_H
#define ENCRYPTED_EMAIL_SYSTEM_H

#include "email_client.h"
#include "email_storage.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Encrypted Email System
 * 
 * Main system for encrypted email client.
 */
class EncryptedEmailSystem : public QObject {
    Q_OBJECT

public:
    explicit EncryptedEmailSystem(QObject* parent = nullptr);
    ~EncryptedEmailSystem();

    /**
     * @brief Initialize encrypted email system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get email client
     * @return Email client
     */
    EmailClient* getEmailClient() const { return m_emailClient; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    EncryptedEmailDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    EmailClient* m_emailClient;
    EmailStorage* m_emailStorage;
    EncryptedEmailDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // ENCRYPTED_EMAIL_SYSTEM_H

