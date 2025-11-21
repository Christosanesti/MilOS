#include "network_forensics.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>

NetworkForensics::NetworkForensics(QObject* parent)
    : QObject(parent)
{
}

NetworkForensics::~NetworkForensics() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool NetworkForensics::initialize(const QString& dbPath) {
    m_database = QSqlDatabase::addDatabase("QSQLITE", "forensics_storage");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open forensics database:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qWarning() << "Failed to create forensics tables";
        return false;
    }
    
    return true;
}

bool NetworkForensics::createTables() {
    QSqlQuery query(m_database);
    
    query.prepare("CREATE TABLE IF NOT EXISTS forensic_evidence ("
                  "id TEXT PRIMARY KEY,"
                  "type TEXT,"
                  "source TEXT,"
                  "timestamp TEXT,"
                  "data BLOB,"
                  "metadata TEXT"
                  ")");
    
    if (!query.exec()) {
        qWarning() << "Failed to create forensic_evidence table:" << query.lastError().text();
        return false;
    }
    
    // Create index for faster searches
    query.prepare("CREATE INDEX IF NOT EXISTS idx_timestamp ON forensic_evidence(timestamp)");
    query.exec();
    
    return true;
}

bool NetworkForensics::captureEvidence(const ForensicEvidence& evidence) {
    QSqlQuery query(m_database);
    
    query.prepare("INSERT INTO forensic_evidence "
                  "(id, type, source, timestamp, data, metadata) "
                  "VALUES (:id, :type, :source, :timestamp, :data, :metadata)");
    
    query.bindValue(":id", evidence.id);
    query.bindValue(":type", evidence.type);
    query.bindValue(":source", evidence.source);
    query.bindValue(":timestamp", evidence.timestamp.toString(Qt::ISODate));
    query.bindValue(":data", evidence.data);
    
    QJsonDocument doc = QJsonDocument::fromVariant(evidence.metadata);
    query.bindValue(":metadata", doc.toJson());
    
    if (!query.exec()) {
        qWarning() << "Failed to capture evidence:" << query.lastError().text();
        return false;
    }
    
    return true;
}

ForensicEvidence NetworkForensics::getEvidence(const QString& evidenceId) const {
    QSqlQuery query(m_database);
    
    query.prepare("SELECT * FROM forensic_evidence WHERE id = :id");
    query.bindValue(":id", evidenceId);
    
    if (query.exec() && query.next()) {
        return evidenceFromQuery(query);
    }
    
    return ForensicEvidence();
}

QStringList NetworkForensics::searchEvidence(const QVariantMap& criteria) const {
    QStringList ids;
    QSqlQuery query(m_database);
    
    QString sql = "SELECT id FROM forensic_evidence WHERE 1=1";
    
    if (criteria.contains("type")) {
        sql += " AND type = :type";
    }
    if (criteria.contains("start_time")) {
        sql += " AND timestamp >= :start_time";
    }
    if (criteria.contains("end_time")) {
        sql += " AND timestamp <= :end_time";
    }
    
    query.prepare(sql);
    
    if (criteria.contains("type")) {
        query.bindValue(":type", criteria["type"].toString());
    }
    if (criteria.contains("start_time")) {
        query.bindValue(":start_time", criteria["start_time"].toString());
    }
    if (criteria.contains("end_time")) {
        query.bindValue(":end_time", criteria["end_time"].toString());
    }
    
    if (query.exec()) {
        while (query.next()) {
            ids.append(query.value(0).toString());
        }
    }
    
    return ids;
}

QVariantMap NetworkForensics::generateReport(const QDateTime& startTime, const QDateTime& endTime, const QVariantMap& filters) const {
    QVariantMap report;
    
    QStringList evidenceIds = searchEvidence(filters);
    
    report["start_time"] = startTime.toString(Qt::ISODate);
    report["end_time"] = endTime.toString(Qt::ISODate);
    report["evidence_count"] = evidenceIds.size();
    report["evidence_ids"] = evidenceIds;
    
    // Generate statistics
    QVariantMap stats;
    stats["total_evidence"] = evidenceIds.size();
    stats["packet_count"] = 0;
    stats["flow_count"] = 0;
    
    for (const QString& id : evidenceIds) {
        ForensicEvidence evidence = getEvidence(id);
        if (evidence.type == "packet") {
            stats["packet_count"] = stats["packet_count"].toInt() + 1;
        } else if (evidence.type == "flow") {
            stats["flow_count"] = stats["flow_count"].toInt() + 1;
        }
    }
    
    report["statistics"] = stats;
    
    return report;
}

QVariantMap NetworkForensics::analyzeFlow(const QString& flowId) const {
    QVariantMap analysis;
    
    // In production, would perform deep flow analysis
    // For now, return basic analysis
    analysis["flow_id"] = flowId;
    analysis["packet_count"] = 0;
    analysis["total_bytes"] = 0;
    analysis["duration"] = 0;
    analysis["protocols"] = QStringList();
    
    return analysis;
}

ForensicEvidence NetworkForensics::evidenceFromQuery(const QSqlQuery& query) const {
    ForensicEvidence evidence;
    
    evidence.id = query.value("id").toString();
    evidence.type = query.value("type").toString();
    evidence.source = query.value("source").toString();
    evidence.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);
    evidence.data = query.value("data").toByteArray();
    
    QJsonDocument doc = QJsonDocument::fromJson(query.value("metadata").toByteArray());
    evidence.metadata = doc.object().toVariantMap();
    
    return evidence;
}

