#ifndef ROLE_MANAGER_H
#define ROLE_MANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QSet>

/**
 * @brief Role
 */
enum class Role {
    Administrator,
    SecurityOfficer,
    Personnel,
    Guest
};

/**
 * @brief Permission
 */
enum class Permission {
    ViewAttendance,
    ManageShifts,
    ManageAccess,
    ManageRoles,
    ViewReports,
    ManageBiometricDevices
};

/**
 * @brief Role Manager
 * 
 * Manages role-based access control.
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
     * @brief Assign role to personnel
     * @param personnelId Personnel ID
     * @param role Role
     * @return true if assignment successful, false otherwise
     */
    bool assignRole(const QString& personnelId, Role role);

    /**
     * @brief Get role for personnel
     * @param personnelId Personnel ID
     * @return Role or Guest if not found
     */
    Role getRole(const QString& personnelId) const;

    /**
     * @brief Check permission
     * @param personnelId Personnel ID
     * @param permission Permission to check
     * @return true if permission granted, false otherwise
     */
    bool checkPermission(const QString& personnelId, Permission permission) const;

    /**
     * @brief Grant permission to role
     * @param role Role
     * @param permission Permission
     * @return true if grant successful, false otherwise
     */
    bool grantPermissionToRole(Role role, Permission permission);

    /**
     * @brief Revoke permission from role
     * @param role Role
     * @param permission Permission
     * @return true if revoke successful, false otherwise
     */
    bool revokePermissionFromRole(Role role, Permission permission);

    /**
     * @brief Get permissions for role
     * @param role Role
     * @return Set of permissions
     */
    QSet<Permission> getPermissionsForRole(Role role) const;

Q_SIGNALS:
    /**
     * @brief Emitted when role is assigned
     */
    void roleAssigned(const QString& personnelId, Role role);

    /**
     * @brief Emitted when permission is granted
     */
    void permissionGranted(Role role, Permission permission);

private:
    QMap<QString, Role> m_personnelRoles;
    QMap<Role, QSet<Permission>> m_rolePermissions;
    void initializeDefaultPermissions();
};

#endif // ROLE_MANAGER_H

