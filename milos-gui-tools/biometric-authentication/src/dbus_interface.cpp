#include "dbus_interface.h"
#include "biometric_authentication_system.h"
#include "biometric_authenticator.h"
#include "mfa_manager.h"
#include <QDebug>
#include <QDBusConnection>

BiometricAuthenticationDBusInterface::BiometricAuthenticationDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

BiometricAuthenticationDBusInterface::~BiometricAuthenticationDBusInterface() {
}

bool BiometricAuthenticationDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.BiometricAuthentication")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/BiometricAuthentication", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void BiometricAuthenticationDBusInterface::setSystem(BiometricAuthenticationSystem* system) {
    m_system = system;
}

bool BiometricAuthenticationDBusInterface::EnrollUser(const QString& userId, int biometricType) {
    if (!m_system || !m_system->getAuthenticator()) {
        return false;
    }
    
    QByteArray templateData = m_system->getAuthenticator()->enrollUser(userId, static_cast<BiometricType>(biometricType));
    return !templateData.isEmpty();
}

bool BiometricAuthenticationDBusInterface::AuthenticateUser(const QString& userId, int biometricType) {
    if (!m_system || !m_system->getAuthenticator()) {
        return false;
    }
    
    AuthenticationResult result = m_system->getAuthenticator()->authenticateUser(userId, static_cast<BiometricType>(biometricType));
    return (result == AuthenticationResult::Success);
}

bool BiometricAuthenticationDBusInterface::IsUserEnrolled(const QString& userId, int biometricType) {
    if (!m_system || !m_system->getAuthenticator()) {
        return false;
    }
    
    return m_system->getAuthenticator()->isUserEnrolled(userId, static_cast<BiometricType>(biometricType));
}

QStringList BiometricAuthenticationDBusInterface::GetEnrolledTypes(const QString& userId) {
    QStringList result;
    
    if (!m_system || !m_system->getAuthenticator()) {
        return result;
    }
    
    QList<BiometricType> types = m_system->getAuthenticator()->getEnrolledTypes(userId);
    for (BiometricType type : types) {
        result.append(QString::number(static_cast<int>(type)));
    }
    
    return result;
}

bool BiometricAuthenticationDBusInterface::AuthenticateMFA(const QString& userId, const QStringList& factors) {
    if (!m_system || !m_system->getMFAManager()) {
        return false;
    }
    
    QList<MFAFactorType> factorTypes;
    for (const QString& factor : factors) {
        if (factor == "biometric") {
            factorTypes.append(MFAFactorType::Biometric);
        } else if (factor == "token") {
            factorTypes.append(MFAFactorType::HardwareToken);
        } else if (factor == "password") {
            factorTypes.append(MFAFactorType::Password);
        } else if (factor == "pin") {
            factorTypes.append(MFAFactorType::PIN);
        }
    }
    
    return m_system->getMFAManager()->authenticate(userId, factorTypes);
}

