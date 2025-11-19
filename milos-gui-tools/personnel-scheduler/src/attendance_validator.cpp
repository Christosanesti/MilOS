#include "attendance_validator.h"
#include <QDebug>
#include <QDateTime>

AttendanceValidator::AttendanceValidator(QObject* parent)
    : QObject(parent)
    , m_duplicateWindowSeconds(60)  // 1 minute window for duplicate detection
{
}

AttendanceValidator::~AttendanceValidator() {
}

AttendanceValidator::ValidationResult AttendanceValidator::validate(const AttendanceRecord& record) const {
    ValidationResult result;
    result.valid = true;
    
    // Validate personnel ID
    if (record.personnelId.isEmpty()) {
        result.valid = false;
        result.errorMessage = "Personnel ID is required";
        return result;
    }
    
    // Validate device ID
    if (record.deviceId.isEmpty()) {
        result.valid = false;
        result.errorMessage = "Device ID is required";
        return result;
    }
    
    // Validate timestamp
    if (!record.timestamp.isValid()) {
        result.valid = false;
        result.errorMessage = "Invalid timestamp";
        return result;
    }
    
    // Validate location
    if (record.location.isEmpty()) {
        result.valid = false;
        result.errorMessage = "Location is required";
        return result;
    }
    
    // Validate match score (should be >= 70 for acceptance)
    if (record.matchScore < 70) {
        result.valid = false;
        result.errorMessage = "Biometric match score too low: " + QString::number(record.matchScore);
        return result;
    }
    
    return result;
}

bool AttendanceValidator::isDuplicate(const AttendanceRecord& record) const {
    RecordKey key;
    key.personnelId = record.personnelId;
    key.timestamp = record.timestamp;
    key.eventType = record.eventType;
    
    // Check if record exists in recent records
    if (m_recentRecords.contains(key)) {
        return true;
    }
    
    // Check for records within duplicate window
    QDateTime windowStart = record.timestamp.addSecs(-m_duplicateWindowSeconds);
    QDateTime windowEnd = record.timestamp.addSecs(m_duplicateWindowSeconds);
    
    for (const RecordKey& existingKey : m_recentRecords) {
        if (existingKey.personnelId == record.personnelId &&
            existingKey.eventType == record.eventType &&
            existingKey.timestamp >= windowStart &&
            existingKey.timestamp <= windowEnd) {
            return true;
        }
    }
    
    return false;
}

void AttendanceValidator::addRecord(const AttendanceRecord& record) {
    RecordKey key;
    key.personnelId = record.personnelId;
    key.timestamp = record.timestamp;
    key.eventType = record.eventType;
    
    m_recentRecords.insert(key);
    
    // Clean up old records (older than duplicate window)
    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-m_duplicateWindowSeconds * 2);
    QSet<RecordKey> toRemove;
    for (const RecordKey& existingKey : m_recentRecords) {
        if (existingKey.timestamp < cutoff) {
            toRemove.insert(existingKey);
        }
    }
    for (const RecordKey& key : toRemove) {
        m_recentRecords.remove(key);
    }
}

void AttendanceValidator::clearTracking() {
    m_recentRecords.clear();
}

