#include "personnel_scheduler.h"
#include "attendance_tracker.h"
#include "attendance_storage.h"
#include "shift_scheduler.h"
#include "conflict_detector.h"
#include "leave_manager.h"
#include "shift_swap.h"
#include "coverage_manager.h"
#include "access_control.h"
#include "access_restrictions.h"
#include "role_manager.h"
#include "report_generator.h"
#include "analytics_engine.h"
#include "export_manager.h"
#include "compliance_reporter.h"
#include "personnel_integration.h"
#include "audit_logger.h"
#include <QDebug>
#include <QStandardPaths>

PersonnelScheduler::PersonnelScheduler(QObject* parent)
    : QObject(parent)
    , m_deviceManager(new DeviceManager(this))
    , m_healthMonitor(new DeviceHealthMonitor(this))
    , m_attendanceTracker(new AttendanceTracker(m_deviceManager, this))
    , m_attendanceStorage(new AttendanceStorage(this))
    , m_shiftScheduler(new ShiftScheduler(this))
    , m_conflictDetector(new ConflictDetector(this))
    , m_leaveManager(new LeaveManager(this))
    , m_swapManager(new ShiftSwapManager(this))
    , m_coverageManager(new CoverageManager(this))
    , m_accessControl(new AccessControl(this))
    , m_restrictionsManager(new AccessRestrictionsManager(this))
    , m_roleManager(new RoleManager(this))
    , m_reportGenerator(new ReportGenerator(this))
    , m_analyticsEngine(new AnalyticsEngine(this))
    , m_exportManager(new ExportManager(this))
    , m_complianceReporter(new ComplianceReporter(this))
    , m_personnelIntegration(new PersonnelIntegration(this))
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
    
    // Initialize shift scheduler
    if (!m_shiftScheduler->initialize()) {
        qWarning() << "Failed to initialize shift scheduler";
        return false;
    }
    
    // Initialize access control
    if (!m_accessControl->initialize()) {
        qWarning() << "Failed to initialize access control";
        return false;
    }
    
    // Initialize restrictions manager
    if (!m_restrictionsManager->initialize()) {
        qWarning() << "Failed to initialize restrictions manager";
        return false;
    }
    
    // Initialize role manager
    if (!m_roleManager->initialize()) {
        qWarning() << "Failed to initialize role manager";
        return false;
    }
    
    // Initialize report generator
    if (!m_reportGenerator->initialize()) {
        qWarning() << "Failed to initialize report generator";
        return false;
    }
    
    // Initialize analytics engine
    if (!m_analyticsEngine->initialize()) {
        qWarning() << "Failed to initialize analytics engine";
        return false;
    }
    
    // Initialize export manager
    if (!m_exportManager->initialize()) {
        qWarning() << "Failed to initialize export manager";
        return false;
    }
    
    // Initialize compliance reporter
    if (!m_complianceReporter->initialize()) {
        qWarning() << "Failed to initialize compliance reporter";
        return false;
    }
    
    // Connect access control components
    m_accessControl->setRestrictionsManager(m_restrictionsManager);
    m_accessControl->setRoleManager(m_roleManager);
    
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
    
    // Connect shift scheduler signals
    connect(m_shiftScheduler, &ShiftScheduler::shiftCreated, this, [this](const QString& shiftId) {
        QVariantMap eventData;
        eventData["shift_id"] = shiftId;
        m_auditLogger->logDeviceOperation("shift_created", QString(), eventData);
    });
    
    connect(m_conflictDetector, &ConflictDetector::conflictDetected, this, [this](const Conflict conflict) {
        QVariantMap eventData;
        eventData["shift_id1"] = conflict.shiftId1;
        eventData["shift_id2"] = conflict.shiftId2;
        eventData["personnel_id"] = conflict.personnelId;
        eventData["description"] = conflict.description;
        m_auditLogger->logDeviceOperation("conflict_detected", QString(), eventData);
    });
    
    connect(m_leaveManager, &LeaveManager::leaveRequestCreated, this, [this](const QString& requestId) {
        QVariantMap eventData;
        eventData["request_id"] = requestId;
        m_auditLogger->logDeviceOperation("leave_request_created", QString(), eventData);
    });
    
    connect(m_swapManager, &ShiftSwapManager::swapRequestCreated, this, [this](const QString& swapId) {
        QVariantMap eventData;
        eventData["swap_id"] = swapId;
        m_auditLogger->logDeviceOperation("swap_request_created", QString(), eventData);
    });
    
    connect(m_accessControl, &AccessControl::accessGranted, this, [this](const QString& personnelId, const QString& location, const QDateTime& time) {
        QVariantMap eventData;
        eventData["personnel_id"] = personnelId;
        eventData["location"] = location;
        eventData["time"] = time.toString(Qt::ISODate);
        m_auditLogger->logDeviceOperation("access_granted", QString(), eventData);
    });
    
    connect(m_accessControl, &AccessControl::accessDenied, this, [this](const QString& personnelId, const QString& location, const QString& reason, const QDateTime& time) {
        QVariantMap eventData;
        eventData["personnel_id"] = personnelId;
        eventData["location"] = location;
        eventData["reason"] = reason;
        eventData["time"] = time.toString(Qt::ISODate);
        m_auditLogger->logDeviceOperation("access_denied", QString(), eventData);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setDeviceManager(m_deviceManager);
    m_dbusInterface->setDeviceHealthMonitor(m_healthMonitor);
    m_dbusInterface->setAttendanceTracker(m_attendanceTracker);
    m_dbusInterface->setShiftScheduler(m_shiftScheduler);
    m_dbusInterface->setConflictDetector(m_conflictDetector);
    m_dbusInterface->setLeaveManager(m_leaveManager);
    m_dbusInterface->setShiftSwapManager(m_swapManager);
    m_dbusInterface->setCoverageManager(m_coverageManager);
    m_dbusInterface->setAccessControl(m_accessControl);
    m_dbusInterface->setAccessRestrictionsManager(m_restrictionsManager);
    m_dbusInterface->setRoleManager(m_roleManager);
    m_dbusInterface->setReportGenerator(m_reportGenerator);
    m_dbusInterface->setAnalyticsEngine(m_analyticsEngine);
    m_dbusInterface->setExportManager(m_exportManager);
    m_dbusInterface->setComplianceReporter(m_complianceReporter);
    m_dbusInterface->setPersonnelIntegration(m_personnelIntegration);
    
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

