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
 * @brief D-Bus Interface Wrapper for Audit Service
 * 
 * Wraps the org.milos.AuditService D-Bus interface for use in QML.
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
     * @brief Log a security event
     * @param eventData Event data (JSON string)
     * @return Event ID if logged successfully, empty string otherwise
     */
    Q_INVOKABLE QString logEvent(const QString& eventData);

    /**
     * @brief Query audit logs
     * @param queryParams Query parameters (JSON string)
     * @return Query results (JSON string)
     */
    Q_INVOKABLE QString queryEvents(const QString& queryParams);

    /**
     * @brief Get event statistics
     * @param queryParams Query parameters (JSON string)
     * @return Statistics (JSON string)
     */
    Q_INVOKABLE QString getEventStatistics(const QString& queryParams);

    /**
     * @brief Export audit logs
     * @param timeRange Time range (JSON string)
     * @param format Export format (json, csv, xml)
     * @return Exported data (string)
     */
    Q_INVOKABLE QString exportAuditLog(const QString& timeRange, const QString& format);

    /**
     * @brief Verify log integrity
     * @return true if integrity verified, false if tampering detected
     */
    Q_INVOKABLE bool verifyLogIntegrity();

    /**
     * @brief Get service health status
     * @return JSON string with health status
     */
    Q_INVOKABLE QString getHealthStatus();

    /**
     * @brief Refresh service connection
     */
    Q_INVOKABLE void refreshConnection();

Q_SIGNALS:
    void serviceAvailableChanged(bool available);
    void currentStatusChanged(const QString& status);
    
    // Event operation signals
    void eventLogged(const QString& eventId);
    void eventLogFailed(const QString& error);
    void queryCompleted(const QString& results);
    void queryFailed(const QString& error);
    void statisticsUpdated(const QString& statistics);
    void statisticsFailed(const QString& error);
    void exportCompleted(const QString& data, const QString& format);
    void exportFailed(const QString& error);
    void integrityVerified(bool verified);
    void integrityViolationDetected();
    void healthStatusUpdated(const QString& healthStatus);

private Q_SLOTS:
    void onServiceRegistered(const QString& serviceName);
    void onServiceUnregistered(const QString& serviceName);
    void onEventLogged(const QString& eventId);
    void onIntegrityViolationDetected();
    void onLogEventFinished(QDBusPendingCallWatcher* watcher);
    void onQueryEventsFinished(QDBusPendingCallWatcher* watcher);
    void onGetEventStatisticsFinished(QDBusPendingCallWatcher* watcher);
    void onExportAuditLogFinished(QDBusPendingCallWatcher* watcher);
    void onVerifyLogIntegrityFinished(QDBusPendingCallWatcher* watcher);
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

