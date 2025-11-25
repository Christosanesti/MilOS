#include "alertzoneservice.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QStandardPaths>
#include <QSettings>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <algorithm>
#include <cstdlib>

AlertZoneService::AlertZoneService(QObject* parent)
    : QObject(parent)
    , m_alertCount(0)
    , m_escalationTimeout(30000) // 30 seconds default
    , m_acknowledgmentTimeout(300000) // 5 minutes default
    , m_queueTimer(new QTimer(this))
    , m_deduplicationTimer(new QTimer(this))
    , m_escalationTimer(new QTimer(this))
    , m_acknowledgmentTimer(new QTimer(this))
    , m_dbusConnection(QDBusConnection::systemBus())
    , m_dataGuardInterface(nullptr)
    , m_auditInterface(nullptr)
{
    // Load configuration - try YAML first, fallback to INI
    bool yamlLoaded = false;
    QString yamlPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.yaml";
    if (QFile::exists(yamlPath)) {
        loadConfigurationFromYAML();
        yamlLoaded = true;
    }
    
    // If YAML doesn't exist or didn't load properly, load from INI
    if (!yamlLoaded) {
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.ini", QSettings::IniFormat);
        settings.beginGroup("Filtering");
        m_enabledCategories = settings.value("enabledCategories", QStringList({"encryption_failures", "network_breaches", "blocked_transmissions", "hardening_violations"})).toStringList();
        settings.endGroup();
        
        settings.beginGroup("Escalation");
        m_escalationTimeout = settings.value("timeout", 30000).toInt();
        settings.endGroup();
        
        settings.beginGroup("Acknowledgment");
        m_acknowledgmentTimeout = settings.value("timeout", 300000).toInt();
        settings.endGroup();
    }
    
    // Setup queue processing timer
    m_queueTimer->setSingleShot(false);
    m_queueTimer->setInterval(100); // Process queue every 100ms
    connect(m_queueTimer, &QTimer::timeout, this, &AlertZoneService::processAlertQueue);
    m_queueTimer->start();
    
    // Setup deduplication timer
    m_deduplicationTimer->setSingleShot(false);
    m_deduplicationTimer->setInterval(5000); // Check deduplication every 5 seconds
    m_deduplicationTimer->start();
    
    // Setup escalation timer
    m_escalationTimer->setSingleShot(false);
    m_escalationTimer->setInterval(1000); // Check escalation every second
    connect(m_escalationTimer, &QTimer::timeout, this, &AlertZoneService::updateEscalationLevels);
    m_escalationTimer->start();
    
    // Setup acknowledgment timer
    m_acknowledgmentTimer->setSingleShot(false);
    m_acknowledgmentTimer->setInterval(10000); // Check auto-acknowledgment every 10 seconds
    connect(m_acknowledgmentTimer, &QTimer::timeout, this, &AlertZoneService::autoAcknowledgeLowSeverityAlerts);
    m_acknowledgmentTimer->start();
    
    // Connect to audit service
    m_auditInterface = new QDBusInterface(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        m_dbusConnection,
        this
    );
    
    // Subscribe to D-Bus signals
    subscribeToDBusSignals();
}

AlertZoneService::~AlertZoneService()
{
    unsubscribeFromDBusSignals();
    if (m_dataGuardInterface) {
        delete m_dataGuardInterface;
    }
    if (m_auditInterface) {
        delete m_auditInterface;
    }
}

