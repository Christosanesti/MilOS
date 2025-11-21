#include "dbus_interface.h"
#include "drone_uav_system.h"
#include "drone_controller.h"
#include "mission_planner.h"
#include <QDebug>
#include <QDBusConnection>

DroneUAVDBusInterface::DroneUAVDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

DroneUAVDBusInterface::~DroneUAVDBusInterface() {
}

bool DroneUAVDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.DroneUAV")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/DroneUAV", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void DroneUAVDBusInterface::setSystem(DroneUAVSystem* system) {
    m_system = system;
}

bool DroneUAVDBusInterface::ConnectDrone(const QString& droneId) {
    if (!m_system) {
        return false;
    }
    
    DroneController* controller = m_system->getDroneController(droneId);
    if (!controller) {
        return false;
    }
    
    return controller->connect();
}

bool DroneUAVDBusInterface::ExecuteCommand(const QString& droneId, int command, const QVariantMap& parameters) {
    if (!m_system) {
        return false;
    }
    
    DroneController* controller = m_system->getDroneController(droneId);
    if (!controller) {
        return false;
    }
    
    return controller->executeCommand(static_cast<DroneCommand>(command), parameters);
}

QVariantMap DroneUAVDBusInterface::GetPosition(const QString& droneId) {
    QVariantMap result;
    
    if (!m_system) {
        return result;
    }
    
    DroneController* controller = m_system->getDroneController(droneId);
    if (!controller) {
        return result;
    }
    
    DronePosition position = controller->getPosition();
    result["latitude"] = position.latitude;
    result["longitude"] = position.longitude;
    result["altitude"] = position.altitude;
    result["heading"] = position.heading;
    result["timestamp"] = position.timestamp.toString(Qt::ISODate);
    
    return result;
}

QVariantMap DroneUAVDBusInterface::GetTelemetry(const QString& droneId) {
    QVariantMap result;
    
    if (!m_system) {
        return result;
    }
    
    DroneController* controller = m_system->getDroneController(droneId);
    if (!controller) {
        return result;
    }
    
    return controller->getTelemetry();
}

QString DroneUAVDBusInterface::CreateMission(const QString& name, const QString& description) {
    if (!m_system || !m_system->getMissionPlanner()) {
        return QString();
    }
    
    return m_system->getMissionPlanner()->createMission(name, description);
}

bool DroneUAVDBusInterface::AddWaypoint(const QString& missionId, double lat, double lon, double alt, double speed) {
    if (!m_system || !m_system->getMissionPlanner()) {
        return false;
    }
    
    Waypoint waypoint;
    waypoint.latitude = lat;
    waypoint.longitude = lon;
    waypoint.altitude = alt;
    waypoint.speed = speed;
    
    return m_system->getMissionPlanner()->addWaypoint(missionId, waypoint);
}

bool DroneUAVDBusInterface::ExecuteMission(const QString& missionId, const QString& droneId) {
    if (!m_system || !m_system->getMissionPlanner()) {
        return false;
    }
    
    return m_system->getMissionPlanner()->executeMission(missionId, droneId);
}

