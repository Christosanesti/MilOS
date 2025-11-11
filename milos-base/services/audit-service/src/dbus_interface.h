#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class ConfigParser;
class LogStorage;
class HashChain;
class QueryEngine;
class EventCollector;

/**
 * @brief D-Bus Interface for Audit Service
 * 
 * Implements org.milos.AuditService D-Bus interface with methods
 * and signals for audit logging and query operations.
 * Uses Qt D-Bus adaptor pattern.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.AuditService")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param configParser Configuration parser instance
     * @param logStorage Log storage instance
     * @param hashChain Hash chain instance
     * @param queryEngine Query engine instance
     * @param eventCollector Event collector instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        LogStorage* logStorage,
        HashChain* hashChain,
        QueryEngine* queryEngine,
        EventCollector* eventCollector
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
     * @brief Log a security event
     * @param eventData Event data (JSON string)
     * @return Event ID if logged successfully, empty string otherwise
     */
    QString LogEvent(const QString& eventData);

    /**
     * @brief Query audit logs
     * @param queryParams Query parameters (JSON string)
     * @return Query results (JSON string)
     */
    QString QueryEvents(const QString& queryParams);

    /**
     * @brief Get event statistics
     * @param queryParams Query parameters (JSON string)
     * @return Statistics (JSON string)
     */
    QString GetEventStatistics(const QString& queryParams);

    /**
     * @brief Export audit logs
     * @param timeRange Time range (JSON string)
     * @param format Export format (json, csv, xml)
     * @return Exported data (string)
     */
    QString ExportAuditLog(const QString& timeRange, const QString& format);

    /**
     * @brief Verify log integrity
     * @return true if integrity verified, false if tampering detected
     */
    bool VerifyLogIntegrity();

    /**
     * @brief Get detailed health status of the service
     * @return JSON string with health status information
     */
    QString GetHealthStatus();

Q_SIGNALS:
    /**
     * @brief Signal emitted when an event is logged
     * @param eventId Event ID
     */
    void EventLogged(const QString& eventId);

    /**
     * @brief Signal emitted when integrity violation is detected
     */
    void IntegrityViolationDetected();

private:
    bool m_running;
    bool m_initialized;

    ConfigParser* m_configParser;
    LogStorage* m_logStorage;
    HashChain* m_hashChain;
    QueryEngine* m_queryEngine;
    EventCollector* m_eventCollector;

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

