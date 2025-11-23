#ifndef AUDIT_MANAGER_H
#define AUDIT_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>
#include <QTimer>

class DBusInterfaceWrapper;

/**
 * @brief Audit Manager for GUI Application
 * 
 * Manages audit operations and state for the GUI application.
 * Provides high-level interface for audit log management operations.
 */
class AuditManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool queryingEvents READ queryingEvents NOTIFY queryingEventsChanged)
    Q_PROPERTY(bool verifyingIntegrity READ verifyingIntegrity NOTIFY verifyingIntegrityChanged)
    Q_PROPERTY(bool exportingLogs READ exportingLogs NOTIFY exportingLogsChanged)
    Q_PROPERTY(QVariantList eventList READ eventList NOTIFY eventListChanged)
    Q_PROPERTY(QVariantMap statistics READ statistics NOTIFY statisticsChanged)
    Q_PROPERTY(bool integrityStatus READ integrityStatus NOTIFY integrityStatusChanged)
    Q_PROPERTY(QString healthStatus READ healthStatus NOTIFY healthStatusChanged)

public:
    explicit AuditManager(QObject* parent = nullptr);
    ~AuditManager();

    bool queryingEvents() const { return m_queryingEvents; }
    bool verifyingIntegrity() const { return m_verifyingIntegrity; }
    bool exportingLogs() const { return m_exportingLogs; }
    QVariantList eventList() const { return m_eventList; }
    QVariantMap statistics() const { return m_statistics; }
    bool integrityStatus() const { return m_integrityStatus; }
    QString healthStatus() const { return m_healthStatus; }

    void setDBusInterface(DBusInterfaceWrapper* dbusInterface);

public Q_SLOTS:
    /**
     * @brief Query events with filters
     * @param filters Filter parameters (timestamp, source, event_type, etc.)
     */
    Q_INVOKABLE void queryEvents(const QVariantMap& filters);

    /**
     * @brief Refresh event statistics
     * @param filters Optional filter parameters
     */
    Q_INVOKABLE void refreshStatistics(const QVariantMap& filters = QVariantMap());

    /**
     * @brief Export audit logs
     * @param timeRange Time range for export
     * @param format Export format (json, csv, xml)
     */
    Q_INVOKABLE void exportLogs(const QVariantMap& timeRange, const QString& format);

    /**
     * @brief Verify log integrity
     */
    Q_INVOKABLE void verifyIntegrity();

    /**
     * @brief Refresh health status
     */
    Q_INVOKABLE void refreshHealthStatus();

    /**
     * @brief Get event details
     * @param eventId Event ID
     * @return Event details as QVariantMap
     */
    Q_INVOKABLE QVariantMap getEventDetails(const QString& eventId);

Q_SIGNALS:
    void queryingEventsChanged(bool querying);
    void verifyingIntegrityChanged(bool verifying);
    void exportingLogsChanged(bool exporting);
    void eventListChanged(const QVariantList& events);
    void statisticsChanged(const QVariantMap& statistics);
    void integrityStatusChanged(bool verified);
    void healthStatusChanged(const QString& status);
    
    // Operation completion signals
    void queryCompleted(const QVariantList& events);
    void queryFailed(const QString& error);
    void statisticsUpdated(const QVariantMap& statistics);
    void statisticsFailed(const QString& error);
    void exportCompleted(const QString& filePath, const QString& format);
    void exportFailed(const QString& error);
    void integrityVerified(bool verified);
    void integrityViolationDetected();
    void healthStatusUpdated(const QVariantMap& healthStatus);

private Q_SLOTS:
    void onQueryCompleted(const QString& results);
    void onQueryFailed(const QString& error);
    void onStatisticsUpdated(const QString& statistics);
    void onStatisticsFailed(const QString& error);
    void onExportCompleted(const QString& data, const QString& format);
    void onExportFailed(const QString& error);
    void onIntegrityVerified(bool verified);
    void onIntegrityViolationDetected();
    void onHealthStatusUpdated(const QString& healthStatus);
    void onHealthPollTimeout();

private:
    void parseEventList(const QString& jsonResult);
    void parseStatistics(const QString& jsonResult);
    void parseHealthStatus(const QString& jsonResult);
    void startHealthPolling();
    void stopHealthPolling();
    QVariantMap jsonToVariantMap(const QString& jsonString);

    DBusInterfaceWrapper* m_dbusInterface;
    bool m_queryingEvents;
    bool m_verifyingIntegrity;
    bool m_exportingLogs;
    QVariantList m_eventList;
    QVariantMap m_statistics;
    bool m_integrityStatus;
    QString m_healthStatus;
    QTimer* m_healthPollTimer;
};

#endif // AUDIT_MANAGER_H

