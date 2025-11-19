#include "access_restrictions.h"
#include <QUuid>
#include <QDebug>

AccessRestrictionsManager::AccessRestrictionsManager(QObject* parent)
    : QObject(parent)
{
}

AccessRestrictionsManager::~AccessRestrictionsManager() {
}

bool AccessRestrictionsManager::initialize() {
    return true;
}

QString AccessRestrictionsManager::createTimeRestriction(const QString& personnelId,
                                                        const QString& location,
                                                        const QTime& startTime,
                                                        const QTime& endTime,
                                                        const QList<int>& daysOfWeek) {
    TimeRestriction restriction;
    restriction.restrictionId = generateRestrictionId();
    restriction.personnelId = personnelId;
    restriction.location = location;
    restriction.startTime = startTime;
    restriction.endTime = endTime;
    restriction.daysOfWeek = daysOfWeek;
    restriction.enabled = true;
    
    m_timeRestrictions.append(restriction);
    emit timeRestrictionCreated(restriction.restrictionId);
    
    return restriction.restrictionId;
}

bool AccessRestrictionsManager::checkTimeRestriction(const QString& personnelId,
                                                    const QString& location,
                                                    const QDateTime& requestTime) const {
    for (const TimeRestriction& restriction : m_timeRestrictions) {
        if (restriction.personnelId == personnelId &&
            restriction.location == location &&
            restriction.enabled) {
            
            // Check day of week
            int dayOfWeek = requestTime.date().dayOfWeek() - 1;  // Qt: 1=Monday, 7=Sunday
            if (!restriction.daysOfWeek.contains(dayOfWeek)) {
                return false;  // Not allowed on this day
            }
            
            // Check time range
            QTime requestTimeOnly = requestTime.time();
            if (requestTimeOnly < restriction.startTime || requestTimeOnly > restriction.endTime) {
                return false;  // Outside allowed time range
            }
        }
    }
    
    return true;  // No restrictions found, allow access
}

QString AccessRestrictionsManager::createLocationRestriction(const QString& personnelId,
                                                            const QString& location,
                                                            bool allowed) {
    LocationRestriction restriction;
    restriction.restrictionId = generateRestrictionId();
    restriction.personnelId = personnelId;
    restriction.location = location;
    restriction.allowed = allowed;
    restriction.enabled = true;
    
    m_locationRestrictions.append(restriction);
    emit locationRestrictionCreated(restriction.restrictionId);
    
    return restriction.restrictionId;
}

bool AccessRestrictionsManager::checkLocationRestriction(const QString& personnelId, const QString& location) const {
    for (const LocationRestriction& restriction : m_locationRestrictions) {
        if (restriction.personnelId == personnelId &&
            restriction.location == location &&
            restriction.enabled) {
            return restriction.allowed;
        }
    }
    
    return true;  // No restrictions found, allow access
}

QList<TimeRestriction> AccessRestrictionsManager::getTimeRestrictions(const QString& personnelId,
                                                                      const QString& location) const {
    QList<TimeRestriction> filtered;
    
    for (const TimeRestriction& restriction : m_timeRestrictions) {
        if (!personnelId.isEmpty() && restriction.personnelId != personnelId) {
            continue;
        }
        if (!location.isEmpty() && restriction.location != location) {
            continue;
        }
        
        filtered.append(restriction);
    }
    
    return filtered;
}

QList<LocationRestriction> AccessRestrictionsManager::getLocationRestrictions(const QString& personnelId,
                                                                               const QString& location) const {
    QList<LocationRestriction> filtered;
    
    for (const LocationRestriction& restriction : m_locationRestrictions) {
        if (!personnelId.isEmpty() && restriction.personnelId != personnelId) {
            continue;
        }
        if (!location.isEmpty() && restriction.location != location) {
            continue;
        }
        
        filtered.append(restriction);
    }
    
    return filtered;
}

QString AccessRestrictionsManager::generateRestrictionId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

