#include "attendance_tracker.h"
#include "biometric_capture.h"
#include "attendance_validator.h"
#include "entry_exit_logger.h"
#include <QUuid>
#include <QDebug>

AttendanceTracker::AttendanceTracker(DeviceManager* deviceManager, QObject* parent)
    : QObject(parent)
    , m_deviceManager(deviceManager)
    , m_tracking(false)
{
}

AttendanceTracker::~AttendanceTracker() {
    stopTracking();
}

bool AttendanceTracker::initialize() {
    if (!m_deviceManager) {
        qWarning() << "Device manager not set";
        return false;
    }
    
    return true;
}

void AttendanceTracker::startTracking() {
    if (m_tracking) {
        return;
    }
    
    m_tracking = true;
}

void AttendanceTracker::stopTracking() {
    if (!m_tracking) {
        return;
    }
    
    m_tracking = false;
}

QString AttendanceTracker::recordEntry(const QString& deviceId, const QString& personnelId, const QString& location) {
    if (!m_tracking) {
        qWarning() << "Tracking not started";
        return QString();
    }
    
    if (!m_deviceManager) {
        qWarning() << "Device manager not available";
        return QString();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device || !device->isConnected()) {
        qWarning() << "Device not available:" << deviceId;
        return QString();
    }
    
    // Capture biometric data
    QByteArray biometricData = device->capture(5000);
    if (biometricData.isEmpty()) {
        qWarning() << "Failed to capture biometric data";
        return QString();
    }
    
    // Create attendance record
    AttendanceRecord record;
    record.recordId = generateRecordId();
    record.personnelId = personnelId;
    record.deviceId = deviceId;
    record.deviceType = device->deviceType();
    record.eventType = AttendanceEventType::Entry;
    record.timestamp = QDateTime::currentDateTime();
    record.location = location;
    record.matchScore = 100;  // Will be set during verification
    record.biometricData = biometricData;
    record.validated = false;
    
    // Validate record
    AttendanceValidator validator;
    AttendanceValidator::ValidationResult validation = validator.validate(record);
    if (!validation.valid) {
        emit attendanceValidationFailed(record.recordId, validation.errorMessage);
        return QString();
    }
    
    // Check for duplicates
    if (validator.isDuplicate(record)) {
        emit attendanceValidationFailed(record.recordId, "Duplicate attendance record");
        return QString();
    }
    
    validator.addRecord(record);
    record.validated = true;
    
    // Store record
    m_attendanceRecords.append(record);
    m_personnelPresence[personnelId] = record.timestamp;
    
    emit attendanceRecorded(record);
    
    return record.recordId;
}

QString AttendanceTracker::recordExit(const QString& deviceId, const QString& personnelId, const QString& location) {
    if (!m_tracking) {
        qWarning() << "Tracking not started";
        return QString();
    }
    
    if (!m_deviceManager) {
        qWarning() << "Device manager not available";
        return QString();
    }
    
    // Check if personnel is present
    if (!isPersonnelPresent(personnelId)) {
        qWarning() << "Personnel not present:" << personnelId;
        return QString();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device || !device->isConnected()) {
        qWarning() << "Device not available:" << deviceId;
        return QString();
    }
    
    // Capture biometric data
    QByteArray biometricData = device->capture(5000);
    if (biometricData.isEmpty()) {
        qWarning() << "Failed to capture biometric data";
        return QString();
    }
    
    // Create attendance record
    AttendanceRecord record;
    record.recordId = generateRecordId();
    record.personnelId = personnelId;
    record.deviceId = deviceId;
    record.deviceType = device->deviceType();
    record.eventType = AttendanceEventType::Exit;
    record.timestamp = QDateTime::currentDateTime();
    record.location = location;
    record.matchScore = 100;  // Will be set during verification
    record.biometricData = biometricData;
    record.validated = false;
    
    // Validate record
    AttendanceValidator validator;
    AttendanceValidator::ValidationResult validation = validator.validate(record);
    if (!validation.valid) {
        emit attendanceValidationFailed(record.recordId, validation.errorMessage);
        return QString();
    }
    
    // Check for duplicates
    if (validator.isDuplicate(record)) {
        emit attendanceValidationFailed(record.recordId, "Duplicate attendance record");
        return QString();
    }
    
    validator.addRecord(record);
    record.validated = true;
    
    // Store record
    m_attendanceRecords.append(record);
    m_personnelPresence.remove(personnelId);
    
    emit attendanceRecorded(record);
    
    return record.recordId;
}

QList<AttendanceRecord> AttendanceTracker::getAttendanceRecords(const QString& personnelId,
                                                                 const QDateTime& startDate,
                                                                 const QDateTime& endDate) const {
    QList<AttendanceRecord> filtered;
    
    for (const AttendanceRecord& record : m_attendanceRecords) {
        // Filter by personnel ID
        if (!personnelId.isEmpty() && record.personnelId != personnelId) {
            continue;
        }
        
        // Filter by date range
        if (startDate.isValid() && record.timestamp < startDate) {
            continue;
        }
        if (endDate.isValid() && record.timestamp > endDate) {
            continue;
        }
        
        filtered.append(record);
    }
    
    return filtered;
}

AttendanceRecord AttendanceTracker::getAttendanceRecord(const QString& recordId) const {
    for (const AttendanceRecord& record : m_attendanceRecords) {
        if (record.recordId == recordId) {
            return record;
        }
    }
    
    AttendanceRecord empty;
    return empty;
}

bool AttendanceTracker::isPersonnelPresent(const QString& personnelId) const {
    return m_personnelPresence.contains(personnelId);
}

QString AttendanceTracker::generateRecordId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool AttendanceTracker::validateAttendanceEvent(const AttendanceRecord& record) const {
    AttendanceValidator validator;
    AttendanceValidator::ValidationResult result = validator.validate(record);
    return result.valid;
}

bool AttendanceTracker::isDuplicate(const AttendanceRecord& record) const {
    AttendanceValidator validator;
    return validator.isDuplicate(record);
}

void AttendanceTracker::onBiometricCaptured(const QByteArray& data) {
    // Handle biometric capture event
    // This can be used for real-time processing
}

