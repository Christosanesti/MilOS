#include "mission_planner.h"
#include <QUuid>
#include <QDebug>
#include <QTimer>
#include <QMap>

MissionPlanner::MissionPlanner(QObject* parent)
    : QObject(parent)
{
}

MissionPlanner::~MissionPlanner() {
}

bool MissionPlanner::initialize() {
    return true;
}

QString MissionPlanner::createMission(const QString& name, const QString& description) {
    Mission mission;
    mission.id = generateMissionId();
    mission.name = name;
    mission.description = description;
    mission.created = QDateTime::currentDateTime();
    mission.status = "draft";
    
    m_missions[mission.id] = mission;
    
    return mission.id;
}

bool MissionPlanner::addWaypoint(const QString& missionId, const Waypoint& waypoint) {
    if (!m_missions.contains(missionId)) {
        return false;
    }
    
    Mission& mission = m_missions[missionId];
    Waypoint wp = waypoint;
    if (wp.id.isEmpty()) {
        wp.id = generateWaypointId();
    }
    mission.waypoints.append(wp);
    
    return true;
}

Mission MissionPlanner::getMission(const QString& missionId) const {
    return m_missions.value(missionId, Mission());
}

bool MissionPlanner::executeMission(const QString& missionId, const QString& droneId) {
    if (!m_missions.contains(missionId)) {
        return false;
    }
    
    Mission& mission = m_missions[missionId];
    mission.status = "active";
    
    // In production, would send waypoints to drone controller
    qDebug() << "Executing mission" << missionId << "with drone" << droneId;
    
    // Simulate mission completion
    QTimer::singleShot(10000, this, [this, missionId]() {
        if (m_missions.contains(missionId)) {
            m_missions[missionId].status = "completed";
            emit missionCompleted(missionId, true);
        }
    });
    
    return true;
}

QString MissionPlanner::generateMissionId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString MissionPlanner::generateWaypointId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

