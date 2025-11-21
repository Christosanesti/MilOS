#include "drone_controller.h"
#include <QUuid>
#include <QDebug>
#include <QTimer>
#include <QMap>

DroneController::DroneController(QObject* parent)
    : QObject(parent)
    , m_status(DroneStatus::Disconnected)
{
    m_position.latitude = 0.0;
    m_position.longitude = 0.0;
    m_position.altitude = 0.0;
    m_position.heading = 0.0;
}

DroneController::~DroneController() {
}

bool DroneController::initialize(const QString& droneId) {
    m_droneId = droneId;
    return true;
}

bool DroneController::connect() {
    if (m_status == DroneStatus::Connected || m_status == DroneStatus::Flying) {
        return true;
    }
    
    // In production, would connect to actual drone hardware
    updateStatus(DroneStatus::Connected);
    
    return true;
}

void DroneController::disconnect() {
    updateStatus(DroneStatus::Disconnected);
}

bool DroneController::executeCommand(DroneCommand command, const QVariantMap& parameters) {
    if (m_status == DroneStatus::Disconnected) {
        return false;
    }
    
    // In production, would send command to actual drone
    switch (command) {
        case DroneCommand::Takeoff:
            updateStatus(DroneStatus::Flying);
            break;
        case DroneCommand::Land:
            updateStatus(DroneStatus::Landing);
            QTimer::singleShot(5000, this, [this]() {
                updateStatus(DroneStatus::Connected);
            });
            break;
        case DroneCommand::EmergencyStop:
            updateStatus(DroneStatus::Emergency);
            break;
        default:
            break;
    }
    
    return true;
}

QVariantMap DroneController::getTelemetry() const {
    QVariantMap telemetry;
    
    telemetry["drone_id"] = m_droneId;
    telemetry["status"] = static_cast<int>(m_status);
    telemetry["latitude"] = m_position.latitude;
    telemetry["longitude"] = m_position.longitude;
    telemetry["altitude"] = m_position.altitude;
    telemetry["heading"] = m_position.heading;
    telemetry["battery"] = 85;  // Placeholder
    telemetry["signal_strength"] = 95;  // Placeholder
    
    return telemetry;
}

void DroneController::updateStatus(DroneStatus status) {
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

