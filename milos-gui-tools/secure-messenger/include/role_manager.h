#ifndef ROLE_MANAGER_H
#define ROLE_MANAGER_H

#include "usb_authorization.h"
#include <QObject>
#include <QString>
#include <QSet>

/**
 * @brief Permission
 */
enum class Permission {
    GenerateKeys,
    DistributeKeys,
    RevokeKeys,
    EnrollUsers,
    ApproveEnrollments,
    ManageNetwork,
    ViewLogs
};

/**
 * @brief Role Manager
 * 
 * Manages role-based privileges for commanding officers and tech personnel.
 */
class RoleManager : public QObject {
    Q_OBJECT

public:
    explicit RoleManager(QObject* parent = nullptr);
    ~RoleManager();

    /**
     * @brief Initialize role manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check permission
     * @param userId User ID
     * @param permission Permission to check
     * @return true if permission granted, false otherwise
     */
    bool checkPermission(const QString& userId, Permission permission) const;

    /**
     * @brief Get role for user
     * @param userId User ID
     * @return User role or UserRole::User if not found
     */
    UserRole getRole(const QString& userId) const;

    /**
     * @brief Set role for user
     * @param userId User ID
     * @param role User role
     * @return true if set successful, false otherwise
     */
    bool setRole(const QString& userId, UserRole role);

private:
    QMap<QString, UserRole> m_userRoles;
    QMap<UserRole, QSet<Permission>> m_rolePermissions;
    
    void initializeDefaultPermissions();
};

#endif // ROLE_MANAGER_H

