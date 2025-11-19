#include "user_enrollment.h"
#include <QUuid>
#include <QDebug>

UserEnrollment::UserEnrollment(QObject* parent)
    : QObject(parent)
{
}

UserEnrollment::~UserEnrollment() {
}

bool UserEnrollment::initialize() {
    return true;
}

UserInfo UserEnrollment::enrollUser(const QString& username,
                                   const QString& email,
                                   UserRole role,
                                   const QString& enrolledBy) {
    UserInfo user;
    user.userId = generateUserId();
    user.username = username;
    user.email = email;
    user.role = role;
    user.status = EnrollmentStatus::Pending;
    user.enrolledAt = QDateTime::currentDateTime();
    user.enrolledBy = enrolledBy;
    
    m_users.append(user);
    
    emit userEnrolled(user.userId);
    
    return user;
}

bool UserEnrollment::approveEnrollment(const QString& userId, const QString& approverId) {
    for (int i = 0; i < m_users.size(); i++) {
        if (m_users[i].userId == userId) {
            m_users[i].status = EnrollmentStatus::Approved;
            emit enrollmentApproved(userId);
            return true;
        }
    }
    
    return false;
}

bool UserEnrollment::rejectEnrollment(const QString& userId, const QString& reason) {
    for (int i = 0; i < m_users.size(); i++) {
        if (m_users[i].userId == userId) {
            m_users[i].status = EnrollmentStatus::Rejected;
            emit enrollmentRejected(userId, reason);
            return true;
        }
    }
    
    return false;
}

UserInfo UserEnrollment::getUserInfo(const QString& userId) const {
    for (const UserInfo& user : m_users) {
        if (user.userId == userId) {
            return user;
        }
    }
    
    return UserInfo();
}

QList<UserInfo> UserEnrollment::getAllUsers(UserRole role) const {
    if (role == UserRole::User) {
        return m_users;
    }
    
    QList<UserInfo> filtered;
    for (const UserInfo& user : m_users) {
        if (user.role == role) {
            filtered.append(user);
        }
    }
    
    return filtered;
}

bool UserEnrollment::isUserEnrolled(const QString& userId) const {
    for (const UserInfo& user : m_users) {
        if (user.userId == userId && user.status == EnrollmentStatus::Active) {
            return true;
        }
    }
    
    return false;
}

QString UserEnrollment::generateUserId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

