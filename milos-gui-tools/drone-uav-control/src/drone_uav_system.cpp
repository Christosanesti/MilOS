#include "drone_uav_system.h"
#include "drone_controller.h"
#include <QDebug>

DroneUAVSystem::DroneUAVSystem(QObject* parent)
    : QObject(parent)
    , m_missionPlanner(new MissionPlanner(this))
    , m_dbusInterface(new DroneUAVDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

DroneUAVSystem::~DroneUAVSystem() {
}

bool DroneUAVSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize mission planner
    if (!m_missionPlanner->initialize()) {
        qWarning() << "Failed to initialize mission planner";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_missionPlanner, &MissionPlanner::missionCompleted, this, [this](const QString& missionId, bool success) {
        QString droneId = "unknown";  // Would get from mission context
        m_auditLogger->logMissionExecuted(missionId, droneId);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setSystem(this);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

DroneController* DroneUAVSystem::getDroneController(const QString& droneId) const {
    for (DroneController* controller : m_droneControllers) {
        if (controller->droneId() == droneId) {
            return controller;
        }
    }
    return nullptr;
}

bool DroneUAVSystem::addDroneController(DroneController* controller) {
    if (!controller) {
        return false;
    }
    
    m_droneControllers.append(controller);
    
    // Connect signals for audit logging
    connect(controller, &DroneController::statusChanged, this, [this, controller](DroneStatus status) {
        QString command = QString("Status changed to %1").arg(static_cast<int>(status));
        m_auditLogger->logDroneCommand(controller->droneId(), command);
    });
    
    return true;
}

