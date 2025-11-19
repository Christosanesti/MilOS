#ifndef ATTENDANCE_TRACKER_H
#define ATTENDANCE_TRACKER_H

#include "biometric_abstraction.h"
#include "device_manager.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QVariantMap>

/**
 * @brief Attendance Event Type
 */
enum class AttendanceEventType {
    Entry,
    Exit
};

/**
 * @brief Attendance Record
 */
struct AttendanceRecord {
    QString recordId;
    QString personnelId;
    QString deviceId;
    BiometricDeviceType deviceType;
    AttendanceEventType eventType;
    QDateTime timestamp;
    QString location;
    int matchScore;
    QByteArray biometricData;  // Encrypted
    bool validated;
};

/**
 * @brief Attendance Tracker
 * 
 * Tracks real-time attendance via biometric hardware.
 */
class AttendanceTracker : public QObject {
    Q_OBJECT

public:
    explicit AttendanceTracker(DeviceManager* deviceManager, QObject* parent = nullptr);
    ~AttendanceTracker();

    /**
     * @brief Initialize attendance tracker
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start tracking
     */
    void startTracking();

    /**
     * @brief Stop tracking
     */
    void stopTracking();

    /**
     * @brief Record attendance entry
     * @param deviceId Device ID
     * @param personnelId Personnel ID
     * @param location Location/area
     * @return Attendance record ID or empty string if failed
     */
    QString recordEntry(const QString& deviceId, const QString& personnelId, const QString& location);

    /**
     * @brief Record attendance exit
     * @param deviceId Device ID
     * @param personnelId Personnel ID
     * @param location Location/area
     * @return Attendance record ID or empty string if failed
     */
    QString recordExit(const QString& deviceId, const QString& personnelId, const QString& location);

    /**
     * @brief Get attendance records
     * @param personnelId Optional personnel ID filter
     * @param startDate Optional start date filter
     * @param endDate Optional end date filter
     * @return List of attendance records
     */
    QList<AttendanceRecord> getAttendanceRecords(const QString& personnelId = QString(),
                                                 const QDateTime& startDate = QDateTime(),
                                                 const QDateTime& endDate = QDateTime()) const;

    /**
     * @brief Get attendance record by ID
     * @param recordId Record ID
     * @return Attendance record or empty record if not found
     */
    AttendanceRecord getAttendanceRecord(const QString& recordId) const;

    /**
     * @brief Check if personnel is currently present
     * @param personnelId Personnel ID
     * @return true if present, false otherwise
     */
    bool isPersonnelPresent(const QString& personnelId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when attendance is recorded
     */
    void attendanceRecorded(const AttendanceRecord& record);

    /**
     * @brief Emitted when attendance validation fails
     */
    void attendanceValidationFailed(const QString& recordId, const QString& reason);

private slots:
    /**
     * @brief Handle biometric capture
     */
    void onBiometricCaptured(const QByteArray& data);

private:
    DeviceManager* m_deviceManager;
    QList<AttendanceRecord> m_attendanceRecords;
    QMap<QString, QDateTime> m_personnelPresence;  // personnelId -> last entry timestamp
    bool m_tracking;
    QString m_currentLocation;
    
    QString generateRecordId() const;
    bool validateAttendanceEvent(const AttendanceRecord& record) const;
    bool isDuplicate(const AttendanceRecord& record) const;
};

#endif // ATTENDANCE_TRACKER_H

