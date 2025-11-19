#ifndef USER_ENROLLMENT_H
#define USER_ENROLLMENT_H

#include "usb_authorization.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QList>

/**
 * @brief User Enrollment Status
 */
enum class EnrollmentStatus {
    Pending,
    Approved,
    Rejected,
    Active,
    Suspended
};

/**
 * @brief User Information
 */
struct UserInfo {
    QString userId;
    QString username;
    QString email;
    UserRole role;
    EnrollmentStatus status;
    QDateTime enrolledAt;
    QDateTime lastActive;
    QString enrolledBy;
};

/**
 * @brief User Enrollment
 * 
 * Manages user enrollment and authorization within the secure messenger network.
 */
class UserEnrollment : public QObject {
    Q_OBJECT

public:
    explicit UserEnrollment(QObject* parent = nullptr);
    ~UserEnrollment();

    /**
     * @brief Initialize user enrollment
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Enroll user
     * @param username Username
     * @param email Email address
     * @param role User role
     * @param enrolledBy User ID of enrolling officer
     * @return User information if enrollment successful
     */
    UserInfo enrollUser(const QString& username,
                       const QString& email,
                       UserRole role,
                       const QString& enrolledBy);

    /**
     * @brief Approve enrollment
     * @param userId User ID
     * @param approverId Approver user ID
     * @return true if approval successful, false otherwise
     */
    bool approveEnrollment(const QString& userId, const QString& approverId);

    /**
     * @brief Reject enrollment
     * @param userId User ID
     * @param reason Rejection reason
     * @return true if rejection successful, false otherwise
     */
    bool rejectEnrollment(const QString& userId, const QString& reason);

    /**
     * @brief Get user information
     * @param userId User ID
     * @return User information or empty if not found
     */
    UserInfo getUserInfo(const QString& userId) const;

    /**
     * @brief Get all users
     * @param role Optional role filter
     * @return List of user information
     */
    QList<UserInfo> getAllUsers(UserRole role = UserRole::User) const;

    /**
     * @brief Check if user is enrolled
     * @param userId User ID
     * @return true if enrolled, false otherwise
     */
    bool isUserEnrolled(const QString& userId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when user is enrolled
     */
    void userEnrolled(const QString& userId);

    /**
     * @brief Emitted when enrollment is approved
     */
    void enrollmentApproved(const QString& userId);

    /**
     * @brief Emitted when enrollment is rejected
     */
    void enrollmentRejected(const QString& userId, const QString& reason);

private:
    QList<UserInfo> m_users;
    QString generateUserId() const;
};

#endif // USER_ENROLLMENT_H

