#include "device_manager.h"
#include "biometric_abstraction.h"
#include <QDebug>

DeviceManager::DeviceManager(QObject* parent)
    : QObject(parent)
    , m_monitoring(false)
    , m_monitorTimer(new QTimer(this))
{
    connect(m_monitorTimer, &QTimer::timeout, this, &DeviceManager::checkForDevices);
    m_monitorTimer->setInterval(5000);  // Check every 5 seconds
}

DeviceManager::~DeviceManager() {
    stopMonitoring();
}

bool DeviceManager::initialize() {
    // Detect and register available devices
    QList<QString> faceDevices = BiometricDeviceFactory::detectDevices(BiometricDeviceType::FaceRecognition);
    for (const QString& deviceId : faceDevices) {
        auto device = BiometricDeviceFactory::createFaceRecognitionDevice(deviceId);
        if (device) {
            if (device->initialize()) {
                registerDevice(std::move(device));
            }
        }
    }
    
    QList<QString> fingerprintDevices = BiometricDeviceFactory::detectDevices(BiometricDeviceType::Fingerprint);
    for (const QString& deviceId : fingerprintDevices) {
        auto device = BiometricDeviceFactory::createFingerprintDevice(deviceId);
        if (device) {
            if (device->initialize()) {
                registerDevice(std::move(device));
            }
        }
    }
    
    return true;
}

void DeviceManager::startMonitoring() {
    if (m_monitoring) {
        return;
    }
    
    m_monitoring = true;
    m_monitorTimer->start();
}

void DeviceManager::stopMonitoring() {
    if (!m_monitoring) {
        return;
    }
    
    m_monitoring = false;
    m_monitorTimer->stop();
}

QList<QString> DeviceManager::detectDevices(BiometricDeviceType type) {
    return BiometricDeviceFactory::detectDevices(type);
}

bool DeviceManager::registerDevice(std::unique_ptr<IBiometricDevice> device) {
    if (!device) {
        return false;
    }
    
    QString deviceId = device->deviceId();
    
    if (m_devices.contains(deviceId)) {
        qWarning() << "Device already registered:" << deviceId;
        return false;
    }
    
    IBiometricDevice* devicePtr = device.get();
    connect(devicePtr, &IBiometricDevice::statusChanged, this, &DeviceManager::onDeviceStatusChanged);
    connect(devicePtr, &IBiometricDevice::errorOccurred, this, [this, deviceId](const QString& error) {
        emit deviceError(deviceId, error);
    });
    
    m_devices[deviceId] = std::move(device);
    emit deviceConnected(deviceId);
    
    return true;
}

void DeviceManager::unregisterDevice(const QString& deviceId) {
    if (m_devices.contains(deviceId)) {
        m_devices.remove(deviceId);
        emit deviceDisconnected(deviceId);
    }
}

IBiometricDevice* DeviceManager::getDevice(const QString& deviceId) const {
    if (m_devices.contains(deviceId)) {
        return m_devices[deviceId].get();
    }
    return nullptr;
}

QList<QString> DeviceManager::getRegisteredDevices() const {
    return m_devices.keys();
}

QList<QString> DeviceManager::getDevicesByType(BiometricDeviceType type) const {
    QList<QString> devices;
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if (it.value()->deviceType() == type) {
            devices.append(it.key());
        }
    }
    return devices;
}

void DeviceManager::checkForDevices() {
    // Detect new devices
    QList<QString> faceDevices = detectDevices(BiometricDeviceType::FaceRecognition);
    for (const QString& deviceId : faceDevices) {
        if (!m_devices.contains(deviceId)) {
            auto device = BiometricDeviceFactory::createFaceRecognitionDevice(deviceId);
            if (device && device->initialize()) {
                registerDevice(std::move(device));
            }
        }
    }
    
    QList<QString> fingerprintDevices = detectDevices(BiometricDeviceType::Fingerprint);
    for (const QString& deviceId : fingerprintDevices) {
        if (!m_devices.contains(deviceId)) {
            auto device = BiometricDeviceFactory::createFingerprintDevice(deviceId);
            if (device && device->initialize()) {
                registerDevice(std::move(device));
            }
        }
    }
    
    // Check for disconnected devices
    QList<QString> disconnectedDevices;
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if (!it.value()->isConnected()) {
            disconnectedDevices.append(it.key());
        }
    }
    
    for (const QString& deviceId : disconnectedDevices) {
        unregisterDevice(deviceId);
    }
}

void DeviceManager::onDeviceStatusChanged(DeviceStatus status) {
    IBiometricDevice* device = qobject_cast<IBiometricDevice*>(sender());
    if (device) {
        if (status == DeviceStatus::Connected) {
            emit deviceConnected(device->deviceId());
        } else if (status == DeviceStatus::Disconnected || status == DeviceStatus::Error) {
            emit deviceDisconnected(device->deviceId());
        }
    }
}

void DeviceManager::onDeviceError(const QString& error) {
    IBiometricDevice* device = qobject_cast<IBiometricDevice*>(sender());
    if (device) {
        emit deviceError(device->deviceId(), error);
    }
}


