#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class ConfigParser;
class PackageManager;
class SignatureVerifier;
class UpdateApplier;
class RollbackManager;
class AuditLogger;

/**
 * @brief D-Bus Interface for Update Service
 * 
 * Implements org.milos.UpdateService D-Bus interface with methods
 * and signals for update management operations.
 * Uses Qt D-Bus adaptor pattern.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.UpdateService")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param configParser Configuration parser instance
     * @param packageManager Package manager instance
     * @param signatureVerifier Signature verifier instance
     * @param updateApplier Update applier instance
     * @param rollbackManager Rollback manager instance
     * @param auditLogger Audit logger instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        PackageManager* packageManager,
        SignatureVerifier* signatureVerifier,
        UpdateApplier* updateApplier,
        RollbackManager* rollbackManager,
        AuditLogger* auditLogger
    );

    /**
     * @brief Start D-Bus interface
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop D-Bus interface
     */
    void stop();

    /**
     * @brief Check if D-Bus interface is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Check if D-Bus interface is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Check for available updates
     * @return JSON string with available updates
     */
    QString CheckUpdates();

    /**
     * @brief Apply security updates
     * @param packageList List of packages to update (JSON array string)
     * @return Update ID if successful, empty string otherwise
     */
    QString ApplyUpdates(const QString& packageList);

    /**
     * @brief Rollback update
     * @param updateId Update ID to rollback
     * @return true if rollback successful, false otherwise
     */
    bool RollbackUpdate(const QString& updateId);

    /**
     * @brief Get current update status
     * @param updateId Update ID (optional, empty for current update)
     * @return JSON string with update status
     */
    QString GetUpdateStatus(const QString& updateId = QString());

    /**
     * @brief Get update history
     * @return JSON string with update history
     */
    QString GetUpdateHistory();

    /**
     * @brief Get detailed health status of the service
     * @return JSON string with health status information
     */
    QString GetHealthStatus();

Q_SIGNALS:
    /**
     * @brief Signal emitted when updates are available
     * @param packageInfo JSON string with package information
     */
    void UpdateAvailable(const QString& packageInfo);

    /**
     * @brief Signal emitted when update starts
     * @param updateId Update ID
     */
    void UpdateStarted(const QString& updateId);

    /**
     * @brief Signal emitted when update completes
     * @param updateId Update ID
     * @param status Update status (COMPLETED, FAILED)
     */
    void UpdateCompleted(const QString& updateId, const QString& status);

    /**
     * @brief Signal emitted when update error occurs
     * @param updateId Update ID
     * @param error Error message
     */
    void UpdateError(const QString& updateId, const QString& error);

private:
    bool m_running;
    bool m_initialized;

    ConfigParser* m_configParser;
    PackageManager* m_packageManager;
    SignatureVerifier* m_signatureVerifier;
    UpdateApplier* m_updateApplier;
    RollbackManager* m_rollbackManager;
    AuditLogger* m_auditLogger;

    /**
     * @brief Register D-Bus interface
     * @return true if registration successful, false otherwise
     */
    bool registerInterface();

    /**
     * @brief Unregister D-Bus interface
     */
    void unregisterInterface();
};

#endif // DBUS_INTERFACE_H

