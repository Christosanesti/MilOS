#include "location_storage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

LocationStorage::LocationStorage(QObject* parent)
    : QObject(parent)
{
}

LocationStorage::~LocationStorage() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool LocationStorage::initialize(const QString& dbPath) {
    m_database = QSqlDatabase::addDatabase("QSQLITE", "location_storage");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open location database:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qWarning() << "Failed to create location tables";
        return false;
    }
    
    return true;
}

bool LocationStorage::createTables() {
    QSqlQuery query(m_database);
    
    query.prepare("CREATE TABLE IF NOT EXISTS locations ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "personnel_id TEXT,"
                  "latitude REAL,"
                  "longitude REAL,"
                  "altitude REAL,"
                  "source INTEGER,"
                  "timestamp TEXT,"
                  "accuracy REAL,"
                  "metadata TEXT"
                  ")");
    
    if (!query.exec()) {
        qWarning() << "Failed to create locations table:" << query.lastError().text();
        return false;
    }
    
    // Create index for faster queries
    query.prepare("CREATE INDEX IF NOT EXISTS idx_personnel_timestamp ON locations(personnel_id, timestamp)");
    query.exec();
    
    return true;
}

bool LocationStorage::storeLocation(const LocationData& location) {
    QSqlQuery query(m_database);
    
    query.prepare("INSERT INTO locations "
                  "(personnel_id, latitude, longitude, altitude, source, timestamp, accuracy, metadata) "
                  "VALUES (:personnel_id, :latitude, :longitude, :altitude, :source, :timestamp, :accuracy, :metadata)");
    
    query.bindValue(":personnel_id", location.personnelId);
    query.bindValue(":latitude", location.coordinate.latitude());
    query.bindValue(":longitude", location.coordinate.longitude());
    query.bindValue(":altitude", location.coordinate.altitude());
    query.bindValue(":source", static_cast<int>(location.source));
    query.bindValue(":timestamp", location.timestamp.toString(Qt::ISODate));
    query.bindValue(":accuracy", location.accuracy);
    
    QJsonDocument doc = QJsonDocument::fromVariant(location.metadata);
    query.bindValue(":metadata", doc.toJson());
    
    if (!query.exec()) {
        qWarning() << "Failed to store location:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<LocationData> LocationStorage::getLocationHistory(const QString& personnelId,
                                                        const QDateTime& startTime,
                                                        const QDateTime& endTime) const {
    QList<LocationData> history;
    QSqlQuery query(m_database);
    
    QString sql = "SELECT * FROM locations WHERE personnel_id = :personnel_id";
    if (startTime.isValid()) {
        sql += " AND timestamp >= :start_time";
    }
    if (endTime.isValid()) {
        sql += " AND timestamp <= :end_time";
    }
    sql += " ORDER BY timestamp ASC";
    
    query.prepare(sql);
    query.bindValue(":personnel_id", personnelId);
    if (startTime.isValid()) {
        query.bindValue(":start_time", startTime.toString(Qt::ISODate));
    }
    if (endTime.isValid()) {
        query.bindValue(":end_time", endTime.toString(Qt::ISODate));
    }
    
    if (query.exec()) {
        while (query.next()) {
            history.append(locationFromQuery(query));
        }
    }
    
    return history;
}

QList<QGeoCoordinate> LocationStorage::getPathCoordinates(const QString& personnelId,
                                                         const QDateTime& startTime,
                                                         const QDateTime& endTime) const {
    QList<QGeoCoordinate> coordinates;
    QList<LocationData> history = getLocationHistory(personnelId, startTime, endTime);
    
    for (const LocationData& location : history) {
        if (location.coordinate.isValid()) {
            coordinates.append(location.coordinate);
        }
    }
    
    return coordinates;
}

LocationData LocationStorage::locationFromQuery(const QSqlQuery& query) const {
    LocationData location;
    
    location.personnelId = query.value("personnel_id").toString();
    location.coordinate = QGeoCoordinate(
        query.value("latitude").toDouble(),
        query.value("longitude").toDouble(),
        query.value("altitude").toDouble()
    );
    location.source = static_cast<LocationSource>(query.value("source").toInt());
    location.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);
    location.accuracy = query.value("accuracy").toDouble();
    
    QJsonDocument doc = QJsonDocument::fromJson(query.value("metadata").toByteArray());
    location.metadata = doc.object().toVariantMap();
    
    return location;
}

