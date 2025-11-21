#ifndef DRONE_CONTROLLER_H
#define DRONE_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Drone Status
 */
enum class DroneStatus {
    Disconnected,
    Connected,
    Armed,
    Flying,
    Landing,
    Emergency
};

/**
 * @brief Drone Command
 */
enum class DroneCommand {
    Takeoff,
    Land,
    ReturnToHome,
    Hover,
    Move,
    EmergencyStop
};

/**
 * @brief Drone Position
 */
struct DronePosition {
    double latitude;
    double longitude;
    double altitude;
    double heading;
    QDateTime timestamp;
};

/**
 * @brief Drone Controller
 * 
 * Controls drone/UAV operations.
 */
class DroneController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString droneId READ droneId CONSTANT)
    Q_PROPERTY(DroneStatus status READ status NOTIFY statusChanged)

public:
    explicit DroneController(QObject* parent = nullptr);
    ~DroneController();

    /**
     * @brief Initialize drone controller
     * @param droneId Drone ID
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& droneId);

    /**
     * @brief Get drone ID
     * @return Drone ID
     */
    QString droneId() const { return m_droneId; }

    /**
     * @brief Get drone status
     * @return Drone status
     */
    DroneStatus status() const { return m_status; }

    /**
     * @brief Connect to drone
     * @return true if connect successful, false otherwise
     */
    bool connect();

    /**
     * @brief Disconnect from drone
     */
    void disconnect();

    /**
     * @brief Execute command
     * @param command Drone command
     * @param parameters Command parameters
     * @return true if execute successful, false otherwise
     */
    bool executeCommand(DroneCommand command, const QVariantMap& parameters = QVariantMap());

    /**
     * @brief Get current position
     * @return Drone position
     */
    DronePosition getPosition() const { return m_position; }

    /**
     * @brief Get telemetry data
     * @return Telemetry data
     */
    QVariantMap getTelemetry() const;

Q_SIGNALS:
    /**
     * @brief Emitted when status changes
     */
    void statusChanged(DroneStatus status);

    /**
     * @brief Emitted when position updates
     */
    void positionUpdated(const DronePosition& position);

private:
    QString m_droneId;
    DroneStatus m_status;
    DronePosition m_position;
    
    void updateStatus(DroneStatus status);
};

Q_DECLARE_METATYPE(DroneStatus)
Q_DECLARE_METATYPE(DroneCommand)
Q_DECLARE_METATYPE(DronePosition)

#endif // DRONE_CONTROLLER_H

