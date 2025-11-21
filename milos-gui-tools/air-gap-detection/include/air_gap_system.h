#ifndef AIR_GAP_SYSTEM_H
#define AIR_GAP_SYSTEM_H

#include "air_gap_detector.h"
#include "air_gap_enforcement.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>
#include <QString>

/**
 * @brief Air-Gap System
 * 
 * Main system for air-gap detection and enforcement.
 */
class AirGapSystem : public QObject {
    Q_OBJECT

public:
    explicit AirGapSystem(QObject* parent = nullptr);
    ~AirGapSystem();

    /**
     * @brief Initialize air-gap system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start monitoring
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop monitoring
     */
    void stop();

    /**
     * @brief Get air-gap detector
     * @return Air-gap detector
     */
    AirGapDetector* getDetector() const { return m_detector; }

    /**
     * @brief Get air-gap enforcement
     * @return Air-gap enforcement
     */
    AirGapEnforcement* getEnforcement() const { return m_enforcement; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    AirGapDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    AirGapDetector* m_detector;
    AirGapEnforcement* m_enforcement;
    AirGapDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
    bool m_running;
};

#endif // AIR_GAP_SYSTEM_H

