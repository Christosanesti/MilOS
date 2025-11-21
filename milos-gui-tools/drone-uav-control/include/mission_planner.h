#ifndef MISSION_PLANNER_H
#define MISSION_PLANNER_H

#include "drone_controller.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

/**
 * @brief Waypoint
 */
struct Waypoint {
    QString id;
    double latitude;
    double longitude;
    double altitude;
    double speed;
    QString action;  // "survey", "patrol", "land", etc.
    QVariantMap parameters;
};

/**
 * @brief Mission
 */
struct Mission {
    QString id;
    QString name;
    QString description;
    QList<Waypoint> waypoints;
    QDateTime created;
    QDateTime scheduled;
    QString status;  // "draft", "scheduled", "active", "completed"
};

/**
 * @brief Mission Planner
 * 
 * Plans and manages drone missions.
 */
class MissionPlanner : public QObject {
    Q_OBJECT

public:
    explicit MissionPlanner(QObject* parent = nullptr);
    ~MissionPlanner();

    /**
     * @brief Initialize mission planner
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create mission
     * @param name Mission name
     * @param description Mission description
     * @return Mission ID or empty if failed
     */
    QString createMission(const QString& name, const QString& description);

    /**
     * @brief Add waypoint to mission
     * @param missionId Mission ID
     * @param waypoint Waypoint
     * @return true if add successful, false otherwise
     */
    bool addWaypoint(const QString& missionId, const Waypoint& waypoint);

    /**
     * @brief Get mission
     * @param missionId Mission ID
     * @return Mission or empty if not found
     */
    Mission getMission(const QString& missionId) const;

    /**
     * @brief Execute mission
     * @param missionId Mission ID
     * @param droneId Drone ID
     * @return true if execute successful, false otherwise
     */
    bool executeMission(const QString& missionId, const QString& droneId);

Q_SIGNALS:
    /**
     * @brief Emitted when mission completes
     */
    void missionCompleted(const QString& missionId, bool success);

private:
    QMap<QString, Mission> m_missions;
    
    QString generateMissionId() const;
    QString generateWaypointId() const;
};

Q_DECLARE_METATYPE(Waypoint)
Q_DECLARE_METATYPE(Mission)

#endif // MISSION_PLANNER_H

