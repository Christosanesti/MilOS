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
 * @brief D-Bus Interface Wrapper for Update Service
 * 
 * Wraps the org.milos.UpdateService D-Bus interface for use in QML.
 * Provides signals and properties for reactive UI updates.
 */
class DBusInterfaceWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool serviceAvailable READ serviceAvailable NOTIFY serviceAvailableChanged)
    Q_PROPERTY(QString currentStatus READ currentStatus NOTIFY currentStatusChanged)
    Q_PROPERTY(QStringList availableUpdates READ availableUpdates NOTIFY availableUpdatesChanged)

public:
    explicit DBusInterfaceWrapper(QObject* parent = nullptr);
    ~DBusInterfaceWrapper();

    bool serviceAvailable() const { return m_serviceAvailable; }
    QString currentStatus() const { return m_currentStatus; }
    QStringList availableUpdates() const { return m_availableUpdates; }

public Q_SLOTS:
    /**
     * @brief Check for available updates
     * @return true if check initiated successfully
     */
    Q_INVOKABLE bool checkUpdates();

    /**
     * @brief Apply security updates
     * @param packageList List of package names to update
     * @return Update ID if successful, empty string otherwise
     */
    Q_INVOKABLE QString applyUpdates(const QStringList& packageList);

    /**
     * @brief Rollback update
     * @param updateId Update ID to rollback
     * @return true if rollback initiated successfully
     */
    Q_INVOKABLE bool rollbackUpdate(const QString& updateId);

    /**
     * @brief Get current update status
     * @param updateId Optional update ID (empty for current update)
     * @return JSON string with update status
     */
    Q_INVOKABLE QString getUpdateStatus(const QString& updateId = QString());

    /**
     * @brief Get update history
     * @return JSON string with update history
     */
    Q_INVOKABLE QString getUpdateHistory();

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
    void availableUpdatesChanged(const QStringList& updates);
    
    // Update operation signals
    void updateCheckCompleted(const QString& result);
    void updateCheckFailed(const QString& error);
    void updateStarted(const QString& updateId);
    void updateProgress(const QString& updateId, int progress);
    void updateCompleted(const QString& updateId, bool success);
    void updateError(const QString& updateId, const QString& error);
    void rollbackCompleted(const QString& updateId, bool success);
    void rollbackFailed(const QString& updateId, const QString& error);
    
    // History signals
    void historyUpdated(const QString& history);

private Q_SLOTS:
    void onServiceRegistered(const QString& serviceName);
    void onServiceUnregistered(const QString& serviceName);
    void onUpdateAvailable(const QString& packageInfo);
    void onUpdateStarted(const QString& updateId);
    void onUpdateCompleted(const QString& updateId, const QString& status);
    void onUpdateError(const QString& updateId, const QString& error);
    void onCheckUpdatesFinished(QDBusPendingCallWatcher* watcher);
    void onApplyUpdatesFinished(QDBusPendingCallWatcher* watcher);
    void onRollbackUpdateFinished(QDBusPendingCallWatcher* watcher);
    void onGetUpdateStatusFinished(QDBusPendingCallWatcher* watcher);
    void onGetUpdateHistoryFinished(QDBusPendingCallWatcher* watcher);
    void onGetHealthStatusFinished(QDBusPendingCallWatcher* watcher);

private:
    void connectToService();
    void disconnectFromService();
    void checkServiceAvailability();
    void parseUpdateList(const QString& jsonResult);

    QDBusInterface* m_dbusInterface;
    QDBusConnection m_connection;
    bool m_serviceAvailable;
    QString m_currentStatus;
    QStringList m_availableUpdates;
    QString m_currentUpdateId;
};

#endif // DBUS_INTERFACE_WRAPPER_H

