#include "biometric_device.h"
#include <QDebug>
#include <QRandomGenerator>

BiometricDevice::BiometricDevice(QObject* parent)
    : QObject(parent)
    , m_connected(false)
{
}

BiometricDevice::~BiometricDevice() {
}

bool BiometricDevice::initialize(const QString& deviceId, const QString& deviceName, BiometricType biometricType) {
    m_deviceId = deviceId;
    m_deviceName = deviceName;
    m_biometricType = biometricType;
    return true;
}

bool BiometricDevice::connectDevice() {
    if (m_connected) {
        return true;
    }
    
    // In production, would connect to actual hardware
    // For now, simulate connection
    m_connected = true;
    emit connectionChanged(true);
    
    return true;
}

void BiometricDevice::disconnectDevice() {
    if (!m_connected) {
        return;
    }
    
    m_connected = false;
    emit connectionChanged(false);
}

QByteArray BiometricDevice::captureSample() {
    if (!m_connected) {
        return QByteArray();
    }
    
    // In production, would capture from actual hardware
    // For now, generate simulated biometric data
    QByteArray sample;
    sample.resize(256);
    for (int i = 0; i < 256; ++i) {
        sample[i] = QRandomGenerator::global()->bounded(256);
    }
    
    emit sampleCaptured(sample);
    
    return sample;
}

bool BiometricDevice::verifySample(const QByteArray& sample, const QByteArray& templateData) {
    if (sample.isEmpty() || templateData.isEmpty()) {
        return false;
    }
    
    // In production, would use proper biometric matching algorithm
    // For now, simple comparison
    if (sample.size() != templateData.size()) {
        return false;
    }
    
    // Simulate matching (90% match threshold)
    int matches = 0;
    for (int i = 0; i < sample.size(); ++i) {
        if (qAbs(sample[i] - templateData[i]) < 25) {  // Allow small variance
            matches++;
        }
    }
    
    double matchRatio = static_cast<double>(matches) / sample.size();
    return matchRatio >= 0.9;
}

