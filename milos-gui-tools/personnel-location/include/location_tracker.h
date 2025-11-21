#ifndef LOCATION_TRACKER_H
#define LOCATION_TRACKER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QGeoCoordinate>
#include <QVariantMap>

/**
 * @brief Location Source
 */
enum class LocationSource {
    GPS,
    WiFi,
    Cellular,
    Network,
    Manual
};

/**
 * @brief Location Data
 */
struct LocationData {
    QString personnelId;
    QGeoCoordinate coordinate;
    LocationSource source;
    QDateTime timestamp;
    double accuracy;  // in meters
    QVariantMap metadata;
};

/**
 * @brief Location Tracker
 * 
 * Tracks personnel location using multiple sources.
 */
class LocationTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool trackingEnabled READ trackingEnabled WRITE setTrackingEnabled NOTIFY trackingEnabledChanged)

public:
    explicit LocationTracker(QObject* parent = nullptr);
    ~LocationTracker();

    /**
     * @brief Initialize location tracker
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start tracking for personnel
     * @param personnelId Personnel ID
     * @return true if start successful, false otherwise
     */
    bool startTracking(const QString& personnelId);

    /**
     * @brief Stop tracking for personnel
     * @param personnelId Personnel ID
     * @return true if stop successful, false otherwise
     */
    bool stopTracking(const QString& personnelId);

    /**
     * @brief Get current location
     * @param personnelId Personnel ID
     * @return Location data or empty if not found
     */
    LocationData getCurrentLocation(const QString& personnelId) const;

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
     * @brief Check if tracking is enabled
     * @return true if enabled, false otherwise
     */
    bool trackingEnabled() const { return m_trackingEnabled; }

    /**
     * @brief Set tracking enabled
     * @param enabled Enabled state
     */
    void setTrackingEnabled(bool enabled);

Q_SIGNALS:
    /**
     * @brief Emitted when location is updated
     */
    void locationUpdated(const QString& personnelId, const LocationData& location);

    /**
     * @brief Emitted when tracking enabled state changes
     */
    void trackingEnabledChanged(bool enabled);

private slots:
    void updateLocation();

private:
    bool m_trackingEnabled;
    QMap<QString, LocationData> m_currentLocations;
    QMap<QString, QList<LocationData>> m_locationHistory;
    QTimer* m_updateTimer;
    
    LocationData getGPSLocation() const;
    LocationData getWiFiLocation() const;
    LocationData getNetworkLocation() const;
    LocationData combineLocationSources(const QList<LocationData>& sources) const;
};

Q_DECLARE_METATYPE(LocationSource)
Q_DECLARE_METATYPE(LocationData)

#endif // LOCATION_TRACKER_H

