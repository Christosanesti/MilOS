#ifndef DBUS_INTERFACE_WRAPPER_H
#define DBUS_INTERFACE_WRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>

/**
 * @brief D-Bus Interface Wrapper for Data Transmission Guard Service
 * 
 * Wraps the org.milos.DataGuard D-Bus interface for use in QML.
 * Provides signals and properties for reactive UI updates.
 */
class DBusInterfaceWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool serviceAvailable READ serviceAvailable NOTIFY serviceAvailableChanged)
    Q_PROPERTY(QString currentStatus READ currentStatus NOTIFY currentStatusChanged)

public:
    explicit DBusInterfaceWrapper(QObject* parent = nullptr);
    ~DBusInterfaceWrapper();

    bool serviceAvailable() const { return m_serviceAvailable; }
    QString currentStatus() const { return m_currentStatus; }

public Q_SLOTS:
    /**
     * @brief Get current transmission monitoring status
     * @return JSON string with status information
     */
    Q_INVOKABLE QString getTransmissionStatus();

    /**
     * @brief Get list of blocked transmissions
     * @return JSON string with blocked transmissions list
     */
    Q_INVOKABLE QString getBlockedTransmissions();

    /**
     * @brief Configure a transmission policy
     * @param policy JSON string containing policy configuration
     * @return true if configuration successful, false otherwise
     */
    Q_INVOKABLE bool configurePolicy(const QString& policy);

    /**
     * @brief Get status of a specific policy
     * @param policyId Policy ID to query
     * @return JSON string with policy status
     */
    Q_INVOKABLE QString getPolicyStatus(const QString& policyId);

    /**
     * @brief Get detailed health status of the service
     * @return JSON string with health status information
     */
    Q_INVOKABLE QString getHealthStatus();

    /**
     * @brief Refresh service connection
     */
    Q_INVOKABLE void refreshConnection();

Q_SIGNALS:
    void serviceAvailableChanged(bool available);
    void currentStatusChanged(const QString& status);
    
    // Operation signals
    void transmissionStatusUpdated(const QString& status);
    void transmissionStatusFailed(const QString& error);
    void blockedTransmissionsUpdated(const QString& blocked);
    void blockedTransmissionsFailed(const QString& error);
    void policyConfigured(bool success);
    void policyConfigurationFailed(const QString& error);
    void policyStatusUpdated(const QString& status);
    void policyStatusFailed(const QString& error);
    void healthStatusUpdated(const QString& healthStatus);
    void transmissionBlocked(const QString& transmissionInfo);
    void policyViolationDetected(const QString& violationInfo);

private Q_SLOTS:
    void onServiceRegistered(const QString& serviceName);
    void onServiceUnregistered(const QString& serviceName);
    void onTransmissionBlocked(const QString& transmissionInfo);
    void onPolicyViolationDetected(const QString& violationInfo);
    void onGetTransmissionStatusFinished(QDBusPendingCallWatcher* watcher);
    void onGetBlockedTransmissionsFinished(QDBusPendingCallWatcher* watcher);
    void onConfigurePolicyFinished(QDBusPendingCallWatcher* watcher);
    void onGetPolicyStatusFinished(QDBusPendingCallWatcher* watcher);
    void onGetHealthStatusFinished(QDBusPendingCallWatcher* watcher);

private:
    void connectToService();
    void disconnectFromService();
    void checkServiceAvailability();

    QDBusInterface* m_dbusInterface;
    QDBusConnection m_connection;
    bool m_serviceAvailable;
    QString m_currentStatus;
};

#endif // DBUS_INTERFACE_WRAPPER_H

