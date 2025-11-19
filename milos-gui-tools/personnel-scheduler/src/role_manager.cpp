#include "role_manager.h"
#include <QDebug>

RoleManager::RoleManager(QObject* parent)
    : QObject(parent)
{
    initializeDefaultPermissions();
}

RoleManager::~RoleManager() {
}

bool RoleManager::initialize() {
    return true;
}

bool RoleManager::assignRole(const QString& personnelId, Role role) {
    if (personnelId.isEmpty()) {
        return false;
    }
    
    m_personnelRoles[personnelId] = role;
    emit roleAssigned(personnelId, role);
    
    return true;
}

Role RoleManager::getRole(const QString& personnelId) const {
    return m_personnelRoles.value(personnelId, Role::Guest);
}

bool RoleManager::checkPermission(const QString& personnelId, Permission permission) const {
    Role role = getRole(personnelId);
    QSet<Permission> permissions = getPermissionsForRole(role);
    
    return permissions.contains(permission);
}

bool RoleManager::grantPermissionToRole(Role role, Permission permission) {
    m_rolePermissions[role].insert(permission);
    emit permissionGranted(role, permission);
    return true;
}

bool RoleManager::revokePermissionFromRole(Role role, Permission permission) {
    if (m_rolePermissions.contains(role)) {
        m_rolePermissions[role].remove(permission);
        return true;
    }
    
    return false;
}

QSet<Permission> RoleManager::getPermissionsForRole(Role role) const {
    return m_rolePermissions.value(role, QSet<Permission>());
}

void RoleManager::initializeDefaultPermissions() {
    // Administrator: All permissions
    grantPermissionToRole(Role::Administrator, Permission::ViewAttendance);
    grantPermissionToRole(Role::Administrator, Permission::ManageShifts);
    grantPermissionToRole(Role::Administrator, Permission::ManageAccess);
    grantPermissionToRole(Role::Administrator, Permission::ManageRoles);
    grantPermissionToRole(Role::Administrator, Permission::ViewReports);
    grantPermissionToRole(Role::Administrator, Permission::ManageBiometricDevices);
    
    // Security Officer: Most permissions except role management
    grantPermissionToRole(Role::SecurityOfficer, Permission::ViewAttendance);
    grantPermissionToRole(Role::SecurityOfficer, Permission::ManageShifts);
    grantPermissionToRole(Role::SecurityOfficer, Permission::ManageAccess);
    grantPermissionToRole(Role::SecurityOfficer, Permission::ViewReports);
    grantPermissionToRole(Role::SecurityOfficer, Permission::ManageBiometricDevices);
    
    // Personnel: Limited permissions
    grantPermissionToRole(Role::Personnel, Permission::ViewAttendance);
    
    // Guest: No permissions
}

