#include "alertzoneservice.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QStandardPaths>
#include <QSettings>
#include <algorithm>

AlertZoneService::AlertZoneService(QObject* parent)
    : QObject(parent)
    , m_alertCount(0)
    , m_queueTimer(new QTimer(this))
    , m_deduplicationTimer(new QTimer(this))
    , m_dbusConnection(QDBusConnection::systemBus())
    , m_dataGuardInterface(nullptr)
{
    // Load enabled categories from config
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.ini", QSettings::IniFormat);
    settings.beginGroup("Filtering");
    m_enabledCategories = settings.value("enabledCategories", QStringList({"encryption_failures", "network_breaches", "blocked_transmissions", "hardening_violations"})).toStringList();
    settings.endGroup();
    
    // Setup queue processing timer
    m_queueTimer->setSingleShot(false);
    m_queueTimer->setInterval(100); // Process queue every 100ms
    connect(m_queueTimer, &QTimer::timeout, this, &AlertZoneService::processAlertQueue);
    m_queueTimer->start();
    
    // Setup deduplication timer
    m_deduplicationTimer->setSingleShot(false);
    m_deduplicationTimer->setInterval(5000); // Check deduplication every 5 seconds
    m_deduplicationTimer->start();
    
    // Subscribe to D-Bus signals
    subscribeToDBusSignals();
}

AlertZoneService::~AlertZoneService()
{
    unsubscribeFromDBusSignals();
    if (m_dataGuardInterface) {
        delete m_dataGuardInterface;
    }
}

void AlertZoneService::setEnabledCategories(const QStringList& categories)
{
    if (m_enabledCategories != categories) {
        m_enabledCategories = categories;
        
        // Save to config
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.ini", QSettings::IniFormat);
        settings.beginGroup("Filtering");
        settings.setValue("enabledCategories", categories);
        settings.endGroup();
        
        emit enabledCategoriesChanged();
    }
}

void AlertZoneService::addAlert(const QString& severity, const QString& category, const QString& message)
{
    Alert alert;
    alert.severity = severity;
    alert.category = category;
    alert.message = message;
    alert.timestamp = QDateTime::currentMSecsSinceEpoch();
    alert.source = "manual";
    alert.count = 1;
    
    m_alertQueue.enqueue(alert);
}

void AlertZoneService::addAlertWithData(const QString& severity, const QString& category, const QString& message, const QVariantMap& data)
{
    Alert alert;
    alert.severity = severity;
    alert.category = category;
    alert.message = message;
    alert.data = data;
    alert.timestamp = QDateTime::currentMSecsSinceEpoch();
    alert.source = data.value("source", "unknown").toString();
    alert.count = 1;
    
    m_alertQueue.enqueue(alert);
}

void AlertZoneService::clearAlerts()
{
    m_activeAlerts.clear();
    m_alertQueue.clear();
    m_alertCount = 0;
    emit alertsCleared();
    emit alertCountChanged(m_alertCount);
}

void AlertZoneService::subscribeToDBusSignals()
{
    connectToDataGuardService();
    connectToGUIApplicationSignals();
}

void AlertZoneService::unsubscribeFromDBusSignals()
{
    disconnectFromDataGuardService();
}

void AlertZoneService::processAlertQueue()
{
    while (!m_alertQueue.isEmpty()) {
        Alert alert = m_alertQueue.dequeue();
        
        // Check filtering
        if (!shouldDisplayAlert(alert)) {
            continue;
        }
        
        // Check deduplication
        if (isDuplicate(alert)) {
            // Merge with existing alert
            for (int i = 0; i < m_activeAlerts.size(); ++i) {
                if (m_activeAlerts[i].category == alert.category &&
                    m_activeAlerts[i].message == alert.message &&
                    m_activeAlerts[i].severity == alert.severity) {
                    mergeDuplicate(m_activeAlerts[i], alert);
                    break;
                }
            }
        } else {
            // Add new alert
            m_activeAlerts.append(alert);
            processAlert(alert);
        }
    }
    
    // Sort alerts by priority
    sortAlertsByPriority();
}

void AlertZoneService::onTransmissionBlocked(const QString& transmissionInfo)
{
    Alert alert = parseAlertFromDBus("TransmissionBlocked", transmissionInfo);
    m_alertQueue.enqueue(alert);
}

void AlertZoneService::onPolicyViolationDetected(const QString& violationInfo)
{
    Alert alert = parseAlertFromDBus("PolicyViolationDetected", violationInfo);
    m_alertQueue.enqueue(alert);
}

void AlertZoneService::onServiceRegistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner)
{
    Q_UNUSED(oldOwner)
    if (serviceName == "org.milos.DataGuard" && !newOwner.isEmpty()) {
        connectToDataGuardService();
    }
}

void AlertZoneService::onServiceUnregistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner)
{
    Q_UNUSED(oldOwner)
    Q_UNUSED(newOwner)
    if (serviceName == "org.milos.DataGuard") {
        disconnectFromDataGuardService();
    }
}

void AlertZoneService::processAlert(const Alert& alert)
{
    m_alertCount++;
    emit alertReceived(alert.severity, alert.category, alert.message);
    emit alertCountChanged(m_alertCount);
}

AlertZoneService::Alert AlertZoneService::parseAlertFromDBus(const QString& signalName, const QString& jsonData)
{
    Alert alert;
    alert.source = "data_guard";
    alert.timestamp = QDateTime::currentMSecsSinceEpoch();
    alert.count = 1;
    
    // Parse JSON data
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QVariantMap data = obj.toVariantMap();
        alert.data = data;
        
        alert.severity = extractSeverity(data);
        alert.category = extractCategory(data, signalName);
        alert.message = extractMessage(data, signalName);
    } else {
        // Fallback if JSON parsing fails
        alert.severity = "medium";
        alert.category = signalName == "TransmissionBlocked" ? "blocked_transmissions" : "hardening_violations";
        alert.message = jsonData;
    }
    
    return alert;
}

