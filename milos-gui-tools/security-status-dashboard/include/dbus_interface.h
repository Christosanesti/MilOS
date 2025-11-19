#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QVariantMap>

/**
 * @brief D-Bus Interface for Security Status Dashboard
 * 
 * Provides D-Bus interface org.milos.SecurityStatusDashboard
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SecurityStatusDashboard")

public:
    explicit DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get overall security status
     * @return Overall status as JSON string
     */
    Q_SCRIPTABLE QString GetOverallStatus();

    /**
     * @brief Get security posture
     * @return Security posture as JSON string
     */
    Q_SCRIPTABLE QString GetSecurityPosture();

    /**
     * @brief Get hardening status
     * @return Hardening status as JSON string
     */
    Q_SCRIPTABLE QString GetHardeningStatus();

    /**
     * @brief Get encryption status
     * @return Encryption status as JSON string
     */
    Q_SCRIPTABLE QString GetEncryptionStatus();

    /**
     * @brief Get network security status
     * @return Network security status as JSON string
     */
    Q_SCRIPTABLE QString GetNetworkSecurityStatus();

    /**
     * @brief Get update status
     * @return Update status as JSON string
     */
    Q_SCRIPTABLE QString GetUpdateStatus();

    /**
     * @brief Refresh all status
     * @return true if refresh successful, false otherwise
     */
    Q_SCRIPTABLE bool RefreshStatus();

    /**
     * @brief Set status aggregator
     */
    void setStatusAggregator(class StatusAggregator* statusAggregator);

Q_SIGNALS:
    /**
     * @brief Emitted when status is refreshed
     */
    Q_SCRIPTABLE void StatusRefreshed();

private:
    bool m_initialized;
    class StatusAggregator* m_statusAggregator;
};

#endif // DBUS_INTERFACE_H

