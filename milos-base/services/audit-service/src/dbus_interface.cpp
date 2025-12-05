#include "dbus_interface.h"
#include "config_parser.h"
#include "log_storage.h"
#include "hash_chain.h"
#include "query_engine.h"
#include "event_collector.h"
#include <milos/logging/logger.h>
#include <QDBusConnection>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QUuid>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
    , m_hashChain(nullptr)
    , m_queryEngine(nullptr)
    , m_eventCollector(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(
    ConfigParser* configParser,
    LogStorage* logStorage,
    HashChain* hashChain,
    QueryEngine* queryEngine,
    EventCollector* eventCollector
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_logStorage = logStorage;
    m_hashChain = hashChain;
    m_queryEngine = queryEngine;
    m_eventCollector = eventCollector;

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        // Logging handled by service initialization
        return false;
    }

    if (m_running) {
        return true;
    }

    // Register D-Bus interface
    if (!registerInterface()) {
        // Logging handled by service initialization
        return false;
    }

    m_running = true;
    // Logging handled by service initialization
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    // Logging handled by service shutdown
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

QString DBusInterface::LogEvent(const QString& eventData) {
    if (!m_eventCollector) {
        // Logging handled by service
        return QString();
    }

    // Collect event
    if (!m_eventCollector->collectEvent(eventData.toStdString())) {
        // Logging handled by service
        return QString();
    }

    // Extract event ID from event data
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(eventData.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("log_id")) {
            QString eventId = obj["log_id"].toString();
            emit EventLogged(eventId);
            return eventId;
        }
    }

    // Generate event ID
    QString eventId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    emit EventLogged(eventId);
    return eventId;
}

QString DBusInterface::QueryEvents(const QString& queryParams) {
    if (!m_queryEngine) {
        // Logging handled by service
        return QString("{\"error\":\"Query engine not available\"}");
    }

    std::string result = m_queryEngine->queryLogs(queryParams.toStdString());
    return QString::fromStdString(result);
}

QString DBusInterface::GetEventStatistics(const QString& queryParams) {
    if (!m_queryEngine) {
        // Logging handled by service
        return QString("{\"error\":\"Query engine not available\"}");
    }

    std::string result = m_queryEngine->getStatistics(queryParams.toStdString());
    return QString::fromStdString(result);
}

QString DBusInterface::ExportAuditLog(const QString& timeRange, const QString& format) {
    if (!m_queryEngine) {
        // Logging handled by service
        return QString();
    }

    std::string result = m_queryEngine->exportLogs(timeRange.toStdString(), format.toStdString());
    return QString::fromStdString(result);
}

bool DBusInterface::VerifyLogIntegrity() {
    if (!m_hashChain) {
        // Logging handled by service
        return false;
    }

    bool verified = m_hashChain->verifyIntegrity();
    
    if (!verified) {
        emit IntegrityViolationDetected();
    }
    
    return verified;
}

QString DBusInterface::GetHealthStatus() {
    // Get health status from service (via AuditService)
    // For now, we'll construct it from available components
    QJsonObject health;

    health["dbus_interface"] = isHealthy();

    if (m_logStorage) {
        QJsonObject storage;
        storage["initialized"] = true;
        storage["healthy"] = m_logStorage->isHealthy();
        health["log_storage"] = storage;
    }

    if (m_hashChain) {
        QJsonObject hash;
        hash["initialized"] = true;
        hash["healthy"] = m_hashChain->isHealthy();
        health["hash_chain"] = hash;
    }

    if (m_eventCollector) {
        QJsonObject collector;
        collector["running"] = m_eventCollector->isRunning();
        collector["healthy"] = m_eventCollector->isHealthy();
        health["event_collector"] = collector;
    }

    health["overall_health"] = (isHealthy() ? "healthy" : "unhealthy");
    health["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(health);
    return QString::fromUtf8(doc.toJson());
}

bool DBusInterface::registerInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.isConnected()) {
        // Logging handled by service initialization
        return false;
    }

    // Register object at path /org/milos/AuditService
    QString objectPath = "/org/milos/AuditService";
    if (!connection.registerObject(objectPath, this)) {
        // Logging handled by service initialization
        return false;
    }

    // Register service name org.milos.AuditService
    QString serviceName = "org.milos.AuditService";
    if (!connection.registerService(serviceName)) {
        // Logging handled by service initialization
        connection.unregisterObject(objectPath);
        return false;
    }

    // Logging handled by service initialization
    return true;
}

void DBusInterface::unregisterInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    QString objectPath = "/org/milos/AuditService";
    QString serviceName = "org.milos.AuditService";

    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);
}

