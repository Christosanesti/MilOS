#include "secure_messenger.h"
#include <QDebug>

SecureMessenger::SecureMessenger(QObject* parent)
    : QObject(parent)
    , m_usbAuth(new USBAuthorization(this))
    , m_keyGen(new KeyGenerator(this))
    , m_keyMgr(new KeyManager(this))
    , m_enrollment(new UserEnrollment(this))
    , m_roleMgr(new RoleManager(this))
    , m_dbusInterface(new SecureMessengerDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
    , m_running(false)
{
}

SecureMessenger::~SecureMessenger() {
    stop();
}

bool SecureMessenger::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize components
    if (!m_usbAuth->initialize()) {
        qWarning() << "Failed to initialize USB authorization";
        return false;
    }

    if (!m_keyGen->initialize()) {
        qWarning() << "Failed to initialize key generator";
        return false;
    }

    if (!m_keyMgr->initialize()) {
        qWarning() << "Failed to initialize key manager";
        return false;
    }

    if (!m_enrollment->initialize()) {
        qWarning() << "Failed to initialize user enrollment";
        return false;
    }

    if (!m_roleMgr->initialize()) {
        qWarning() << "Failed to initialize role manager";
        return false;
    }

    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }

    // Connect signals for audit logging
    connect(m_keyGen, &KeyGenerator::keyGenerated, this, [this](const QString& keyId) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        m_auditLogger->logKeyOperation("key_generated", QString(), eventData);
    });

    connect(m_keyGen, &KeyGenerator::keyWrittenToUSB, this, [this](const QString& keyId, const QString& devicePath) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        eventData["device_path"] = devicePath;
        m_auditLogger->logKeyOperation("key_written_to_usb", QString(), eventData);
    });

    connect(m_keyMgr, &KeyManager::keyDistributed, this, [this](const QString& keyId, const QString& devicePath) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        eventData["device_path"] = devicePath;
        m_auditLogger->logKeyOperation("key_distributed", QString(), eventData);
    });

    connect(m_keyMgr, &KeyManager::keyRevoked, this, [this](const QString& keyId) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        m_auditLogger->logKeyOperation("key_revoked", QString(), eventData);
    });

    connect(m_enrollment, &UserEnrollment::userEnrolled, this, [this](const QString& userId) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        m_auditLogger->logKeyOperation("user_enrolled", userId, eventData);
    });

    connect(m_enrollment, &UserEnrollment::enrollmentApproved, this, [this](const QString& userId) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        m_auditLogger->logKeyOperation("enrollment_approved", userId, eventData);
    });

    connect(m_enrollment, &UserEnrollment::enrollmentRejected, this, [this](const QString& userId, const QString& reason) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        eventData["reason"] = reason;
        m_auditLogger->logKeyOperation("enrollment_rejected", userId, eventData);
    });

    // Initialize D-Bus interface
    m_dbusInterface->setUSBAuthorization(m_usbAuth);
    m_dbusInterface->setKeyGenerator(m_keyGen);
    m_dbusInterface->setKeyManager(m_keyMgr);
    m_dbusInterface->setUserEnrollment(m_enrollment);
    m_dbusInterface->setRoleManager(m_roleMgr);

    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }

    m_initialized = true;
    return true;
}

bool SecureMessenger::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    m_running = true;
    return true;
}

void SecureMessenger::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
}