void AlertZoneService::setEnabledCategories(const QStringList& categories)
{
    if (m_enabledCategories != categories) {
        m_enabledCategories = categories;
        
        // Save to config (both INI and YAML)
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.ini", QSettings::IniFormat);
        settings.beginGroup("Filtering");
        settings.setValue("enabledCategories", categories);
        settings.endGroup();
        
        saveConfigurationToYAML();
        
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
    alert.acknowledged = false;
    alert.escalationLevel = 1;
    alert.escalationStartTime = alert.timestamp;
    alert.alertId = generateAlertId();
    
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
    alert.acknowledged = false;
    alert.escalationLevel = 1;
    alert.escalationStartTime = alert.timestamp;
    alert.alertId = generateAlertId();
    
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
        alert.acknowledged = false;
        alert.escalationLevel = 1;
        alert.escalationStartTime = alert.timestamp;
        alert.alertId = generateAlertId();
    
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

void AlertZoneService::setEscalationTimeout(int timeout)
{
    if (m_escalationTimeout != timeout) {
        m_escalationTimeout = timeout;
        
        // Save to config (both INI and YAML)
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.ini", QSettings::IniFormat);
        settings.beginGroup("Escalation");
        settings.setValue("timeout", timeout);
        settings.endGroup();
        
        saveConfigurationToYAML();
        
        emit escalationTimeoutChanged();
    }
}

void AlertZoneService::setAcknowledgmentTimeout(int timeout)
{
    if (m_acknowledgmentTimeout != timeout) {
        m_acknowledgmentTimeout = timeout;
        
        // Save to config (both INI and YAML)
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.ini", QSettings::IniFormat);
        settings.beginGroup("Acknowledgment");
        settings.setValue("timeout", timeout);
        settings.endGroup();
        
        saveConfigurationToYAML();
        
        emit acknowledgmentTimeoutChanged();
    }
}

QString AlertZoneService::generateAlertId()
{
    return QString("alert_%1_%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(qrand());
}

void AlertZoneService::acknowledgeAlert(const QString& alertId)
{
    for (int i = 0; i < m_activeAlerts.size(); ++i) {
        if (m_activeAlerts[i].alertId == alertId) {
            m_activeAlerts[i].acknowledged = true;
            m_activeAlerts[i].acknowledgmentTimestamp = QDateTime::currentMSecsSinceEpoch();
            
            // Log to audit service
            logAcknowledgmentToAudit(alertId, m_activeAlerts[i]);
            
            emit alertAcknowledged(alertId);
            return;
        }
    }
}

void AlertZoneService::navigateToAlertSource(const QString& alertId)
{
    for (const Alert& alert : m_activeAlerts) {
        if (alert.alertId == alertId) {
            QString target = getNavigationTarget(alert.category);
            QVariantMap context;
            context["alertId"] = alertId;
            context["category"] = alert.category;
            context["severity"] = alert.severity;
            context["message"] = alert.message;
            launchApplication(target, context);
            return;
        }
    }
}

QVariantMap AlertZoneService::getAlert(const QString& alertId)
{
    for (const Alert& alert : m_activeAlerts) {
        if (alert.alertId == alertId) {
            QVariantMap result;
            result["alertId"] = alert.alertId;
            result["severity"] = alert.severity;
            result["category"] = alert.category;
            result["message"] = alert.message;
            result["timestamp"] = alert.timestamp;
            result["acknowledged"] = alert.acknowledged;
            result["escalationLevel"] = alert.escalationLevel;
            result["data"] = alert.data;
            return result;
        }
    }
    return QVariantMap();
}

QVariantList AlertZoneService::getActiveAlerts()
{
    QVariantList result;
    for (const Alert& alert : m_activeAlerts) {
        QVariantMap alertMap;
        alertMap["alertId"] = alert.alertId;
        alertMap["severity"] = alert.severity;
        alertMap["category"] = alert.category;
        alertMap["message"] = alert.message;
        alertMap["timestamp"] = alert.timestamp;
        alertMap["acknowledged"] = alert.acknowledged;
        alertMap["escalationLevel"] = alert.escalationLevel;
        alertMap["count"] = alert.count;
        result.append(alertMap);
    }
    return result;
}

void AlertZoneService::updateEscalationLevels()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    for (int i = 0; i < m_activeAlerts.size(); ++i) {
        if (m_activeAlerts[i].acknowledged) {
            continue; // Don't escalate acknowledged alerts
        }
        
        int newLevel = calculateEscalationLevel(m_activeAlerts[i]);
        if (newLevel != m_activeAlerts[i].escalationLevel) {
            m_activeAlerts[i].escalationLevel = newLevel;
            emit alertEscalated(m_activeAlerts[i].alertId, newLevel);
        }
    }
}

int AlertZoneService::calculateEscalationLevel(const Alert& alert)
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 timeSinceCreation = currentTime - alert.timestamp;
    
    // Escalation levels based on time
    if (timeSinceCreation < m_escalationTimeout) {
        return 1; // Subtle
    } else if (timeSinceCreation < m_escalationTimeout * 2) {
        return 2; // Standard
    } else if (timeSinceCreation < m_escalationTimeout * 3) {
        return 3; // Intense
    } else {
        return 4; // Maximum
    }
}

void AlertZoneService::autoAcknowledgeLowSeverityAlerts()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    for (int i = 0; i < m_activeAlerts.size(); ++i) {
        if (!m_activeAlerts[i].acknowledged &&
            m_activeAlerts[i].severity == "low" &&
            (currentTime - m_activeAlerts[i].timestamp) > m_acknowledgmentTimeout) {
            acknowledgeAlert(m_activeAlerts[i].alertId);
        }
    }
}

