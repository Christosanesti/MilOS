#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QDBusContext>

class DeviceManager;
class DeviceHealthMonitor;
class AttendanceTracker;
class ShiftScheduler;
class ConflictDetector;
class LeaveManager;
class ShiftSwapManager;
class CoverageManager;

/**
 * @brief D-Bus Interface for Personnel Scheduler
 * 
 * Provides D-Bus interface for device management operations.
 */
class PersonnelSchedulerDBusInterface : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.PersonnelScheduler")

public:
    explicit PersonnelSchedulerDBusInterface(QObject* parent = nullptr);
    ~PersonnelSchedulerDBusInterface();

    /**
     * @brief Set device manager
     */
    void setDeviceManager(DeviceManager* deviceManager);

    /**
     * @brief Set device health monitor
     */
    void setDeviceHealthMonitor(DeviceHealthMonitor* healthMonitor);

    /**
     * @brief Set attendance tracker
     */
    void setAttendanceTracker(AttendanceTracker* attendanceTracker);

    /**
     * @brief Set shift scheduler
     */
    void setShiftScheduler(ShiftScheduler* shiftScheduler);

    /**
     * @brief Set conflict detector
     */
    void setConflictDetector(ConflictDetector* conflictDetector);

    /**
     * @brief Set leave manager
     */
    void setLeaveManager(LeaveManager* leaveManager);

    /**
     * @brief Set shift swap manager
     */
    void setShiftSwapManager(ShiftSwapManager* swapManager);

    /**
     * @brief Set coverage manager
     */
    void setCoverageManager(CoverageManager* coverageManager);

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

