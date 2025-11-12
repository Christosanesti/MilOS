#include "segment_config.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QUuid>
#include <QDebug>
#include <iostream>

Q_DECLARE_METATYPE(SegmentConfig)
Q_DECLARE_METATYPE(QList<SegmentConfig>)

QJsonObject SegmentConfig::toJson() const {
    QJsonObject obj;
    obj["segmentId"] = segmentId;
    obj["name"] = name;
    obj["description"] = description;
    obj["networkAddress"] = networkAddress;
    
    QJsonArray allowedIPsArray;
    for (const QString& ip : allowedIPs) {
        allowedIPsArray.append(ip);
    }
    obj["allowedIPs"] = allowedIPsArray;
    
    QJsonArray blockedIPsArray;
    for (const QString& ip : blockedIPs) {
        blockedIPsArray.append(ip);
    }
    obj["blockedIPs"] = blockedIPsArray;
    
    QJsonArray allowedPortsArray;
    for (const QString& port : allowedPorts) {
        allowedPortsArray.append(port);
    }
    obj["allowedPorts"] = allowedPortsArray;
    
    QJsonArray blockedPortsArray;
    for (const QString& port : blockedPorts) {
        blockedPortsArray.append(port);
    }
    obj["blockedPorts"] = blockedPortsArray;
    
    QJsonArray allowedProtocolsArray;
    for (const QString& protocol : allowedProtocols) {
        allowedProtocolsArray.append(protocol);
    }
    obj["allowedProtocols"] = allowedProtocolsArray;
    
    obj["isIsolated"] = isIsolated;
    obj["priority"] = priority;
    
    return obj;
}

SegmentConfig SegmentConfig::fromJson(const QJsonObject& json) {
    SegmentConfig config;
    config.segmentId = json["segmentId"].toString();
    config.name = json["name"].toString();
    config.description = json["description"].toString();
    config.networkAddress = json["networkAddress"].toString();
    
    QJsonArray allowedIPsArray = json["allowedIPs"].toArray();
    for (const QJsonValue& value : allowedIPsArray) {
        config.allowedIPs.append(value.toString());
    }
    
    QJsonArray blockedIPsArray = json["blockedIPs"].toArray();
    for (const QJsonValue& value : blockedIPsArray) {
        config.blockedIPs.append(value.toString());
    }
    
    QJsonArray allowedPortsArray = json["allowedPorts"].toArray();
    for (const QJsonValue& value : allowedPortsArray) {
        config.allowedPorts.append(value.toString());
    }
    
    QJsonArray blockedPortsArray = json["blockedPorts"].toArray();
    for (const QJsonValue& value : blockedPortsArray) {
        config.blockedPorts.append(value.toString());
    }
    
    QJsonArray allowedProtocolsArray = json["allowedProtocols"].toArray();
    for (const QJsonValue& value : allowedProtocolsArray) {
        config.allowedProtocols.append(value.toString());
    }
    
    config.isIsolated = json["isIsolated"].toBool();
    config.priority = json["priority"].toInt();
    
    return config;
}

SegmentConfigManager::SegmentConfigManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

SegmentConfigManager::~SegmentConfigManager() {
}

