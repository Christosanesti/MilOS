#include "dbus_interface.h"
#include "config_parser.h"
#include "policy_manager.h"
#include "network_enforcement.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <iostream>

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
        std::cerr << "D-Bus interface not initialized" << std::endl;
        return false;
    }

    if (m_running) {
        return true;
    }

    // Register D-Bus interface
    if (!registerInterface()) {
        std::cerr << "Failed to register D-Bus interface" << std::endl;
        return false;
    }

    m_running = true;
    std::cout << "D-Bus interface started" << std::endl;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
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
    
    // TODO: Retrieve actual blocked transmission list from network enforcement
    // For now, return empty list
    // This would require storing blocked transmission details
    
    return blocked;
}

bool DBusInterface::ConfigurePolicy(const QString& policy) {
    if (!m_policyManager) {
        std::cerr << "Policy manager not available" << std::endl;
        return false;
    }

    // Parse policy JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(policy.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Failed to parse policy JSON: " << error.errorString().toStdString() << std::endl;
        return false;
    }

    if (!doc.isObject()) {
        std::cerr << "Policy JSON is not an object" << std::endl;
        return false;
    }

    QJsonObject policyObj = doc.object();
    
    // TODO: Create NetworkPolicy from JSON and add to policy manager
    // For now, return false as full implementation requires policy manager extension
    
    std::cerr << "Policy configuration not fully implemented yet" << std::endl;
    return false;
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
        std::cerr << "Cannot connect to D-Bus system bus: " 
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    // Register object at path /org/milos/DataGuard
    QString objectPath = "/org/milos/DataGuard";
    if (!connection.registerObject(objectPath, this)) {
        std::cerr << "Failed to register D-Bus object: " 
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    // Register service name org.milos.DataGuard
    QString serviceName = "org.milos.DataGuard";
    if (!connection.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: " 
                  << connection.lastError().message().toStdString() << std::endl;
        connection.unregisterObject(objectPath);
        return false;
    }

    std::cout << "D-Bus interface registered: " << serviceName.toStdString() 
              << " at " << objectPath.toStdString() << std::endl;
    return true;
}

void DBusInterface::unregisterInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();
    
    QString objectPath = "/org/milos/DataGuard";
    QString serviceName = "org.milos.DataGuard";
    
    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);
}

