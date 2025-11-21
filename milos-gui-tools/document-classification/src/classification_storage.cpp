#include "classification_storage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

ClassificationStorage::ClassificationStorage(QObject* parent)
    : QObject(parent)
{
}

ClassificationStorage::~ClassificationStorage() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool ClassificationStorage::initialize(const QString& dbPath) {
    m_database = QSqlDatabase::addDatabase("QSQLITE", "classification_storage");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open classification database:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qWarning() << "Failed to create classification tables";
        return false;
    }
    
    return true;
}

bool ClassificationStorage::createTables() {
    QSqlQuery query(m_database);
    
    query.prepare("CREATE TABLE IF NOT EXISTS classifications ("
                  "file_path TEXT PRIMARY KEY,"
                  "level INTEGER,"
                  "category TEXT,"
                  "owner TEXT,"
                  "classified_at TEXT,"
                  "classified_by TEXT,"
                  "handling_instructions TEXT,"
                  "custom_metadata TEXT"
                  ")");
    
    if (!query.exec()) {
        qWarning() << "Failed to create classifications table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ClassificationStorage::storeClassification(const QString& filePath, const ClassificationMetadata& metadata) {
    QSqlQuery query(m_database);
    
    query.prepare("INSERT OR REPLACE INTO classifications "
                  "(file_path, level, category, owner, classified_at, classified_by, handling_instructions, custom_metadata) "
                  "VALUES (:file_path, :level, :category, :owner, :classified_at, :classified_by, :handling_instructions, :custom_metadata)");
    
    query.bindValue(":file_path", filePath);
    query.bindValue(":level", static_cast<int>(metadata.level));
    query.bindValue(":category", metadata.category);
    query.bindValue(":owner", metadata.owner);
    query.bindValue(":classified_at", metadata.classifiedAt.toString(Qt::ISODate));
    query.bindValue(":classified_by", metadata.classifiedBy);
    query.bindValue(":handling_instructions", metadata.handlingInstructions.join(";"));
    
    QJsonDocument doc = QJsonDocument::fromVariant(metadata.customMetadata);
    query.bindValue(":custom_metadata", doc.toJson());
    
    if (!query.exec()) {
        qWarning() << "Failed to store classification:" << query.lastError().text();
        return false;
    }
    
    return true;
}

ClassificationMetadata ClassificationStorage::retrieveClassification(const QString& filePath) const {
    QSqlQuery query(m_database);
    
    query.prepare("SELECT * FROM classifications WHERE file_path = :file_path");
    query.bindValue(":file_path", filePath);
    
    if (!query.exec() || !query.next()) {
        return ClassificationMetadata();
    }
    
    return metadataFromQuery(query);
}

QList<QString> ClassificationStorage::getAllClassifiedFiles() const {
    QList<QString> files;
    QSqlQuery query(m_database);
    
    query.prepare("SELECT file_path FROM classifications");
    
    if (query.exec()) {
        while (query.next()) {
            files.append(query.value(0).toString());
        }
    }
    
    return files;
}

QList<QString> ClassificationStorage::getFilesByLevel(ClassificationLevel level) const {
    QList<QString> files;
    QSqlQuery query(m_database);
    
    query.prepare("SELECT file_path FROM classifications WHERE level = :level");
    query.bindValue(":level", static_cast<int>(level));
    
    if (query.exec()) {
        while (query.next()) {
            files.append(query.value(0).toString());
        }
    }
    
    return files;
}

ClassificationMetadata ClassificationStorage::metadataFromQuery(const QSqlQuery& query) const {
    ClassificationMetadata metadata;
    
    metadata.level = static_cast<ClassificationLevel>(query.value("level").toInt());
    metadata.category = query.value("category").toString();
    metadata.owner = query.value("owner").toString();
    metadata.classifiedAt = QDateTime::fromString(query.value("classified_at").toString(), Qt::ISODate);
    metadata.classifiedBy = query.value("classified_by").toString();
    metadata.handlingInstructions = query.value("handling_instructions").toString().split(";");
    
    QJsonDocument doc = QJsonDocument::fromJson(query.value("custom_metadata").toByteArray());
    metadata.customMetadata = doc.object().toVariantMap();
    
    return metadata;
}