public slots:
    /**
     * @brief Detect available devices
     * @param deviceType Device type ("face" or "fingerprint")
     * @return List of device IDs
     */
    QStringList DetectDevices(const QString& deviceType);

    /**
     * @brief Register device
     * @param deviceId Device ID
     * @param deviceType Device type ("face" or "fingerprint")
     * @return true if registration successful, false otherwise
     */
    bool RegisterDevice(const QString& deviceId, const QString& deviceType);

    /**
     * @brief Unregister device
     * @param deviceId Device ID
     * @return true if unregistration successful, false otherwise
     */
    bool UnregisterDevice(const QString& deviceId);

    /**
     * @brief Get registered devices
     * @return List of device IDs
     */
    QStringList GetRegisteredDevices();

    /**
     * @brief Get device information
     * @param deviceId Device ID
     * @return Device information map
     */
    QVariantMap GetDeviceInfo(const QString& deviceId);

    /**
     * @brief Get device health status
     * @param deviceId Device ID
     * @return Device health status map
     */
    QVariantMap GetDeviceHealth(const QString& deviceId);

    /**
     * @brief Capture biometric data
     * @param deviceId Device ID
     * @param timeoutMs Timeout in milliseconds
     * @return Captured biometric data (base64 encoded)
     */
    QString CaptureBiometric(const QString& deviceId, int timeoutMs);

    /**
     * @brief Verify biometric data
     * @param deviceId Device ID
     * @param templateData Template data (base64 encoded)
     * @param capturedData Captured data (base64 encoded)
     * @return Match score (0-100)
     */
    int VerifyBiometric(const QString& deviceId, const QString& templateData, const QString& capturedData);

    /**
     * @brief Enroll biometric template
     * @param deviceId Device ID
     * @param capturedData Captured data (base64 encoded)
     * @return Template data (base64 encoded)
     */
    QString EnrollBiometric(const QString& deviceId, const QString& capturedData);

    /**
     * @brief Get device configuration
     * @param deviceId Device ID
     * @return Device configuration map
     */
    QVariantMap GetDeviceConfiguration(const QString& deviceId);

    /**
     * @brief Set device configuration
     * @param deviceId Device ID
     * @param config Configuration map
     * @return true if configuration successful, false otherwise
     */
    bool SetDeviceConfiguration(const QString& deviceId, const QVariantMap& config);

    /**
     * @brief Record attendance entry
     * @param deviceId Device ID
     * @param personnelId Personnel ID
     * @param location Location/area
     * @return Attendance record ID or empty string if failed
     */
    QString RecordAttendanceEntry(const QString& deviceId, const QString& personnelId, const QString& location);

    /**
     * @brief Record attendance exit
     * @param deviceId Device ID
     * @param personnelId Personnel ID
     * @param location Location/area
     * @return Attendance record ID or empty string if failed
     */
    QString RecordAttendanceExit(const QString& deviceId, const QString& personnelId, const QString& location);

    /**
     * @brief Get attendance records
     * @param personnelId Optional personnel ID filter
     * @param startDate Optional start date (ISO format)
     * @param endDate Optional end date (ISO format)
     * @return JSON string with attendance records
     */
    QString GetAttendanceRecords(const QString& personnelId, const QString& startDate, const QString& endDate);

    /**
     * @brief Get attendance record by ID
     * @param recordId Record ID
     * @return JSON string with attendance record
     */
    QString GetAttendanceRecord(const QString& recordId);

    /**
     * @brief Check if personnel is present
     * @param personnelId Personnel ID
     * @return true if present, false otherwise
     */
    bool IsPersonnelPresent(const QString& personnelId);

    /**
     * @brief Create shift assignment
     * @param personnelId Personnel ID
     * @param startDateTime Start date/time (ISO format)
     * @param endDateTime End date/time (ISO format)
     * @param shiftType Shift type (0=Day, 1=Night, 2=Swing, 3=Custom)
     * @param location Location/area
     * @return Shift ID or empty string if failed
     */
    QString CreateShift(const QString& personnelId, const QString& startDateTime, const QString& endDateTime, int shiftType, const QString& location);

    /**
     * @brief Get shift assignments
     * @param personnelId Optional personnel ID filter
     * @param startDate Optional start date (ISO format)
     * @param endDate Optional end date (ISO format)
     * @return JSON string with shift assignments
     */
    QString GetShifts(const QString& personnelId, const QString& startDate, const QString& endDate);

    /**
     * @brief Detect conflicts in schedule
     * @param startDate Optional start date (ISO format)
     * @param endDate Optional end date (ISO format)
     * @return JSON string with detected conflicts
     */
    QString DetectConflicts(const QString& startDate, const QString& endDate);

    /**
     * @brief Create leave request
     * @param personnelId Personnel ID
     * @param startDate Start date (ISO format)
     * @param endDate End date (ISO format)
     * @param leaveType Leave type (0=Vacation, 1=Sick, 2=Personal, 3=Emergency, 4=Other)
     * @param reason Reason for leave
     * @return Request ID or empty string if failed
     */
    QString CreateLeaveRequest(const QString& personnelId, const QString& startDate, const QString& endDate, int leaveType, const QString& reason);

    /**
     * @brief Approve leave request
     * @param requestId Request ID
     * @param approverId Approver ID
     * @return true if approval successful, false otherwise
     */
    bool ApproveLeaveRequest(const QString& requestId, const QString& approverId);

    /**
     * @brief Create shift swap request
     * @param shiftId Shift ID
     * @param requesterId Requester personnel ID
     * @param targetPersonnelId Target personnel ID
     * @param reason Reason for swap
     * @return Swap ID or empty string if failed
     */
    QString CreateSwapRequest(const QString& shiftId, const QString& requesterId, const QString& targetPersonnelId, const QString& reason);

    /**
     * @brief Approve shift swap request
     * @param swapId Swap ID
     * @param approverId Approver ID
     * @return true if approval successful, false otherwise
     */
    bool ApproveSwapRequest(const QString& swapId, const QString& approverId);

    /**
     * @brief Create coverage request
     * @param shiftId Shift ID
     * @param requesterId Requester personnel ID
     * @param reason Reason for coverage request
     * @return Request ID or empty string if failed
     */
    QString CreateCoverageRequest(const QString& shiftId, const QString& requesterId, const QString& reason);

Q_SIGNALS:
    /**
     * @brief Emitted when device is connected
     */
    void DeviceConnected(const QString& deviceId);

    /**
     * @brief Emitted when device is disconnected
     */
    void DeviceDisconnected(const QString& deviceId);

    /**
     * @brief Emitted when device error occurs
     */
    void DeviceError(const QString& deviceId, const QString& error);

    /**
     * @brief Emitted when device health status changes
     */
    void DeviceHealthChanged(const QString& deviceId, const QVariantMap& health);

private:
    DeviceManager* m_deviceManager;
    DeviceHealthMonitor* m_healthMonitor;
    AttendanceTracker* m_attendanceTracker;
    ShiftScheduler* m_shiftScheduler;
    ConflictDetector* m_conflictDetector;
    LeaveManager* m_leaveManager;
    ShiftSwapManager* m_swapManager;
    CoverageManager* m_coverageManager;
    bool m_initialized;
};

#endif // DBUS_INTERFACE_H