QString AlertZoneService::extractSeverity(const QVariantMap& data)
{
    QString severity = data.value("severity", "medium").toString().toLower();
    if (severity == "critical" || severity == "high" || severity == "medium" || severity == "low") {
        return severity;
    }
    
    // Determine severity from other fields
    if (data.value("blocked", false).toBool() || data.value("violation", false).toBool()) {
        return "high";
    }
    return "medium";
}

QString AlertZoneService::extractCategory(const QVariantMap& data, const QString& signalName)
{
    if (signalName == "TransmissionBlocked") {
        return "blocked_transmissions";
    } else if (signalName == "PolicyViolationDetected") {
        QString violationType = data.value("violation_type", "").toString().toLower();
        if (violationType.contains("encryption")) {
            return "encryption_failures";
        } else if (violationType.contains("network")) {
            return "network_breaches";
        } else {
            return "hardening_violations";
        }
    }
    return "unknown";
}

QString AlertZoneService::extractMessage(const QVariantMap& data, const QString& signalName)
{
    QString message = data.value("message", "").toString();
    if (message.isEmpty()) {
        if (signalName == "TransmissionBlocked") {
            QString destination = data.value("destination", "unknown").toString();
            message = QString("Transmission blocked to %1").arg(destination);
        } else if (signalName == "PolicyViolationDetected") {
            QString policyId = data.value("policy_id", "unknown").toString();
            message = QString("Policy violation detected: %1").arg(policyId);
        }
    }
    return message;
}

bool AlertZoneService::isDuplicate(const Alert& alert)
{
    for (const Alert& existing : m_activeAlerts) {
        if (existing.category == alert.category &&
            existing.message == alert.message &&
            existing.severity == alert.severity &&
            (existing.timestamp - alert.timestamp) < 10000) { // Within 10 seconds
            return true;
        }
    }
    return false;
}

void AlertZoneService::mergeDuplicate(Alert& existing, const Alert& newAlert)
{
    existing.count++;
    existing.timestamp = newAlert.timestamp; // Update timestamp
    // Merge data
    for (auto it = newAlert.data.begin(); it != newAlert.data.end(); ++it) {
        existing.data[it.key()] = it.value();
    }
}

int AlertZoneService::getSeverityLevel(const QString& severity)
{
    if (severity == "critical") return 4;
    if (severity == "high") return 3;
    if (severity == "medium") return 2;
    if (severity == "low") return 1;
    return 0;
}

void AlertZoneService::sortAlertsByPriority()
{
    std::sort(m_activeAlerts.begin(), m_activeAlerts.end(), [this](const Alert& a, const Alert& b) {
        int levelA = getSeverityLevel(a.severity);
        int levelB = getSeverityLevel(b.severity);
        if (levelA != levelB) {
            return levelA > levelB; // Higher severity first
        }
        return a.timestamp > b.timestamp; // Newer first within same severity
    });
}

bool AlertZoneService::shouldDisplayAlert(const Alert& alert)
{
    // Check if category is enabled
    if (!m_enabledCategories.contains(alert.category)) {
        return false;
    }
    return true;
}

void AlertZoneService::connectToDataGuardService()
{
    if (m_dataGuardInterface) {
        return; // Already connected
    }
    
    m_dataGuardInterface = new QDBusInterface(
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
        m_dbusConnection,
        this
    );
    
    if (!m_dataGuardInterface->isValid()) {
        qWarning() << "Cannot connect to Data Guard service:" << m_dataGuardInterface->lastError().message();
        delete m_dataGuardInterface;
        m_dataGuardInterface = nullptr;
        return;
    }
    
    // Connect to signals
    m_dbusConnection.connect(
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
        "TransmissionBlocked",
        this,
        SLOT(onTransmissionBlocked(QString))
    );
    
    m_dbusConnection.connect(
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
        "PolicyViolationDetected",
        this,
        SLOT(onPolicyViolationDetected(QString))
    );
    
    // Monitor service registration
    m_dbusConnection.connect(
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus",
        "NameOwnerChanged",
        QStringList() << "org.milos.DataGuard",
        "sss",
        this,
        SLOT(onServiceRegistered(QString,QString,QString))
    );
    
    qDebug() << "Connected to Data Guard service";
}

void AlertZoneService::disconnectFromDataGuardService()
{
    if (m_dataGuardInterface) {
        m_dbusConnection.disconnect(
            "org.milos.DataGuard",
            "/org/milos/DataGuard",
            "org.milos.DataGuard",
            "TransmissionBlocked",
            this,
            SLOT(onTransmissionBlocked(QString))
        );
        
        m_dbusConnection.disconnect(
            "org.milos.DataGuard",
            "/org/milos/DataGuard",
            "org.milos.DataGuard",
            "PolicyViolationDetected",
            this,
            SLOT(onPolicyViolationDetected(QString))
        );
        
        delete m_dataGuardInterface;
        m_dataGuardInterface = nullptr;
    }
}

void AlertZoneService::connectToGUIApplicationSignals()
{
    // Connect to GUI application alert signals
    // Network Defense Dashboard
    m_dbusConnection.connect(
        "",
        "/org/milos/NetworkDefense",
        "org.milos.NetworkDefense",
        "AlertEmitted",
        this,
        SLOT(onPolicyViolationDetected(QString))
    );
    
    // Encryption Manager
    m_dbusConnection.connect(
        "",
        "/org/milos/EncryptionManager",
        "org.milos.EncryptionManager",
        "AlertEmitted",
        this,
        SLOT(onPolicyViolationDetected(QString))
    );
}
