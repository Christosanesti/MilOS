#include "biometric_authenticator.h"
#include <QDebug>

BiometricAuthenticator::BiometricAuthenticator(QObject* parent)
    : QObject(parent)
{
}

BiometricAuthenticator::~BiometricAuthenticator() {
}

bool BiometricAuthenticator::initialize() {
    return true;
}

bool BiometricAuthenticator::addDevice(BiometricDevice* device) {
    if (!device) {
        return false;
    }
    
    m_devices.append(device);
    return true;
}

QByteArray BiometricAuthenticator::enrollUser(const QString& userId, BiometricType biometricType) {
    BiometricDevice* device = getDeviceForType(biometricType);
    if (!device) {
        return QByteArray();
    }
    
    if (!device->isConnected()) {
        if (!device->connectDevice()) {
            return QByteArray();
        }
    }
    
    // Capture sample for enrollment
    QByteArray sample = device->captureSample();
    if (sample.isEmpty()) {
        return QByteArray();
    }
    
    // Store template
    m_userTemplates[userId][biometricType] = sample;
    
    emit userEnrolled(userId, biometricType);
    
    return sample;
}

AuthenticationResult BiometricAuthenticator::authenticateUser(const QString& userId, BiometricType biometricType) {
    if (!isUserEnrolled(userId, biometricType)) {
        return AuthenticationResult::NotEnrolled;
    }
    
    BiometricDevice* device = getDeviceForType(biometricType);
    if (!device) {
        return AuthenticationResult::DeviceError;
    }
    
    if (!device->isConnected()) {
        if (!device->connectDevice()) {
            return AuthenticationResult::DeviceError;
        }
    }
    
    // Capture sample for authentication
    QByteArray sample = device->captureSample();
    if (sample.isEmpty()) {
        return AuthenticationResult::DeviceError;
    }
    
    // Get stored template
    QByteArray templateData = m_userTemplates[userId][biometricType];
    
    // Verify sample
    bool match = device->verifySample(sample, templateData);
    
    AuthenticationResult result = match ? AuthenticationResult::Success : AuthenticationResult::Failed;
    emit authenticationCompleted(userId, result);
    
    return result;
}

bool BiometricAuthenticator::isUserEnrolled(const QString& userId, BiometricType biometricType) const {
    return m_userTemplates.contains(userId) && 
           m_userTemplates[userId].contains(biometricType);
}

QList<BiometricType> BiometricAuthenticator::getEnrolledTypes(const QString& userId) const {
    if (!m_userTemplates.contains(userId)) {
        return QList<BiometricType>();
    }
    
    return m_userTemplates[userId].keys();
}

BiometricDevice* BiometricAuthenticator::getDeviceForType(BiometricType type) const {
    for (BiometricDevice* device : m_devices) {
        if (device->biometricType() == type) {
            return device;
        }
    }
    return nullptr;
}