bool SegmentConfigManager::initialize(const QString& dbPath) {
    if (m_initialized) {
        return true;
    }

    m_dbPath = dbPath;
    
    // Create database directory if it doesn't exist
    QDir dbDir = QFileInfo(dbPath).absoluteDir();
    if (!dbDir.exists()) {
        dbDir.mkpath(".");
    }

    // Initialize SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "NetworkSegmentation");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        std::cerr << "Failed to open database: " << dbPath.toStdString() << std::endl;
        return false;
    }

    // Create segments table
    QSqlQuery query(db);
    query.prepare("CREATE TABLE IF NOT EXISTS segments ("
                  "segment_id TEXT PRIMARY KEY, "
                  "name TEXT NOT NULL, "
                  "description TEXT, "
                  "network_address TEXT NOT NULL, "
                  "config_json TEXT NOT NULL, "
                  "created_at INTEGER NOT NULL, "
                  "updated_at INTEGER NOT NULL)");
    
    if (!query.exec()) {
        std::cerr << "Failed to create segments table: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }

    // Load existing segments
    if (!loadSegments()) {
        std::cerr << "Failed to load segments" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

QString SegmentConfigManager::createSegment(const SegmentConfig& config) {
    if (!m_initialized) {
        return QString();
    }

    if (config.segmentId.isEmpty()) {
        // Generate segment ID
        SegmentConfig newConfig = config;
        newConfig.segmentId = QUuid::createUuid().toString();
        
        if (saveSegment(newConfig)) {
            m_segments[newConfig.segmentId] = newConfig;
            emit segmentCreated(newConfig.segmentId);
            return newConfig.segmentId;
        }
    } else {
        if (saveSegment(config)) {
            m_segments[config.segmentId] = config;
            emit segmentCreated(config.segmentId);
            return config.segmentId;
        }
    }

    return QString();
}

bool SegmentConfigManager::updateSegment(const QString& segmentId, const SegmentConfig& config) {
    if (!m_initialized || !m_segments.contains(segmentId)) {
        return false;
    }

    SegmentConfig updatedConfig = config;
    updatedConfig.segmentId = segmentId;

    if (saveSegment(updatedConfig)) {
        m_segments[segmentId] = updatedConfig;
        emit segmentUpdated(segmentId);
        return true;
    }

    return false;
}

bool SegmentConfigManager::deleteSegment(const QString& segmentId) {
    if (!m_initialized || !m_segments.contains(segmentId)) {
        return false;
    }

    if (removeSegment(segmentId)) {
        m_segments.remove(segmentId);
        emit segmentDeleted(segmentId);
        return true;
    }

    return false;
}

SegmentConfig SegmentConfigManager::getSegment(const QString& segmentId) const {
    return m_segments.value(segmentId, SegmentConfig());
}

QList<SegmentConfig> SegmentConfigManager::listSegments() const {
    return m_segments.values();
}

QList<SegmentConfig> SegmentConfigManager::getSegmentsByNetwork(const QString& networkAddress) const {
    QList<SegmentConfig> results;
    for (const SegmentConfig& config : m_segments.values()) {
        if (config.networkAddress == networkAddress) {
            results.append(config);
        }
    }
    return results;
}

bool SegmentConfigManager::loadSegments() {
    QSqlDatabase db = QSqlDatabase::database("NetworkSegmentation");
    if (!db.isOpen()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT segment_id, config_json FROM segments");
    
    if (!query.exec()) {
        std::cerr << "Failed to load segments: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }

    m_segments.clear();
    while (query.next()) {
        QString segmentId = query.value(0).toString();
        QString configJson = query.value(1).toString();
        
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(configJson.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError) {
            SegmentConfig config = SegmentConfig::fromJson(doc.object());
            m_segments[segmentId] = config;
        }
    }

    return true;
}

bool SegmentConfigManager::saveSegment(const SegmentConfig& config) {
    QSqlDatabase db = QSqlDatabase::database("NetworkSegmentation");
    if (!db.isOpen()) {
        return false;
    }

    QJsonObject json = config.toJson();
    QJsonDocument doc(json);
    QString configJson = QString::fromUtf8(doc.toJson());

    qint64 now = QDateTime::currentSecsSinceEpoch();

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO segments (segment_id, name, description, network_address, config_json, created_at, updated_at) "
                  "VALUES (:segment_id, :name, :description, :network_address, :config_json, "
                  "COALESCE((SELECT created_at FROM segments WHERE segment_id = :segment_id), :now), :now)");
    query.bindValue(":segment_id", config.segmentId);
    query.bindValue(":name", config.name);
    query.bindValue(":description", config.description);
    query.bindValue(":network_address", config.networkAddress);
    query.bindValue(":config_json", configJson);
    query.bindValue(":now", now);

    if (!query.exec()) {
        std::cerr << "Failed to save segment: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }

    return true;
}

bool SegmentConfigManager::removeSegment(const QString& segmentId) {
    QSqlDatabase db = QSqlDatabase::database("NetworkSegmentation");
    if (!db.isOpen()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM segments WHERE segment_id = :segment_id");
    query.bindValue(":segment_id", segmentId);

    if (!query.exec()) {
        std::cerr << "Failed to remove segment: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }

    return true;
}

