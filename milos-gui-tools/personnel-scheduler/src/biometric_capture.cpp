#include "biometric_capture.h"
#include <QDebug>

BiometricCapture::BiometricCapture(DeviceManager* deviceManager, QObject* parent)
    : QObject(parent)
    , m_deviceManager(deviceManager)
{
}

BiometricCapture::~BiometricCapture() {
}

QByteArray BiometricCapture::capture(const QString& deviceId, int timeoutMs) {
    if (!m_deviceManager) {
        emit captureFailed("Device manager not available");
        return QByteArray();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        emit captureFailed("Device not found: " + deviceId);
        return QByteArray();
    }
    
    if (!device->isConnected()) {
        emit captureFailed("Device not connected: " + deviceId);
        return QByteArray();
    }
    
    QByteArray data = device->capture(timeoutMs);
    if (data.isEmpty()) {
        emit captureFailed("Failed to capture biometric data");
        return QByteArray();
    }
    
    emit biometricCaptured(data);
    return data;
}

int BiometricCapture::verify(const QString& deviceId, const QByteArray& templateData, const QByteArray& capturedData) {
    if (!m_deviceManager) {
        return 0;
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device || !device->isConnected()) {
        return 0;
    }
    
    return device->verify(templateData, capturedData);
}

QByteArray BiometricCapture::enroll(const QString& deviceId, const QByteArray& capturedData) {
    if (!m_deviceManager) {
        return QByteArray();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device || !device->isConnected()) {
        return QByteArray();
    }
    
    return device->enroll(capturedData);
}

