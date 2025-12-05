#ifndef DBUS_TEST_HARNESS_H
#define DBUS_TEST_HARNESS_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>

/**
 * @brief D-Bus Test Harness
 * 
 * Provides mock D-Bus services for testing without requiring
 * actual systemd services or D-Bus daemon.
 * 
 * Usage:
 *   DBusTestHarness harness;
 *   harness.startMockService("org.milos.AuditService");
 *   // Test code that uses D-Bus
 *   harness.stopMockService();
 */
class DBusTestHarness : public QObject
{
    Q_OBJECT

public:
    explicit DBusTestHarness(QObject* parent = nullptr);
    ~DBusTestHarness();

    /**
     * @brief Start mock D-Bus service
     * @param serviceName Service name (e.g., "org.milos.AuditService")
     * @param objectPath Object path (e.g., "/org/milos/AuditService")
     * @return true if started successfully
     */
    bool startMockService(const QString& serviceName, 
                          const QString& objectPath = "");

    /**
     * @brief Stop mock D-Bus service
     * @param serviceName Service name to stop
     */
    void stopMockService(const QString& serviceName);

    /**
     * @brief Check if service is available
     * @param serviceName Service name
     * @return true if available
     */
    bool isServiceAvailable(const QString& serviceName) const;

    /**
     * @brief Set mock response for method call
     * @param serviceName Service name
     * @param methodName Method name
     * @param response Response to return
     */
    void setMockResponse(const QString& serviceName,
                        const QString& methodName,
                        const QVariant& response);

    /**
     * @brief Get call history for method
     * @param serviceName Service name
     * @param methodName Method name
     * @return List of call arguments
     */
    QList<QVariantList> getCallHistory(const QString& serviceName,
                                      const QString& methodName) const;

    /**
     * @brief Clear call history
     */
    void clearCallHistory();

    /**
     * @brief Simulate service unavailable
     * @param serviceName Service name
     */
    void simulateServiceUnavailable(const QString& serviceName);

    /**
     * @brief Simulate service error
     * @param serviceName Service name
     * @param errorMessage Error message
     */
    void simulateServiceError(const QString& serviceName,
                             const QString& errorMessage);

private slots:
    void onMethodCall(const QDBusMessage& message);

private:
    struct MockService {
        MockService() : connection(QDBusConnection::sessionBus()), available(false) {}
        
        QString serviceName;
        QString objectPath;
        QString interface;
        QDBusConnection connection;
        QMap<QString, QVariant> responses;
        QMap<QString, QList<QVariantList>> callHistory;
        bool available;
        QString errorMessage;
    };

    QMap<QString, MockService> m_mockServices;
    QString m_sessionBusName;
    
    QString generateObjectPath(const QString& serviceName) const;
    QString generateInterface(const QString& serviceName) const;
};

#endif // DBUS_TEST_HARNESS_H

