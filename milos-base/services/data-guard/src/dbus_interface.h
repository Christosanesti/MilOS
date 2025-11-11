#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class ConfigParser;
class PolicyManager;
class NetworkEnforcement;

/**
 * @brief D-Bus Interface for Data Transmission Guard Service
 * 
 * Implements org.milos.DataGuard D-Bus interface with methods
 * and signals for service management and monitoring.
 * Uses Qt D-Bus adaptor pattern.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.DataGuard")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param configParser Configuration parser instance
     * @param policyManager Policy manager instance
     * @param networkEnforcement Network enforcement instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        PolicyManager* policyManager,
        NetworkEnforcement* networkEnforcement
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
     * @brief Get current transmission monitoring status
     * @return JSON string with status information
     */
    QString GetTransmissionStatus();

    /**
     * @brief Get list of blocked transmissions
     * @return List of blocked transmission information (JSON strings)
     */
    QStringList GetBlockedTransmissions();

    /**
     * @brief Configure a transmission policy
     * @param policy JSON string containing policy configuration
     * @return true if configuration successful, false otherwise
     */
    bool ConfigurePolicy(const QString& policy);

    /**
     * @brief Get status of a specific policy
     * @param policyId Policy ID to query
     * @return JSON string with policy status
     */
    QString GetPolicyStatus(const QString& policyId);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a transmission is blocked
     * @param transmissionInfo JSON string with transmission information
     */
    void TransmissionBlocked(const QString& transmissionInfo);

    /**
     * @brief Signal emitted when a policy violation is detected
     * @param violationInfo JSON string with violation information
     */
    void PolicyViolationDetected(const QString& violationInfo);

private:
    bool m_running;
    bool m_initialized;
    
    ConfigParser* m_configParser;
    PolicyManager* m_policyManager;
    NetworkEnforcement* m_networkEnforcement;

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

