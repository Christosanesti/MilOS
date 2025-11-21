#ifndef PERSONNEL_LOCATION_SYSTEM_H
#define PERSONNEL_LOCATION_SYSTEM_H

#include "location_tracker.h"
#include "location_storage.h"
#include "location_encryption.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Personnel Location System
 * 
 * Main system for personnel location tracking.
 */
class PersonnelLocationSystem : public QObject {
    Q_OBJECT

public:
    explicit PersonnelLocationSystem(QObject* parent = nullptr);
    ~PersonnelLocationSystem();

    /**
     * @brief Initialize personnel location system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get location tracker
     * @return Location tracker
     */
    LocationTracker* getTracker() const { return m_tracker; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    PersonnelLocationDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    LocationTracker* m_tracker;
    LocationStorage* m_storage;
    LocationEncryption* m_encryption;
    PersonnelLocationDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // PERSONNEL_LOCATION_SYSTEM_H

