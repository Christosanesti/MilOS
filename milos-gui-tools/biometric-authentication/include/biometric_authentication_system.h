#ifndef BIOMETRIC_AUTHENTICATION_SYSTEM_H
#define BIOMETRIC_AUTHENTICATION_SYSTEM_H

#include "biometric_authenticator.h"
#include "mfa_manager.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Biometric Authentication System
 * 
 * Main system for biometric authentication integration.
 */
class BiometricAuthenticationSystem : public QObject {
    Q_OBJECT

public:
    explicit BiometricAuthenticationSystem(QObject* parent = nullptr);
    ~BiometricAuthenticationSystem();

    /**
     * @brief Initialize biometric authentication system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get biometric authenticator
     * @return Biometric authenticator
     */
    BiometricAuthenticator* getAuthenticator() const { return m_authenticator; }

    /**
     * @brief Get MFA manager
     * @return MFA manager
     */
    MFAManager* getMFAManager() const { return m_mfaManager; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    BiometricAuthenticationDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    BiometricAuthenticator* m_authenticator;
    MFAManager* m_mfaManager;
    BiometricAuthenticationDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // BIOMETRIC_AUTHENTICATION_SYSTEM_H

