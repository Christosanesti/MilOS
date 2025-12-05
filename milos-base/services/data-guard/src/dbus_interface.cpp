#include "dbus_interface.h"
#include "config_parser.h"
#include "policy_manager.h"
#include "network_enforcement.h"
#include <milos/logging/logger.h>
#include <QDBusConnection>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_configParser(nullptr)
    , m_policyManager(nullptr)
    , m_networkEnforcement(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(
    ConfigParser* configParser,
    PolicyManager* policyManager,
    NetworkEnforcement* networkEnforcement
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_policyManager = policyManager;
    m_networkEnforcement = networkEnforcement;

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        LOG_ERROR("D-Bus interface not initialized");
        return false;
    }

    if (m_running) {
        return true;
    }

    // Register D-Bus interface
    if (!registerInterface()) {
        LOG_ERROR("Failed to register D-Bus interface");
        return false;
    }

    m_running = true;
    LOG_INFO("D-Bus interface started");
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    LOG_INFO("D-Bus interface stopped");
}

bool DBusInterface::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check D-Bus connection health
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        return false;
    }

    return true;
}

QString DBusInterface::GetTransmissionStatus() {
    QJsonObject status;
    
    if (!m_networkEnforcement) {
        status["status"] = "error";
        status["message"] = "Network enforcement not available";
    } else {
        status["status"] = m_networkEnforcement->isRunning() ? "running" : "stopped";
        status["blocked"] = static_cast<qint64>(m_networkEnforcement->getBlockedCount());
        status["allowed"] = static_cast<qint64>(m_networkEnforcement->getAllowedCount());
        status["healthy"] = m_networkEnforcement->isHealthy();
    }

    QJsonDocument doc(status);
    return QString::fromUtf8(doc.toJson());
}

QStringList DBusInterface::GetBlockedTransmissions() {
    QStringList blocked;
    
    if (!m_networkEnforcement) {
        return blocked;
    }
    
    // Retrieve actual blocked transmission list from network enforcement
    std::vector<std::string> blockedList = m_networkEnforcement->getBlockedTransmissions();
    for (const auto& transmission : blockedList) {
        blocked.append(QString::fromStdString(transmission));
    }
    
    return blocked;
}

bool DBusInterface::ConfigurePolicy(const QString& policy) {
    if (!m_policyManager) {
        LOG_ERROR("Policy manager not available");
        return false;
    }

    // Parse policy JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(policy.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("Failed to parse policy JSON: %1").arg(error.errorString()));
        return false;
    }

    if (!doc.isObject()) {
        LOG_ERROR("Policy JSON is not an object");
        return false;
    }

    QJsonObject policyObj = doc.object();
    
    // Create NetworkPolicy from JSON
    NetworkPolicy policy;
    
    // Required fields
    if (!policyObj.contains("policy_id") || !policyObj.contains("policy_name") || 
        !policyObj.contains("policy_type")) {
        LOG_ERROR("Missing required policy fields (policy_id, policy_name, policy_type)");
        return false;
    }
    
    policy.policy_id = policyObj["policy_id"].toString().toStdString();
    policy.policy_name = policyObj["policy_name"].toString().toStdString();
    policy.policy_type = policyObj["policy_type"].toString().toStdString();
    policy.enabled = policyObj.value("enabled").toBool(true);
    policy.priority = policyObj.value("priority").toInt(100);
    
    // Parse rules
    if (policyObj.contains("rules") && policyObj["rules"].isArray()) {
        QJsonArray rulesArray = policyObj["rules"].toArray();
        for (const QJsonValue& ruleValue : rulesArray) {
            if (!ruleValue.isObject()) {
                continue;
            }
            
            QJsonObject ruleObj = ruleValue.toObject();
            PolicyRule rule;
            
            rule.rule_id = ruleObj.value("rule_id").toString("").toStdString();
            rule.source = ruleObj.value("source").toString("").toStdString();
            rule.destination = ruleObj.value("destination").toString("").toStdString();
            rule.protocol = ruleObj.value("protocol").toString("").toStdString();
            rule.port = ruleObj.value("port").toInt(-1);
            rule.encryption_required = ruleObj.value("encryption_required").toBool(false);
            rule.action = ruleObj.value("action").toString("ALLOW").toStdString();
            
            policy.rules.push_back(rule);
        }
    }
    
    // Add timestamps
    policy.created_at = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
    policy.updated_at = policy.created_at;
    
    // Add or update policy in policy manager
    if (!m_policyManager->addOrUpdatePolicy(policy)) {
        LOG_ERROR("Failed to add/update policy");
        return false;
    }
    
    // Apply policy
    if (!m_policyManager->applyPolicy(policy.policy_id)) {
        LOG_ERROR("Failed to apply policy");
        return false;
    }
    
    return true;
}

