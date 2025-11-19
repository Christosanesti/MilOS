#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QVariantMap>

class AccessRestrictionsManager;
class RoleManager;

/**
 * @brief Access Result
 */
enum class AccessResult {
    Granted,
    Denied,
    TimeRestricted,
    LocationRestricted,
    RoleRestricted
};

/**
 * @brief Access Request
 */
struct AccessRequest {
    QString requestId;
    QString personnelId;
    QString location;
    QDateTime requestTime;
    QString deviceId;
    QByteArray biometricData;
};

/**
 * @brief Access Control
 * 
 * Manages access control and security features.
 */
class AccessControl : public QObject {
    Q_OBJECT

public:
    explicit AccessControl(QObject* parent = nullptr);
    ~AccessControl();

    /**
     * @brief Initialize access control
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Request access
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param deviceId Device ID
     * @param biometricData Biometric data
     * @return Access result
     */
    AccessResult requestAccess(const QString& personnelId,
                              const QString& location,
                              const QString& deviceId,
                              const QByteArray& biometricData);

    /**
     * @brief Check access permission
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param requestTime Request time
     * @return true if access granted, false otherwise
     */
    bool checkAccessPermission(const QString& personnelId,
                               const QString& location,
                               const QDateTime& requestTime) const;

    /**
     * @brief Grant access permission
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param startTime Start time (optional)
     * @param endTime End time (optional)
     * @return true if grant successful, false otherwise
     */
    bool grantAccessPermission(const QString& personnelId,
                              const QString& location,
                              const QDateTime& startTime = QDateTime(),
                              const QDateTime& endTime = QDateTime());

    /**
     * @brief Revoke access permission
     * @param personnelId Personnel ID
     * @param location Location/area
     * @return true if revoke successful, false otherwise
     */
    bool revokeAccessPermission(const QString& personnelId, const QString& location);

    /**
     * @brief Get access permissions
     * @param personnelId Optional personnel ID filter
     * @param location Optional location filter
     * @return List of access permissions
     */
    QList<QVariantMap> getAccessPermissions(const QString& personnelId = QString(),
                                            const QString& location = QString()) const;

    /**
     * @brief Set restrictions manager
     */
    void setRestrictionsManager(AccessRestrictionsManager* manager);

    /**
     * @brief Set role manager
     */
    void setRoleManager(RoleManager* manager);

Q_SIGNALS:
    /**
     * @brief Emitted when access is granted
     */
    void accessGranted(const QString& personnelId, const QString& location, const QDateTime& time);

    /**
     * @brief Emitted when access is denied
     */
    void accessDenied(const QString& personnelId, const QString& location, const QString& reason, const QDateTime& time);

private:
    bool checkTimeRestrictions(const QString& personnelId, const QString& location, const QDateTime& requestTime) const;
    bool checkLocationRestrictions(const QString& personnelId, const QString& location) const;
    bool checkRoleRestrictions(const QString& personnelId, const QString& location) const;
    
    QList<QVariantMap> m_accessPermissions;
    AccessRestrictionsManager* m_restrictionsManager;
    RoleManager* m_roleManager;
    QString generateRequestId() const;
};

#endif // ACCESS_CONTROL_H

