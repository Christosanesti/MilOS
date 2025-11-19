#ifndef ATTENDANCE_VALIDATOR_H
#define ATTENDANCE_VALIDATOR_H

#include "attendance_tracker.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSet>

/**
 * @brief Attendance Validator
 * 
 * Validates attendance events and detects duplicates.
 */
class AttendanceValidator : public QObject {
    Q_OBJECT

public:
    explicit AttendanceValidator(QObject* parent = nullptr);
    ~AttendanceValidator();

    /**
     * @brief Validate attendance event
     * @param record Attendance record
     * @return Validation result with error message if invalid
     */
    struct ValidationResult {
        bool valid;
        QString errorMessage;
    };
    
    ValidationResult validate(const AttendanceRecord& record) const;

    /**
     * @brief Check if record is duplicate
     * @param record Attendance record
     * @return true if duplicate, false otherwise
     */
    bool isDuplicate(const AttendanceRecord& record) const;

    /**
     * @brief Add record to duplicate tracking
     * @param record Attendance record
     */
    void addRecord(const AttendanceRecord& record);

    /**
     * @brief Clear duplicate tracking
     */
    void clearTracking();

private:
    struct RecordKey {
        QString personnelId;
        QDateTime timestamp;
        AttendanceEventType eventType;
        
        bool operator<(const RecordKey& other) const {
            if (personnelId != other.personnelId) {
                return personnelId < other.personnelId;
            }
            if (timestamp != other.timestamp) {
                return timestamp < other.timestamp;
            }
            return eventType < other.eventType;
        }
    };
    
    QSet<RecordKey> m_recentRecords;
    int m_duplicateWindowSeconds;  // Time window for duplicate detection
};

#endif // ATTENDANCE_VALIDATOR_H

