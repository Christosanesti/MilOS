#ifndef PERSONNEL_SCHEDULER_H
#define PERSONNEL_SCHEDULER_H

#include "device_manager.h"
#include "device_health.h"
#include "attendance_tracker.h"
#include "attendance_storage.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include "config_parser.h"
#include <QObject>
#include <QString>

/**
 * @brief Personnel Scheduler Application
 * 
 * Main application class for Personnel Scheduler.
 */
class PersonnelScheduler : public QObject {
    Q_OBJECT

public:
    explicit PersonnelScheduler(QObject* parent = nullptr);
    ~PersonnelScheduler();

    /**
     * @brief Initialize application
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start application
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop application
     */
    void stop();

    /**
     * @brief Get device manager
     */
    DeviceManager* getDeviceManager() const { return m_deviceManager; }

    /**
     * @brief Get device health monitor
     */
    DeviceHealthMonitor* getDeviceHealthMonitor() const { return m_healthMonitor; }

    /**
     * @brief Get D-Bus interface
     */
    PersonnelSchedulerDBusInterface* getDBusInterface() const { return m_dbusInterface; }

    /**
     * @brief Get attendance tracker
     */
    AttendanceTracker* getAttendanceTracker() const { return m_attendanceTracker; }

private:
    DeviceManager* m_deviceManager;
    DeviceHealthMonitor* m_healthMonitor;
    AttendanceTracker* m_attendanceTracker;
    AttendanceStorage* m_attendanceStorage;
    PersonnelSchedulerDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    ConfigParser* m_configParser;
    bool m_initialized;
    bool m_running;
};

#endif // PERSONNEL_SCHEDULER_H

