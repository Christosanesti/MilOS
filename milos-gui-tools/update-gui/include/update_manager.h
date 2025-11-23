#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QTimer>

class DBusInterfaceWrapper;

/**
 * @brief Update Manager for GUI Application
 * 
 * Manages update operations and state for the GUI application.
 * Provides high-level interface for update management operations.
 */
class UpdateManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool checkingUpdates READ checkingUpdates NOTIFY checkingUpdatesChanged)
    Q_PROPERTY(bool applyingUpdates READ applyingUpdates NOTIFY applyingUpdatesChanged)
    Q_PROPERTY(int updateProgress READ updateProgress NOTIFY updateProgressChanged)
    Q_PROPERTY(QString currentUpdateId READ currentUpdateId NOTIFY currentUpdateIdChanged)
    Q_PROPERTY(QVariantList updateHistory READ updateHistory NOTIFY updateHistoryChanged)

public:
    explicit UpdateManager(QObject* parent = nullptr);
    ~UpdateManager();

    bool checkingUpdates() const { return m_checkingUpdates; }
    bool applyingUpdates() const { return m_applyingUpdates; }
    int updateProgress() const { return m_updateProgress; }
    QString currentUpdateId() const { return m_currentUpdateId; }
    QVariantList updateHistory() const { return m_updateHistory; }

    void setDBusInterface(DBusInterfaceWrapper* dbusInterface);

public Q_SLOTS:
    /**
     * @brief Check for available updates
     */
    Q_INVOKABLE void checkForUpdates();

    /**
     * @brief Apply selected updates
     * @param packageList List of package names to update
     */
    Q_INVOKABLE void applySelectedUpdates(const QStringList& packageList);

    /**
     * @brief Rollback a specific update
     * @param updateId Update ID to rollback
     */
    Q_INVOKABLE void performRollback(const QString& updateId);

    /**
     * @brief Refresh update history
     */
    Q_INVOKABLE void refreshHistory();

    /**
     * @brief Cancel current update operation
     */
    Q_INVOKABLE void cancelUpdate();

Q_SIGNALS:
    void checkingUpdatesChanged(bool checking);
    void applyingUpdatesChanged(bool applying);
    void updateProgressChanged(int progress);
    void currentUpdateIdChanged(const QString& updateId);
    void updateHistoryChanged(const QVariantList& history);
    
    // Operation completion signals
    void updateCheckCompleted(const QVariantList& updates);
    void updateCheckFailed(const QString& error);
    void updateStarted(const QString& updateId);
    void updateProgressUpdate(const QString& updateId, int progress);
    void updateCompleted(const QString& updateId, bool success);
    void updateFailed(const QString& updateId, const QString& error);
    void rollbackCompleted(const QString& updateId, bool success);
    void rollbackFailed(const QString& updateId, const QString& error);

private Q_SLOTS:
    void onUpdateCheckCompleted(const QString& result);
    void onUpdateCheckFailed(const QString& error);
    void onUpdateStarted(const QString& updateId);
    void onUpdateProgress(const QString& updateId, int progress);
    void onUpdateCompleted(const QString& updateId, bool success);
    void onUpdateError(const QString& updateId, const QString& error);
    void onRollbackCompleted(const QString& updateId, bool success);
    void onRollbackFailed(const QString& updateId, const QString& error);
    void onHistoryUpdated(const QString& history);
    void onStatusPollTimeout();

private:
    void parseUpdateList(const QString& jsonResult);
    void parseHistory(const QString& jsonResult);
    void startStatusPolling();
    void stopStatusPolling();

    DBusInterfaceWrapper* m_dbusInterface;
    bool m_checkingUpdates;
    bool m_applyingUpdates;
    int m_updateProgress;
    QString m_currentUpdateId;
    QVariantList m_updateHistory;
    QVariantList m_availableUpdates;
    QTimer* m_statusPollTimer;
};

#endif // UPDATE_MANAGER_H

