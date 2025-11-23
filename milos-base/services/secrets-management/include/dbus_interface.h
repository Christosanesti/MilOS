#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDBusContext>
#include <QDBusConnection>

class SecretsService;
class SecretStorage;
class SecretRotation;
class SecretVersioning;
class AccessControl;
class ApplicationIntegration;

/**
 * @brief D-Bus Interface for Secrets Management
 * 
 * Provides D-Bus methods and signals for secrets management.
 */
class DBusInterface : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SecretsManagement")

public:
    explicit DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    // Setters for service components
    void setSecretsService(SecretsService* service) { m_secretsService = service; }
    void setSecretStorage(SecretStorage* storage) { m_secretStorage = storage; }
    void setSecretRotation(SecretRotation* rotation) { m_secretRotation = rotation; }
    void setSecretVersioning(SecretVersioning* versioning) { m_secretVersioning = versioning; }
    void setAccessControl(AccessControl* accessControl) { m_accessControl = accessControl; }
    void setApplicationIntegration(ApplicationIntegration* appIntegration) { m_applicationIntegration = appIntegration; }

public Q_SLOTS:
    /**
     * @brief Store secret
     * @param secretId Secret ID
     * @param secretData Secret data (base64 encoded)
     * @param secretType Secret type
     * @param description Description
     * @param expiresAt Expiration date (ISO format)
     * @return true if storage successful, false otherwise
     */
    bool StoreSecret(const QString& secretId, const QString& secretData,
                    const QString& secretType, const QString& description,
                    const QString& expiresAt);

    /**
     * @brief Get secret
     * @param secretId Secret ID
     * @param version Version ID (empty for latest)
     * @return Secret data (base64 encoded), or empty if not found
     */
    QString GetSecret(const QString& secretId, const QString& version = QString());

    /**
     * @brief Rotate secret
     * @param secretId Secret ID
     * @return true if rotation successful, false otherwise
     */
    bool RotateSecret(const QString& secretId);

    /**
     * @brief List all secrets
     * @return List of secret IDs
     */
    QStringList ListSecrets();

    /**
     * @brief Get secret metadata
     * @param secretId Secret ID
     * @return Metadata as QVariantMap (JSON-like structure)
     */
    QVariantMap GetSecretMetadata(const QString& secretId);

    /**
     * @brief Rollback secret to version
     * @param secretId Secret ID
     * @param versionId Version ID
     * @return true if rollback successful, false otherwise
     */
    bool RollbackSecret(const QString& secretId, const QString& versionId);

    /**
     * @brief Delete secret
     * @param secretId Secret ID
     * @return true if deletion successful, false otherwise
     */
    bool DeleteSecret(const QString& secretId);

    /**
     * @brief Authenticate application (Application API)
     * @param applicationId Application ID
     * @param dbusServiceName D-Bus service name
     * @param certificateHash Certificate hash (optional)
     * @return Application identity (JSON string)
     */
    QString AuthenticateApplication(const QString& applicationId,
                                   const QString& dbusServiceName,
                                   const QString& certificateHash = "");

    /**
     * @brief Get secret for application (Application API - with caching)
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @param version Version ID (empty for latest)
     * @return Secret data (base64 encoded), or empty if not found
     */
    QString GetSecretForApplication(const QString& applicationId,
                                    const QString& secretId,
                                    const QString& version = "");

    /**
     * @brief Inject secret into application
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @param injectionMethod Injection method ("environment", "file", "dbus")
     * @param targetPath Target path (for file injection)
     * @return Injection request (JSON string)
     */
    QString InjectSecret(const QString& applicationId,
                        const QString& secretId,
                        const QString& injectionMethod,
                        const QString& targetPath = "");

    /**
     * @brief Refresh cached secret
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @return true if refresh successful, false otherwise
     */
    bool RefreshCachedSecret(const QString& applicationId, const QString& secretId);

    /**
     * @brief Register application
     * @param applicationId Application ID
     * @param applicationName Application name
     * @param dbusServiceName D-Bus service name
     * @return true if registration successful, false otherwise
     */
    bool RegisterApplication(const QString& applicationId,
                            const QString& applicationName,
                            const QString& dbusServiceName);

Q_SIGNALS:
    /**
     * @brief Emitted when secret is rotated
     * @param secretId Secret ID
     */
    void SecretRotated(const QString& secretId);

    /**
     * @brief Emitted when secret is expiring
     * @param secretId Secret ID
     * @param daysRemaining Days until expiration
     */
    void SecretExpiring(const QString& secretId, int daysRemaining);

private:
    SecretsService* m_secretsService;
    SecretStorage* m_secretStorage;
    SecretRotation* m_secretRotation;
    SecretVersioning* m_secretVersioning;
    AccessControl* m_accessControl;
    ApplicationIntegration* m_applicationIntegration;
    QDBusConnection m_dbusConnection;
    bool m_registered;
};

#endif // DBUS_INTERFACE_H

