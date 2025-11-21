#include "location_tracker.h"
#include <QTimer>
#include <QDebug>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>

LocationTracker::LocationTracker(QObject* parent)
    : QObject(parent)
    , m_trackingEnabled(false)
    , m_updateTimer(new QTimer(this))
{
    connect(m_updateTimer, &QTimer::timeout, this, &LocationTracker::updateLocation);
}

LocationTracker::~LocationTracker() {
}

bool LocationTracker::initialize() {
    m_updateTimer->setInterval(5000);  // Update every 5 seconds
    return true;
}

bool LocationTracker::startTracking(const QString& personnelId) {
    if (personnelId.isEmpty()) {
        return false;
    }
    
    if (!m_trackingEnabled) {
        m_trackingEnabled = true;
        m_updateTimer->start();
        emit trackingEnabledChanged(true);
    }
    
    // Initialize location data
    LocationData location;
    location.personnelId = personnelId;
    location.timestamp = QDateTime::currentDateTime();
    m_currentLocations[personnelId] = location;
    
    // Perform initial update
    updateLocation();
    
    return true;
}

bool LocationTracker::stopTracking(const QString& personnelId) {
    if (!m_currentLocations.contains(personnelId)) {
        return false;
    }
    
    m_currentLocations.remove(personnelId);
    
    // Stop timer if no one is being tracked
    if (m_currentLocations.isEmpty() && m_trackingEnabled) {
        m_trackingEnabled = false;
        m_updateTimer->stop();
        emit trackingEnabledChanged(false);
    }
    
    return true;
}

LocationData LocationTracker::getCurrentLocation(const QString& personnelId) const {
    return m_currentLocations.value(personnelId, LocationData());
}

QList<LocationData> LocationTracker::getLocationHistory(const QString& personnelId,
                                                        const QDateTime& startTime,
                                                        const QDateTime& endTime) const {
    QList<LocationData> history = m_locationHistory.value(personnelId);
    
    if (startTime.isValid() || endTime.isValid()) {
        QList<LocationData> filtered;
        for (const LocationData& location : history) {
            if (startTime.isValid() && location.timestamp < startTime) {
                continue;
            }
            if (endTime.isValid() && location.timestamp > endTime) {
                continue;
            }
            filtered.append(location);
        }
        return filtered;
    }
    
    return history;
}

void LocationTracker::setTrackingEnabled(bool enabled) {
    if (m_trackingEnabled != enabled) {
        m_trackingEnabled = enabled;
        if (enabled) {
            m_updateTimer->start();
        } else {
            m_updateTimer->stop();
        }
        emit trackingEnabledChanged(enabled);
    }
}

void LocationTracker::updateLocation() {
    for (auto it = m_currentLocations.begin(); it != m_currentLocations.end(); ++it) {
        QString personnelId = it.key();
        LocationData& location = it.value();
        
        // Try multiple location sources
        QList<LocationData> sources;
        
        LocationData gpsLocation = getGPSLocation();
        if (gpsLocation.coordinate.isValid()) {
            sources.append(gpsLocation);
        }
        
        LocationData wifiLocation = getWiFiLocation();
        if (wifiLocation.coordinate.isValid()) {
            sources.append(wifiLocation);
        }
        
        LocationData networkLocation = getNetworkLocation();
        if (networkLocation.coordinate.isValid()) {
            sources.append(networkLocation);
        }
        
        // Combine sources for best accuracy
        if (!sources.isEmpty()) {
            location = combineLocationSources(sources);
            location.personnelId = personnelId;
            location.timestamp = QDateTime::currentDateTime();
            
            // Store in history
            m_locationHistory[personnelId].append(location);
            
            // Keep only last 1000 entries per personnel
            if (m_locationHistory[personnelId].size() > 1000) {
                m_locationHistory[personnelId].removeFirst();
            }
            
            emit locationUpdated(personnelId, location);
        }
    }
}

LocationData LocationTracker::getGPSLocation() const {
    LocationData location;
    location.source = LocationSource::GPS;
    
    // In production, would use QGeoPositionInfoSource
    // For now, placeholder
    location.coordinate = QGeoCoordinate();  // Invalid coordinate
    location.accuracy = 0;
    
    return location;
}

LocationData LocationTracker::getWiFiLocation() const {
    LocationData location;
    location.source = LocationSource::WiFi;
    
    // In production, would use WiFi triangulation
    // For now, placeholder
    location.coordinate = QGeoCoordinate();  // Invalid coordinate
    location.accuracy = 0;
    
    return location;
}

LocationData LocationTracker::getNetworkLocation() const {
    LocationData location;
    location.source = LocationSource::Network;
    
    // In production, would use network-based location
    // For now, placeholder
    location.coordinate = QGeoCoordinate();  // Invalid coordinate
    location.accuracy = 0;
    
    return location;
}

LocationData LocationTracker::combineLocationSources(const QList<LocationData>& sources) const {
    if (sources.isEmpty()) {
        return LocationData();
    }
    
    // Prefer GPS if available (most accurate)
    for (const LocationData& source : sources) {
        if (source.source == LocationSource::GPS) {
            return source;
        }
    }
    
    // Otherwise, use first available source
    return sources.first();
}

