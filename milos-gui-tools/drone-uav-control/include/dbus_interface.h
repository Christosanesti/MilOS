#ifndef DRONE_UAV_DBUS_INTERFACE_H
#define DRONE_UAV_DBUS_INTERFACE_H

#include "drone_uav_system.h"
#include <QObject>
#include <QString>
#include <QVariantMap>

class DroneUAVSystem;

/**
 * @brief Drone/UAV D-Bus Interface
 */
class DroneUAVDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.DroneUAV")

public:
    explicit DroneUAVDBusInterface(QObject* parent = nullptr);
    ~DroneUAVDBusInterface();

    bool initialize();
    void setSystem(DroneUAVSystem* system);

public slots:
    bool ConnectDrone(const QString& droneId);
    bool ExecuteCommand(const QString& droneId, int command, const QVariantMap& parameters);
    QVariantMap GetPosition(const QString& droneId);
    QVariantMap GetTelemetry(const QString& droneId);
    QString CreateMission(const QString& name, const QString& description);
    bool AddWaypoint(const QString& missionId, double lat, double lon, double alt, double speed);
    bool ExecuteMission(const QString& missionId, const QString& droneId);

private:
    DroneUAVSystem* m_system;
};

#endif // DRONE_UAV_DBUS_INTERFACE_H

