#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVariantMap>

/**
 * @brief User Status
 */
enum class UserStatus {
    Active,
    Inactive,
    Suspended,
    Revoked
};

/**
 * @brief User Manager
 * 
 * Provides user management interface for authorization controls.
 */
class UserManager : public QObject {
    Q_OBJECT

public:
    explicit UserManager(QObject* parent = nullptr);
    ~UserManager();

    /**
     * @brief Initialize user manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get all users
     * @return List of user information
     */
    QList<QVariantMap> getAllUsers() const;

    /**
     * @brief Get user info
     * @param userId User ID
     * @return User information or empty if not found
     */
    QVariantMap getUserInfo(const QString& userId) const;

    /**
     * @brief Update user status
     * @param userId User ID
     * @param status New status
     * @return true if update successful, false otherwise
     */
    bool updateUserStatus(const QString& userId, UserStatus status);

    /**
     * @brief Revoke user access
     * @param userId User ID
     * @return true if revoke successful, false otherwise
     */
    bool revokeUserAccess(const QString& userId);

    /**
     * @brief Restore user access
     * @param userId User ID
     * @return true if restore successful, false otherwise
     */
    bool restoreUserAccess(const QString& userId);

Q_SIGNALS:
    /**
     * @brief Emitted when user status is updated
     */
    void userStatusUpdated(const QString& userId, UserStatus status);

    /**
     * @brief Emitted when user access is revoked
     */
    void userAccessRevoked(const QString& userId);

    /**
     * @brief Emitted when user access is restored
     */
    void userAccessRestored(const QString& userId);

private:
    QMap<QString, QVariantMap> m_users;
};

#endif // USER_MANAGER_H

