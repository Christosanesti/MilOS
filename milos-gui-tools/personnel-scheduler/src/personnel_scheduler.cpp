#include "personnel_scheduler.h"
#include "attendance_tracker.h"
#include "attendance_storage.h"
#include "audit_logger.h"
#include <QDebug>
#include <QStandardPaths>

PersonnelScheduler::PersonnelScheduler(QObject* parent)
    : QObject(parent)
    , m_deviceManager(new DeviceManager(this))
    , m_healthMonitor(new DeviceHealthMonitor(this))
    , m_attendanceTracker(new AttendanceTracker(m_deviceManager, this))
    , m_attendanceStorage(new AttendanceStorage(this))
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
    
    // Initialize attendance tracker
    if (!m_attendanceTracker->initialize()) {
        qWarning() << "Failed to initialize attendance tracker";
        return false;
    }
    
    // Initialize attendance storage
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/attendance.db";
    if (!m_attendanceStorage->initialize(dbPath)) {
        qWarning() << "Failed to initialize attendance storage";
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
    
    // Connect attendance tracker signals
    connect(m_attendanceTracker, &AttendanceTracker::attendanceRecorded, this, [this](const AttendanceRecord& record) {
        // Store record
        m_attendanceStorage->storeRecord(record);
        
        // Log to audit service
        QVariantMap eventData;
        eventData["record_id"] = record.recordId;
        eventData["personnel_id"] = record.personnelId;
        eventData["event_type"] = (record.eventType == AttendanceEventType::Entry) ? "entry" : "exit";
        eventData["location"] = record.location;
        m_auditLogger->logDeviceOperation("attendance_recorded", record.deviceId, eventData);
    });
    
    connect(m_attendanceTracker, &AttendanceTracker::attendanceValidationFailed, this, [this](const QString& recordId, const QString& reason) {
        QVariantMap eventData;
        eventData["record_id"] = recordId;
        eventData["reason"] = reason;
        m_auditLogger->logDeviceOperation("attendance_validation_failed", QString(), eventData);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setDeviceManager(m_deviceManager);
    m_dbusInterface->setDeviceHealthMonitor(m_healthMonitor);
    m_dbusInterface->setAttendanceTracker(m_attendanceTracker);
    
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
    
    // Start attendance tracking
    m_attendanceTracker->startTracking();
    
    m_running = true;
    return true;
}

void PersonnelScheduler::stop() {
    if (!m_running) {
        return;
    }
    
    // Stop device monitoring
    m_deviceManager->stopMonitoring();
    
    // Stop attendance tracking
    m_attendanceTracker->stopTracking();
    
    m_running = false;
}

