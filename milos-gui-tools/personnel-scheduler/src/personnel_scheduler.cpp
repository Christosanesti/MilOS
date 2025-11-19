#include "personnel_scheduler.h"
#include <QDebug>

PersonnelScheduler::PersonnelScheduler(QObject* parent)
    : QObject(parent)
    , m_deviceManager(new DeviceManager(this))
    , m_healthMonitor(new DeviceHealthMonitor(this))
    , m_dbusInterface(new PersonnelSchedulerDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_configParser(new ConfigParser())
    , m_initialized(false)
    , m_running(false)
{
}

PersonnelScheduler::~PersonnelScheduler() {
    stop();
}

bool PersonnelScheduler::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Load configuration
    QString configPath = "/etc/milos/personnel-scheduler/config.yaml";
    if (!m_configParser->load(configPath)) {
        qWarning() << "Failed to load configuration, using defaults";
    }
    
    // Initialize device manager
    if (!m_deviceManager->initialize()) {
        qWarning() << "Failed to initialize device manager";
        return false;
    }
    
    // Set up device health monitoring
    QList<QString> devices = m_deviceManager->getRegisteredDevices();
    for (const QString& deviceId : devices) {
        IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
        if (device) {
            m_healthMonitor->startMonitoring(deviceId, device);
        }
    }
    
    // Connect device manager signals to health monitor
    connect(m_deviceManager, &DeviceManager::deviceConnected, this, [this](const QString& deviceId) {
        IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
        if (device) {
            m_healthMonitor->startMonitoring(deviceId, device);
        }
        m_auditLogger->logDeviceOperation("device_connected", deviceId);
    });
    
    connect(m_deviceManager, &DeviceManager::deviceDisconnected, this, [this](const QString& deviceId) {
        m_healthMonitor->stopMonitoring(deviceId);
        m_auditLogger->logDeviceOperation("device_disconnected", deviceId);
    });
    
    connect(m_deviceManager, &DeviceManager::deviceError, this, [this](const QString& deviceId, const QString& error) {
        QVariantMap eventData;
        eventData["error"] = error;
        m_auditLogger->logDeviceOperation("device_error", deviceId, eventData);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setDeviceManager(m_deviceManager);
    m_dbusInterface->setDeviceHealthMonitor(m_healthMonitor);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

bool PersonnelScheduler::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }
    
    if (m_running) {
        return true;
    }
    
    // Start device monitoring
    m_deviceManager->startMonitoring();
    
    m_running = true;
    return true;
}

void PersonnelScheduler::stop() {
    if (!m_running) {
        return;
    }
    
    // Stop device monitoring
    m_deviceManager->stopMonitoring();
    
    m_running = false;
}

