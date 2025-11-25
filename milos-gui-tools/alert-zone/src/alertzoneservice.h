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

public:
    explicit AlertZoneService(QObject* parent = nullptr);
    ~AlertZoneService();

    int alertCount() const { return m_alertCount; }
    QStringList enabledCategories() const { return m_enabledCategories; }
    void setEnabledCategories(const QStringList& categories);

public Q_SLOTS:
    Q_INVOKABLE void addAlert(const QString& severity, const QString& category, const QString& message);
    Q_INVOKABLE void addAlertWithData(const QString& severity, const QString& category, const QString& message, const QVariantMap& data);
    Q_INVOKABLE void clearAlerts();
    Q_INVOKABLE void subscribeToDBusSignals();
    Q_INVOKABLE void unsubscribeFromDBusSignals();
    Q_INVOKABLE void processAlertQueue();

Q_SIGNALS:
    void alertReceived(const QString& severity, const QString& category, const QString& message);
    void alertCountChanged(int count);
    void alertsCleared();
    void enabledCategoriesChanged();

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
    };
    
    int m_alertCount;
    QStringList m_enabledCategories;
    QQueue<Alert> m_alertQueue;
    QList<Alert> m_activeAlerts;
    QTimer* m_queueTimer;
    QTimer* m_deduplicationTimer;
    
    // D-Bus interfaces
    QDBusConnection m_dbusConnection;
    QDBusInterface* m_dataGuardInterface;
    
    // Alert processing
    void processAlert(const Alert& alert);
    Alert parseAlertFromDBus(const QString& signalName, const QString& jsonData);
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
    
    // D-Bus connection
    void connectToDataGuardService();
    void disconnectFromDataGuardService();
    void connectToGUIApplicationSignals();
};

#endif // ALERTZONESERVICE_H

