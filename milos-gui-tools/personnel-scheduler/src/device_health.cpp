#include "device_health.h"
#include <QDebug>

DeviceHealthMonitor::DeviceHealthMonitor(QObject* parent)
    : QObject(parent)
    , m_healthCheckTimer(new QTimer(this))
    , m_healthCheckInterval(10000)  // Check every 10 seconds
{
    connect(m_healthCheckTimer, &QTimer::timeout, this, &DeviceHealthMonitor::checkDeviceHealth);
    m_healthCheckTimer->setInterval(m_healthCheckInterval);
}

DeviceHealthMonitor::~DeviceHealthMonitor() {
    stopMonitoring();
}

void DeviceHealthMonitor::startMonitoring(const QString& deviceId, IBiometricDevice* device) {
    if (!device) {
        return;
    }
    
    DeviceMonitorInfo info;
    info.device = device;
    info.lastHealth = device->getHealth();
    info.errorCount = 0;
    
    m_monitoredDevices[deviceId] = info;
    
    if (!m_healthCheckTimer->isActive()) {
        m_healthCheckTimer->start();
    }
}

void DeviceHealthMonitor::stopMonitoring(const QString& deviceId) {
    m_monitoredDevices.remove(deviceId);
    
    if (m_monitoredDevices.isEmpty()) {
        m_healthCheckTimer->stop();
    }
}

DeviceHealth DeviceHealthMonitor::getHealth(const QString& deviceId) const {
    if (m_monitoredDevices.contains(deviceId)) {
        return m_monitoredDevices[deviceId].lastHealth;
    }
    
    DeviceHealth health;
    health.status = DeviceStatus::Disconnected;
    health.healthScore = 0;
    health.errorMessage = "Device not monitored";
    return health;
}

QList<QString> DeviceHealthMonitor::getMonitoredDevices() const {
    return m_monitoredDevices.keys();
}

void DeviceHealthMonitor::checkDeviceHealth() {
    for (auto it = m_monitoredDevices.begin(); it != m_monitoredDevices.end(); ++it) {
        QString deviceId = it.key();
        DeviceMonitorInfo& info = it.value();
        
        if (!info.device) {
            continue;
        }
        
        DeviceHealth currentHealth = info.device->getHealth();
        DeviceHealth lastHealth = info.lastHealth;
        
        // Check for status changes
        if (currentHealth.status != lastHealth.status) {
            emit healthStatusChanged(deviceId, currentHealth);
        }
        
        // Check for errors
        if (currentHealth.status == DeviceStatus::Error) {
            info.errorCount++;
            if (info.errorCount > 3) {
                emit deviceErrorDetected(deviceId, currentHealth.errorMessage);
                info.errorCount = 0;  // Reset after reporting
            }
        } else {
            info.errorCount = 0;
        }
        
        // Check for health score degradation
        if (currentHealth.healthScore < lastHealth.healthScore && 
            currentHealth.healthScore < 50) {
            emit deviceErrorDetected(deviceId, "Device health degraded");
        }
        
        info.lastHealth = currentHealth;
    }
}

