#ifndef DATA_GUARD_MANAGER_H
#define DATA_GUARD_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>
#include <QTimer>

class DBusInterfaceWrapper;

/**
 * @brief Data Guard Manager for GUI Application
 * 
 * Manages data guard operations and state for the GUI application.
 * Provides high-level interface for policy management and monitoring operations.
 */
class DataGuardManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loadingStatus READ loadingStatus NOTIFY loadingStatusChanged)
    Q_PROPERTY(bool configuringPolicy READ configuringPolicy NOTIFY configuringPolicyChanged)
    Q_PROPERTY(QVariantMap transmissionStatus READ transmissionStatus NOTIFY transmissionStatusChanged)
    Q_PROPERTY(QVariantList blockedTransmissions READ blockedTransmissions NOTIFY blockedTransmissionsChanged)
    Q_PROPERTY(QVariantMap healthStatus READ healthStatus NOTIFY healthStatusChanged)

public:
    explicit DataGuardManager(QObject* parent = nullptr);
    ~DataGuardManager();

    bool loadingStatus() const { return m_loadingStatus; }
    bool configuringPolicy() const { return m_configuringPolicy; }
    QVariantMap transmissionStatus() const { return m_transmissionStatus; }
    QVariantList blockedTransmissions() const { return m_blockedTransmissions; }
    QVariantMap healthStatus() const { return m_healthStatus; }

    void setDBusInterface(DBusInterfaceWrapper* dbusInterface);

public Q_SLOTS:
    /**
     * @brief Refresh transmission status
     */
    Q_INVOKABLE void refreshTransmissionStatus();

    /**
     * @brief Refresh blocked transmissions list
     */
    Q_INVOKABLE void refreshBlockedTransmissions();

    /**
     * @brief Configure a policy
     * @param policy Policy configuration as QVariantMap
     */
    Q_INVOKABLE void configurePolicy(const QVariantMap& policy);

    /**
     * @brief Get policy status
     * @param policyId Policy ID
     */
    Q_INVOKABLE void getPolicyStatus(const QString& policyId);

    /**
     * @brief Refresh health status
     */
    Q_INVOKABLE void refreshHealthStatus();

Q_SIGNALS:
    void loadingStatusChanged(bool loading);
    void configuringPolicyChanged(bool configuring);
    void transmissionStatusChanged(const QVariantMap& status);
    void blockedTransmissionsChanged(const QVariantList& blocked);
    void healthStatusChanged(const QVariantMap& health);
    
    // Operation completion signals
    void transmissionStatusUpdated(const QVariantMap& status);
    void transmissionStatusFailed(const QString& error);
    void blockedTransmissionsUpdated(const QVariantList& blocked);
    void blockedTransmissionsFailed(const QString& error);
    void policyConfigured(bool success);
    void policyConfigurationFailed(const QString& error);
    void policyStatusUpdated(const QVariantMap& status);
    void policyStatusFailed(const QString& error);
    void healthStatusUpdated(const QVariantMap& health);
    void transmissionBlocked(const QVariantMap& transmissionInfo);
    void policyViolationDetected(const QVariantMap& violationInfo);

private Q_SLOTS:
    void onTransmissionStatusUpdated(const QString& status);
    void onTransmissionStatusFailed(const QString& error);
    void onBlockedTransmissionsUpdated(const QString& blocked);
    void onBlockedTransmissionsFailed(const QString& error);
    void onPolicyConfigured(bool success);
    void onPolicyConfigurationFailed(const QString& error);
    void onPolicyStatusUpdated(const QString& status);
    void onPolicyStatusFailed(const QString& error);
    void onHealthStatusUpdated(const QString& healthStatus);
    void onTransmissionBlocked(const QString& transmissionInfo);
    void onPolicyViolationDetected(const QString& violationInfo);
    void onStatusPollTimeout();

private:
    void parseTransmissionStatus(const QString& jsonResult);
    void parseBlockedTransmissions(const QString& jsonResult);
    void parseHealthStatus(const QString& jsonResult);
    void parsePolicyStatus(const QString& jsonResult);
    QVariantMap jsonToVariantMap(const QString& jsonString);
    void startStatusPolling();
    void stopStatusPolling();

    DBusInterfaceWrapper* m_dbusInterface;
    bool m_loadingStatus;
    bool m_configuringPolicy;
    QVariantMap m_transmissionStatus;
    QVariantList m_blockedTransmissions;
    QVariantMap m_healthStatus;
    QTimer* m_statusPollTimer;
};

#endif // DATA_GUARD_MANAGER_H

