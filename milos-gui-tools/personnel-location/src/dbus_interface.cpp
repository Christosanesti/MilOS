#include "dbus_interface.h"
#include "location_tracker.h"
#include <QDebug>
#include <QDBusConnection>
#include <QJsonDocument>

PersonnelLocationDBusInterface::PersonnelLocationDBusInterface(QObject* parent)
    : QObject(parent)
    , m_tracker(nullptr)
{
}

PersonnelLocationDBusInterface::~PersonnelLocationDBusInterface() {
}

bool PersonnelLocationDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.PersonnelLocation")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/PersonnelLocation", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void PersonnelLocationDBusInterface::setTracker(LocationTracker* tracker) {
    m_tracker = tracker;
}

bool PersonnelLocationDBusInterface::StartTracking(const QString& personnelId) {
    if (!m_tracker) {
        return false;
    }
    
    return m_tracker->startTracking(personnelId);
}

bool PersonnelLocationDBusInterface::StopTracking(const QString& personnelId) {
    if (!m_tracker) {
        return false;
    }
    
    return m_tracker->stopTracking(personnelId);
}

QVariantMap PersonnelLocationDBusInterface::GetCurrentLocation(const QString& personnelId) {
    QVariantMap result;
    
    if (!m_tracker) {
        return result;
    }
    
    LocationData location = m_tracker->getCurrentLocation(personnelId);
    
    result["personnel_id"] = location.personnelId;
    result["latitude"] = location.coordinate.latitude();
    result["longitude"] = location.coordinate.longitude();
    result["altitude"] = location.coordinate.altitude();
    result["source"] = static_cast<int>(location.source);
    result["timestamp"] = location.timestamp.toString(Qt::ISODate);
    result["accuracy"] = location.accuracy;
    
    return result;
}

QStringList PersonnelLocationDBusInterface::GetLocationHistory(const QString& personnelId, const QString& startTime, const QString& endTime) {
    QStringList result;
    
    if (!m_tracker) {
        return result;
    }
    
    QDateTime start = startTime.isEmpty() ? QDateTime() : QDateTime::fromString(startTime, Qt::ISODate);
    QDateTime end = endTime.isEmpty() ? QDateTime() : QDateTime::fromString(endTime, Qt::ISODate);
    
    QList<LocationData> history = m_tracker->getLocationHistory(personnelId, start, end);
    
    for (const LocationData& location : history) {
        QJsonObject json;
        json["latitude"] = location.coordinate.latitude();
        json["longitude"] = location.coordinate.longitude();
        json["timestamp"] = location.timestamp.toString(Qt::ISODate);
        
        QJsonDocument doc(json);
        result.append(doc.toJson());
    }
    
    return result;
}

bool PersonnelLocationDBusInterface::IsTrackingEnabled() {
    if (!m_tracker) {
        return false;
    }
    
    return m_tracker->trackingEnabled();
}