void AlertZoneService::logAcknowledgmentToAudit(const QString& alertId, const Alert& alert)
{
    if (!m_auditInterface || !m_auditInterface->isValid()) {
        qWarning() << "Cannot connect to audit service for acknowledgment logging";
        return;
    }
    
    QVariantMap eventData;
    eventData["event_type"] = "alert_acknowledged";
    eventData["alert_id"] = alertId;
    eventData["alert_category"] = alert.category;
    eventData["alert_severity"] = alert.severity;
    eventData["alert_message"] = alert.message;
    eventData["acknowledgment_timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    eventData["user"] = qgetenv("USER");
    
    QJsonDocument doc = QJsonDocument::fromVariant(eventData);
    QString jsonData = QString::fromUtf8(doc.toJson());
    
    m_auditInterface->call("LogEvent", "alert_acknowledgment", jsonData);
}

QString AlertZoneService::getNavigationTarget(const QString& category)
{
    if (category == "encryption_failures") {
        return "milos-encryption-manager";
    } else if (category == "network_breaches") {
        return "milos-network-dashboard";
    } else if (category == "blocked_transmissions") {
        return "milos-data-guard-gui";
    } else if (category == "hardening_violations") {
        return "systemsettings"; // KDE System Settings
    }
    return "";
}

void AlertZoneService::launchApplication(const QString& application, const QVariantMap& context)
{
    QProcess* process = new QProcess(this);
    
    if (application == "systemsettings") {
        // Launch KDE System Settings with security section
        process->start("systemsettings5", QStringList() << "--args" << "security");
    } else {
        // Launch MilOS application
        QString appPath = QString("/usr/bin/%1").arg(application);
        if (QFile::exists(appPath)) {
            process->start(appPath);
        } else {
            // Try desktop file
            QString desktopFile = QString("/usr/share/applications/%1.desktop").arg(application);
            if (QFile::exists(desktopFile)) {
                process->start("gtk-launch", QStringList() << application);
            } else {
                qWarning() << "Cannot find application:" << application;
                delete process;
                return;
            }
        }
    }
    
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            process, &QProcess::deleteLater);
}

void AlertZoneService::saveConfigurationToYAML()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos";
    QDir().mkpath(configDir);
    
    QString yamlPath = configDir + "/alert-zone.yaml";
    QFile file(yamlPath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open YAML config file for writing:" << yamlPath;
        return;
    }
    
    QTextStream out(&file);
    out << "alert_zone:\n";
    out << "  filtering:\n";
    out << "    enabled_categories:\n";
    for (const QString& category : m_enabledCategories) {
        out << "      - " << category << "\n";
    }
    out << "  escalation:\n";
    out << "    timeout_ms: " << m_escalationTimeout << "\n";
    out << "  acknowledgment:\n";
    out << "    timeout_ms: " << m_acknowledgmentTimeout << "\n";
    
    file.close();
}

void AlertZoneService::loadConfigurationFromYAML()
{
    QString yamlPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/alert-zone.yaml";
    QFile file(yamlPath);
    
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // YAML file doesn't exist, use defaults
        return;
    }
    
    QTextStream in(&file);
    QStringList enabledCategories;
    bool inFiltering = false;
    bool inCategories = false;
    bool inEscalation = false;
    bool inAcknowledgment = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        QString trimmed = line.trimmed();
        
        // Skip empty lines and comments
        if (trimmed.isEmpty() || trimmed.startsWith("#")) {
            continue;
        }
        
        // Detect sections
        if (trimmed == "filtering:") {
            inFiltering = true;
            inEscalation = false;
            inAcknowledgment = false;
            continue;
        } else if (trimmed == "escalation:") {
            inEscalation = true;
            inFiltering = false;
            inAcknowledgment = false;
            continue;
        } else if (trimmed == "acknowledgment:") {
            inAcknowledgment = true;
            inFiltering = false;
            inEscalation = false;
            continue;
        }
        
        // Parse enabled_categories
        if (inFiltering && trimmed.startsWith("enabled_categories:")) {
            inCategories = true;
            continue;
        }
        
        if (inCategories) {
            if (trimmed.startsWith("- ")) {
                QString category = trimmed.mid(2).trimmed();
                enabledCategories.append(category);
            } else if (!trimmed.startsWith(" ") && !trimmed.startsWith("\t")) {
                // End of categories list
                inCategories = false;
            }
        }
        
        // Parse timeout_ms
        if (trimmed.startsWith("timeout_ms:")) {
            QString valueStr = trimmed.mid(trimmed.indexOf(":") + 1).trimmed();
            bool ok;
            int timeout = valueStr.toInt(&ok);
            
            if (ok) {
                if (inEscalation) {
                    m_escalationTimeout = timeout;
                } else if (inAcknowledgment) {
                    m_acknowledgmentTimeout = timeout;
                }
            }
        }
    }
    
    if (!enabledCategories.isEmpty()) {
        m_enabledCategories = enabledCategories;
    }
    
    file.close();
}
