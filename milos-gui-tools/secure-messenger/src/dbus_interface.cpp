#include "dbus_interface.h"
#include "usb_authorization.h"
#include "key_generator.h"
#include "key_manager.h"
#include "user_enrollment.h"
#include "role_manager.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SecureMessengerDBusInterface::SecureMessengerDBusInterface(QObject* parent)
    : QObject(parent)
    , m_usbAuth(nullptr)
    , m_keyGen(nullptr)
    , m_keyMgr(nullptr)
    , m_enrollment(nullptr)
    , m_roleMgr(nullptr)
    , m_initialized(false)
{
}

SecureMessengerDBusInterface::~SecureMessengerDBusInterface() {
}

void SecureMessengerDBusInterface::setUSBAuthorization(USBAuthorization* usbAuth) {
    m_usbAuth = usbAuth;
}

void SecureMessengerDBusInterface::setKeyGenerator(KeyGenerator* keyGen) {
    m_keyGen = keyGen;
}

void SecureMessengerDBusInterface::setKeyManager(KeyManager* keyMgr) {
    m_keyMgr = keyMgr;
}

void SecureMessengerDBusInterface::setUserEnrollment(UserEnrollment* enrollment) {
    m_enrollment = enrollment;
}

void SecureMessengerDBusInterface::setRoleManager(RoleManager* roleMgr) {
    m_roleMgr = roleMgr;
}

bool SecureMessengerDBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }

    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SecureMessenger", this,
                                                       QDBusConnection::ExportAllSlots |
                                                       QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }

    if (!QDBusConnection::sessionBus().registerService("org.milos.SecureMessenger")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }

    m_initialized = true;
    return true;
}

QString SecureMessengerDBusInterface::DetectUSBDevices() {
    if (!m_usbAuth) {
        return QString();
    }

    QList<QString> devices = m_usbAuth->detectUSBDevices();
    
    QJsonArray jsonArray;
    for (const QString& device : devices) {
        jsonArray.append(device);
    }
    
    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::ValidateUSBKey(const QString& devicePath) {
    if (!m_usbAuth) {
        return QString();
    }

    USBKeyInfo keyInfo = m_usbAuth->validateUSBKey(devicePath);
    if (keyInfo.keyId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["key_id"] = keyInfo.keyId;
    obj["user_id"] = keyInfo.userId;
    obj["status"] = static_cast<int>(keyInfo.status);
    obj["created_at"] = keyInfo.createdAt.toString(Qt::ISODate);
    if (keyInfo.expiresAt.isValid()) {
        obj["expires_at"] = keyInfo.expiresAt.toString(Qt::ISODate);
    }
    obj["role"] = static_cast<int>(keyInfo.role);

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GenerateKey(const QString& userId, int role, int expirationDays) {
    if (!m_keyGen) {
        return QString();
    }

    USBKeyInfo keyInfo = m_keyGen->generateKey(userId, static_cast<UserRole>(role), expirationDays);
    return keyInfo.keyId;
}

bool SecureMessengerDBusInterface::WriteKeyToUSB(const QString& keyId, const QString& devicePath) {
    if (!m_keyGen || !m_keyMgr) {
        return false;
    }

    USBKeyInfo keyInfo = m_keyMgr->getKeyInfo(keyId);
    if (keyInfo.keyId.isEmpty()) {
        return false;
    }

    return m_keyGen->writeKeyToUSB(keyInfo, devicePath);
}

bool SecureMessengerDBusInterface::DistributeKey(const QString& keyId, const QString& devicePath) {
    if (!m_keyMgr) {
        return false;
    }

    USBKeyInfo keyInfo = m_keyMgr->getKeyInfo(keyId);
    if (keyInfo.keyId.isEmpty()) {
        return false;
    }

    return m_keyMgr->distributeKey(keyInfo, devicePath);
}

bool SecureMessengerDBusInterface::RevokeKey(const QString& keyId) {
    if (!m_keyMgr) {
        return false;
    }

    return m_keyMgr->revokeKey(keyId);
}

QString SecureMessengerDBusInterface::GetKeyInfo(const QString& keyId) {
    if (!m_keyMgr) {
        return QString();
    }

    USBKeyInfo keyInfo = m_keyMgr->getKeyInfo(keyId);
    if (keyInfo.keyId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["key_id"] = keyInfo.keyId;
    obj["user_id"] = keyInfo.userId;
    obj["status"] = static_cast<int>(keyInfo.status);
    obj["created_at"] = keyInfo.createdAt.toString(Qt::ISODate);
    if (keyInfo.expiresAt.isValid()) {
        obj["expires_at"] = keyInfo.expiresAt.toString(Qt::ISODate);
    }
    obj["role"] = static_cast<int>(keyInfo.role);

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetAllKeys(const QString& userId) {
    if (!m_keyMgr) {
        return QString();
    }

    QList<USBKeyInfo> keys = m_keyMgr->getAllKeys(userId.isEmpty() ? QString() : userId);
    
    QJsonArray jsonArray;
    for (const USBKeyInfo& key : keys) {
        QJsonObject obj;
        obj["key_id"] = key.keyId;
        obj["user_id"] = key.userId;
        obj["status"] = static_cast<int>(key.status);
        obj["created_at"] = key.createdAt.toString(Qt::ISODate);
        if (key.expiresAt.isValid()) {
            obj["expires_at"] = key.expiresAt.toString(Qt::ISODate);
        }
        obj["role"] = static_cast<int>(key.role);
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::EnrollUser(const QString& username, const QString& email, int role, const QString& enrolledBy) {
    if (!m_enrollment) {
        return QString();
    }

    UserInfo user = m_enrollment->enrollUser(username, email, static_cast<UserRole>(role), enrolledBy);
    return user.userId;
}

bool SecureMessengerDBusInterface::ApproveEnrollment(const QString& userId, const QString& approverId) {
    if (!m_enrollment) {
        return false;
    }

    return m_enrollment->approveEnrollment(userId, approverId);
}

bool SecureMessengerDBusInterface::RejectEnrollment(const QString& userId, const QString& reason) {
    if (!m_enrollment) {
        return false;
    }

    return m_enrollment->rejectEnrollment(userId, reason);
}

QString SecureMessengerDBusInterface::GetUserInfo(const QString& userId) {
    if (!m_enrollment) {
        return QString();
    }

    UserInfo user = m_enrollment->getUserInfo(userId);
    if (user.userId.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["user_id"] = user.userId;
    obj["username"] = user.username;
    obj["email"] = user.email;
    obj["role"] = static_cast<int>(user.role);
    obj["status"] = static_cast<int>(user.status);
    obj["enrolled_at"] = user.enrolledAt.toString(Qt::ISODate);
    obj["enrolled_by"] = user.enrolledBy;

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString SecureMessengerDBusInterface::GetAllUsers(int role) {
    if (!m_enrollment) {
        return QString();
    }

    QList<UserInfo> users = m_enrollment->getAllUsers(role < 0 ? UserRole::User : static_cast<UserRole>(role));
    
    QJsonArray jsonArray;
    for (const UserInfo& user : users) {
        QJsonObject obj;
        obj["user_id"] = user.userId;
        obj["username"] = user.username;
        obj["email"] = user.email;
        obj["role"] = static_cast<int>(user.role);
        obj["status"] = static_cast<int>(user.status);
        obj["enrolled_at"] = user.enrolledAt.toString(Qt::ISODate);
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

bool SecureMessengerDBusInterface::CheckPermission(const QString& userId, int permission) {
    if (!m_roleMgr) {
        return false;
    }

    return m_roleMgr->checkPermission(userId, static_cast<Permission>(permission));
}