QString DBusInterface::GetPolicyStatus(const QString& policyId) {
    QJsonObject status;
    
    if (!m_policyManager) {
        status["status"] = "error";
        status["message"] = "Policy manager not available";
    } else {
        const NetworkPolicy* policy = m_policyManager->getPolicy(policyId.toStdString());
        if (!policy) {
            status["status"] = "not_found";
        } else {
            status["status"] = "found";
            status["policy_id"] = QString::fromStdString(policy->policy_id);
            status["policy_name"] = QString::fromStdString(policy->policy_name);
            status["policy_type"] = QString::fromStdString(policy->policy_type);
            status["enabled"] = policy->enabled;
            status["priority"] = policy->priority;
            status["rule_count"] = static_cast<int>(policy->rules.size());
        }
    }

    QJsonDocument doc(status);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetHealthStatus() {
    // Get health status from service (via DataGuardService)
    // For now, we'll construct it from available components
    QJsonObject health;
    
    health["dbus_interface"] = isHealthy();
    
    if (m_networkEnforcement) {
        QJsonObject network;
        network["running"] = m_networkEnforcement->isRunning();
        network["healthy"] = m_networkEnforcement->isHealthy();
        network["blocked_count"] = static_cast<qint64>(m_networkEnforcement->getBlockedCount());
        network["allowed_count"] = static_cast<qint64>(m_networkEnforcement->getAllowedCount());
        health["network_enforcement"] = network;
    }
    
    if (m_policyManager) {
        QJsonObject policy;
        policy["initialized"] = true;
        policy["policy_count"] = static_cast<int>(m_policyManager->getPolicies().size());
        health["policy_manager"] = policy;
    }
    
    health["overall_health"] = (isHealthy() ? "healthy" : "unhealthy");
    health["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonDocument doc(health);
    return QString::fromUtf8(doc.toJson());
}

bool DBusInterface::registerInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();
    
    if (!connection.isConnected()) {
        LOG_ERROR(QString("Cannot connect to D-Bus system bus: %1").arg(connection.lastError().message()));
        return false;
    }

    // Register object at path /org/milos/DataGuard
    QString objectPath = "/org/milos/DataGuard";
    if (!connection.registerObject(objectPath, this)) {
        LOG_ERROR(QString("Failed to register D-Bus object: %1").arg(connection.lastError().message()));
        return false;
    }

    // Register service name org.milos.DataGuard
    QString serviceName = "org.milos.DataGuard";
    if (!connection.registerService(serviceName)) {
        LOG_ERROR(QString("Failed to register D-Bus service: %1").arg(connection.lastError().message()));
        connection.unregisterObject(objectPath);
        return false;
    }

    LOG_INFO(QString("D-Bus interface registered: %1 at %2").arg(serviceName).arg(objectPath));
    return true;
}

void DBusInterface::unregisterInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();
    
    QString objectPath = "/org/milos/DataGuard";
    QString serviceName = "org.milos.DataGuard";
    
    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);
}

