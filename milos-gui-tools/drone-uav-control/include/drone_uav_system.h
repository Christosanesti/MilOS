#ifndef DRONE_UAV_SYSTEM_H
#define DRONE_UAV_SYSTEM_H

#include "drone_controller.h"
#include "mission_planner.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Drone/UAV System
 * 
 * Main system for drone/UAV control.
 */
class DroneUAVSystem : public QObject {
    Q_OBJECT

public:
    explicit DroneUAVSystem(QObject* parent = nullptr);
    ~DroneUAVSystem();

    /**
     * @brief Initialize drone/UAV system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get mission planner
     * @return Mission planner
     */
    MissionPlanner* getMissionPlanner() const { return m_missionPlanner; }

    /**
     * @brief Get drone controller
     * @param droneId Drone ID
     * @return Drone controller or nullptr if not found
     */
    DroneController* getDroneController(const QString& droneId) const;

    /**
     * @brief Add drone controller
     * @param controller Drone controller
     * @return true if add successful, false otherwise
     */
    bool addDroneController(DroneController* controller);

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    DroneUAVDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    QList<DroneController*> m_droneControllers;
    MissionPlanner* m_missionPlanner;
    DroneUAVDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // DRONE_UAV_SYSTEM_H

