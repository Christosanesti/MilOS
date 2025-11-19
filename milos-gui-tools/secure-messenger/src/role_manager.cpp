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

bool RoleManager::checkPermission(const QString& userId, Permission permission) const {
    UserRole role = getRole(userId);
    QSet<Permission> permissions = m_rolePermissions.value(role, QSet<Permission>());
    return permissions.contains(permission);
}

UserRole RoleManager::getRole(const QString& userId) const {
    return m_userRoles.value(userId, UserRole::User);
}

bool RoleManager::setRole(const QString& userId, UserRole role) {
    m_userRoles[userId] = role;
    return true;
}

void RoleManager::initializeDefaultPermissions() {
    // Commanding Officer: All permissions
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::GenerateKeys);
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::DistributeKeys);
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::RevokeKeys);
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::EnrollUsers);
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::ApproveEnrollments);
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::ManageNetwork);
    m_rolePermissions[UserRole::CommandingOfficer].insert(Permission::ViewLogs);
    
    // Tech Personnel: Key and network management
    m_rolePermissions[UserRole::TechPersonnel].insert(Permission::GenerateKeys);
    m_rolePermissions[UserRole::TechPersonnel].insert(Permission::DistributeKeys);
    m_rolePermissions[UserRole::TechPersonnel].insert(Permission::RevokeKeys);
    m_rolePermissions[UserRole::TechPersonnel].insert(Permission::ManageNetwork);
    m_rolePermissions[UserRole::TechPersonnel].insert(Permission::ViewLogs);
    
    // User: No special permissions
}

