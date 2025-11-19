#include "access_control.h"
#include "access_restrictions.h"
#include "role_manager.h"
#include <QUuid>
#include <QDebug>

AccessControl::AccessControl(QObject* parent)
    : QObject(parent)
    , m_restrictionsManager(nullptr)
    , m_roleManager(nullptr)
{
}

void AccessControl::setRestrictionsManager(AccessRestrictionsManager* manager) {
    m_restrictionsManager = manager;
}

void AccessControl::setRoleManager(RoleManager* manager) {
    m_roleManager = manager;
}

AccessControl::~AccessControl() {
}

bool AccessControl::initialize() {
    return true;
}

AccessResult AccessControl::requestAccess(const QString& personnelId,
                                         const QString& location,
                                         const QString& deviceId,
                                         const QByteArray& biometricData) {
    if (personnelId.isEmpty() || location.isEmpty()) {
        emit accessDenied(personnelId, location, "Invalid request", QDateTime::currentDateTime());
        return AccessResult::Denied;
    }
    
    QDateTime requestTime = QDateTime::currentDateTime();
    
    // Check time restrictions
    if (!checkTimeRestrictions(personnelId, location, requestTime)) {
        emit accessDenied(personnelId, location, "Time restriction", requestTime);
        return AccessResult::TimeRestricted;
    }
    
    // Check location restrictions
    if (!checkLocationRestrictions(personnelId, location)) {
        emit accessDenied(personnelId, location, "Location restriction", requestTime);
        return AccessResult::LocationRestricted;
    }
    
    // Check role restrictions
    if (!checkRoleRestrictions(personnelId, location)) {
        emit accessDenied(personnelId, location, "Role restriction", requestTime);
        return AccessResult::RoleRestricted;
    }
    
    // Check access permission
    if (!checkAccessPermission(personnelId, location, requestTime)) {
        emit accessDenied(personnelId, location, "No permission", requestTime);
        return AccessResult::Denied;
    }
    
    // Access granted
    emit accessGranted(personnelId, location, requestTime);
    return AccessResult::Granted;
}

bool AccessControl::checkAccessPermission(const QString& personnelId,
                                         const QString& location,
                                         const QDateTime& requestTime) const {
    for (const QVariantMap& permission : m_accessPermissions) {
        if (permission["personnel_id"].toString() == personnelId &&
            permission["location"].toString() == location) {
            
            // Check time range if specified
            if (permission.contains("start_time") && permission.contains("end_time")) {
                QDateTime startTime = permission["start_time"].toDateTime();
                QDateTime endTime = permission["end_time"].toDateTime();
                
                if (requestTime < startTime || requestTime > endTime) {
                    return false;
                }
            }
            
            return permission["enabled"].toBool();
        }
    }
    
    return false;
}

bool AccessControl::grantAccessPermission(const QString& personnelId,
                                         const QString& location,
                                         const QDateTime& startTime,
                                         const QDateTime& endTime) {
    // Remove existing permission if any
    revokeAccessPermission(personnelId, location);
    
    QVariantMap permission;
    permission["personnel_id"] = personnelId;
    permission["location"] = location;
    permission["enabled"] = true;
    
    if (startTime.isValid()) {
        permission["start_time"] = startTime;
    }
    if (endTime.isValid()) {
        permission["end_time"] = endTime;
    }
    
    m_accessPermissions.append(permission);
    return true;
}

bool AccessControl::revokeAccessPermission(const QString& personnelId, const QString& location) {
    for (int i = 0; i < m_accessPermissions.size(); i++) {
        if (m_accessPermissions[i]["personnel_id"].toString() == personnelId &&
            m_accessPermissions[i]["location"].toString() == location) {
            m_accessPermissions.removeAt(i);
            return true;
        }
    }
    
    return false;
}

QList<QVariantMap> AccessControl::getAccessPermissions(const QString& personnelId,
                                                       const QString& location) const {
    QList<QVariantMap> filtered;
    
    for (const QVariantMap& permission : m_accessPermissions) {
        if (!personnelId.isEmpty() && permission["personnel_id"].toString() != personnelId) {
            continue;
        }
        if (!location.isEmpty() && permission["location"].toString() != location) {
            continue;
        }
        
        filtered.append(permission);
    }
    
    return filtered;
}

bool AccessControl::checkTimeRestrictions(const QString& personnelId, const QString& location, const QDateTime& requestTime) const {
    if (!m_restrictionsManager) {
        return true;  // No restrictions manager, allow access
    }
    
    return m_restrictionsManager->checkTimeRestriction(personnelId, location, requestTime);
}

bool AccessControl::checkLocationRestrictions(const QString& personnelId, const QString& location) const {
    if (!m_restrictionsManager) {
        return true;  // No restrictions manager, allow access
    }
    
    return m_restrictionsManager->checkLocationRestriction(personnelId, location);
}

bool AccessControl::checkRoleRestrictions(const QString& personnelId, const QString& location) const {
    if (!m_roleManager) {
        return true;  // No role manager, allow access
    }
    
    // Check if personnel has permission to access location
    // For now, basic check - would be more sophisticated in production
    return m_roleManager->checkPermission(personnelId, Permission::ManageAccess) ||
           m_roleManager->checkPermission(personnelId, Permission::ViewAttendance);
}

QString AccessControl::generateRequestId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

