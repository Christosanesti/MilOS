#include "biometric_authentication_system.h"
#include "biometric_device.h"
#include <QDebug>

BiometricAuthenticationSystem::BiometricAuthenticationSystem(QObject* parent)
    : QObject(parent)
    , m_authenticator(new BiometricAuthenticator(this))
    , m_mfaManager(new MFAManager(this))
    , m_dbusInterface(new BiometricAuthenticationDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

BiometricAuthenticationSystem::~BiometricAuthenticationSystem() {
}

bool BiometricAuthenticationSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize authenticator
    if (!m_authenticator->initialize()) {
        qWarning() << "Failed to initialize biometric authenticator";
        return false;
    }
    
    // Initialize MFA manager
    if (!m_mfaManager->initialize(m_authenticator)) {
        qWarning() << "Failed to initialize MFA manager";
        return false;
    }
    
    // Add sample devices
    BiometricDevice* fingerprintDevice = new BiometricDevice(this);
    fingerprintDevice->initialize("fingerprint-001", "Fingerprint Scanner", BiometricType::Fingerprint);
    m_authenticator->addDevice(fingerprintDevice);
    
    BiometricDevice* faceDevice = new BiometricDevice(this);
    faceDevice->initialize("face-001", "Face Recognition Camera", BiometricType::Face);
    m_authenticator->addDevice(faceDevice);
    
    // Connect signals for audit logging
    connect(m_authenticator, &BiometricAuthenticator::userEnrolled, this, [this](const QString& userId, BiometricType type) {
        QString typeStr;
        switch (type) {
            case BiometricType::Fingerprint: typeStr = "Fingerprint"; break;
            case BiometricType::Iris: typeStr = "Iris"; break;
            case BiometricType::Voice: typeStr = "Voice"; break;
            case BiometricType::Face: typeStr = "Face"; break;
        }
        m_auditLogger->logUserEnrolled(userId, typeStr);
    });
    
    connect(m_authenticator, &BiometricAuthenticator::authenticationCompleted, this, [this](const QString& userId, AuthenticationResult result) {
        m_auditLogger->logAuthenticationAttempt(userId, result == AuthenticationResult::Success);
    });
    
    connect(m_mfaManager, &MFAManager::mfaAuthenticationCompleted, this, [this](const QString& userId, bool success) {
        m_auditLogger->logMFAAuthentication(userId, success);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setSystem(this);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

