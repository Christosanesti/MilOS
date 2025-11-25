#ifndef ALERTZONESERVICE_H
#define ALERTZONESERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QQueue>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusInterface>

/**
 * @brief Service for managing alert zone alerts
 * 
 * Provides D-Bus interface for receiving and managing alerts
 * in the critical alert zone widget. Handles D-Bus signal subscription,
 * alert processing, deduplication, prioritization, and filtering.
 */
class AlertZoneService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int alertCount READ alertCount NOTIFY alertCountChanged)
    Q_PROPERTY(QStringList enabledCategories READ enabledCategories WRITE setEnabledCategories NOTIFY enabledCategoriesChanged)
    Q_PROPERTY(int escalationTimeout READ escalationTimeout WRITE setEscalationTimeout NOTIFY escalationTimeoutChanged)
    Q_PROPERTY(int acknowledgmentTimeout READ acknowledgmentTimeout WRITE setAcknowledgmentTimeout NOTIFY acknowledgmentTimeoutChanged)

public:
    explicit AlertZoneService(QObject* parent = nullptr);
    ~AlertZoneService();

    int alertCount() const { return m_alertCount; }
    QStringList enabledCategories() const { return m_enabledCategories; }
    void setEnabledCategories(const QStringList& categories);
    int escalationTimeout() const { return m_escalationTimeout; }
    void setEscalationTimeout(int timeout);
    int acknowledgmentTimeout() const { return m_acknowledgmentTimeout; }
    void setAcknowledgmentTimeout(int timeout);

public Q_SLOTS:
    Q_INVOKABLE void addAlert(const QString& severity, const QString& category, const QString& message);
    Q_INVOKABLE void addAlertWithData(const QString& severity, const QString& category, const QString& message, const QVariantMap& data);
    Q_INVOKABLE void clearAlerts();
    Q_INVOKABLE void subscribeToDBusSignals();
    Q_INVOKABLE void unsubscribeFromDBusSignals();
    Q_INVOKABLE void processAlertQueue();
    Q_INVOKABLE void acknowledgeAlert(const QString& alertId);
    Q_INVOKABLE void navigateToAlertSource(const QString& alertId);
    Q_INVOKABLE QVariantMap getAlert(const QString& alertId);
    Q_INVOKABLE QVariantList getActiveAlerts();

Q_SIGNALS:
    void alertReceived(const QString& severity, const QString& category, const QString& message);
    void alertCountChanged(int count);
    void alertsCleared();
    void enabledCategoriesChanged();
    void escalationTimeoutChanged();
    void acknowledgmentTimeoutChanged();
    void alertAcknowledged(const QString& alertId);
    void alertEscalated(const QString& alertId, int escalationLevel);

private slots:
    void onTransmissionBlocked(const QString& transmissionInfo);
    void onPolicyViolationDetected(const QString& violationInfo);
    void onServiceRegistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner);
    void onServiceUnregistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner);

private:
    struct Alert {
        QString severity;
        QString category;
        QString message;
        QVariantMap data;
        qint64 timestamp;
        QString source;
        int count; // For deduplication
        bool acknowledged;
        qint64 acknowledgmentTimestamp;
        int escalationLevel; // 1-4 (subtle, standard, intense, maximum)
        qint64 escalationStartTime;
        QString alertId; // Unique ID for tracking
    };
    
    int m_alertCount;
    QStringList m_enabledCategories;
    int m_escalationTimeout; // milliseconds
    int m_acknowledgmentTimeout; // milliseconds
    QQueue<Alert> m_alertQueue;
    QList<Alert> m_activeAlerts;
    QTimer* m_queueTimer;
    QTimer* m_deduplicationTimer;
    QTimer* m_escalationTimer;
    QTimer* m_acknowledgmentTimer;
    
    // D-Bus interfaces
    QDBusConnection m_dbusConnection;
    QDBusInterface* m_dataGuardInterface;
    
    // Alert processing
    void processAlert(const Alert& alert);
    Alert parseAlertFromDBus(const QString& signalName, const QString& jsonData);
    
    // Configuration persistence
    void saveConfigurationToYAML();
    void loadConfigurationFromYAML();
    QString extractSeverity(const QVariantMap& data);
    QString extractCategory(const QVariantMap& data, const QString& signalName);
    QString extractMessage(const QVariantMap& data, const QString& signalName);
    
    // Deduplication
    bool isDuplicate(const Alert& alert);
    void mergeDuplicate(Alert& existing, const Alert& newAlert);
    
    // Prioritization
    int getSeverityLevel(const QString& severity);
    void sortAlertsByPriority();
    
    // Filtering
    bool shouldDisplayAlert(const Alert& alert);
    
    // Escalation
    void updateEscalationLevels();
    int calculateEscalationLevel(const Alert& alert);
    QString generateAlertId();
    
    // Acknowledgment
    void logAcknowledgmentToAudit(const QString& alertId, const Alert& alert);
    void autoAcknowledgeLowSeverityAlerts();
    
    // Navigation
    QString getNavigationTarget(const QString& category);
    void launchApplication(const QString& application, const QVariantMap& context);
    
    // D-Bus connection
    void connectToDataGuardService();
    void disconnectFromDataGuardService();
    void connectToGUIApplicationSignals();
    QDBusInterface* m_auditInterface;
};

#endif // ALERTZONESERVICE_H

