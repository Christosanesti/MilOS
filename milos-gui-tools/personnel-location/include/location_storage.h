#ifndef LOCATION_STORAGE_H
#define LOCATION_STORAGE_H

#include "location_tracker.h"
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <QDateTime>

/**
 * @brief Location Storage
 * 
 * Stores location data in SQLite database.
 */
class LocationStorage : public QObject {
    Q_OBJECT

public:
    explicit LocationStorage(QObject* parent = nullptr);
    ~LocationStorage();

    /**
     * @brief Initialize location storage
     * @param dbPath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Store location data
     * @param location Location data
     * @return true if store successful, false otherwise
     */
    bool storeLocation(const LocationData& location);

    /**
     * @brief Get location history
     * @param personnelId Personnel ID
     * @param startTime Start time
     * @param endTime End time
     * @return List of location data
     */
    QList<LocationData> getLocationHistory(const QString& personnelId,
                                           const QDateTime& startTime = QDateTime(),
                                           const QDateTime& endTime = QDateTime()) const;

    /**
     * @brief Get path visualization data
     * @param personnelId Personnel ID
     * @param startTime Start time
     * @param endTime End time
     * @return List of coordinates for path
     */
    QList<QGeoCoordinate> getPathCoordinates(const QString& personnelId,
                                             const QDateTime& startTime,
                                             const QDateTime& endTime) const;

private:
    QSqlDatabase m_database;
    bool createTables();
    LocationData locationFromQuery(const QSqlQuery& query) const;
};

#endif // LOCATION_STORAGE_H

