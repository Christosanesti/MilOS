#ifndef AIR_GAP_DBUS_INTERFACE_H
#define AIR_GAP_DBUS_INTERFACE_H

#include "air_gap_detector.h"
#include "air_gap_enforcement.h"
#include <QObject>
#include <QString>
#include <QVariantMap>

class AirGapDetector;
class AirGapEnforcement;

/**
 * @brief Air-Gap D-Bus Interface
 * 
 * D-Bus interface for air-gap detection and enforcement.
 */
class AirGapDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.AirGap")

public:
    explicit AirGapDBusInterface(QObject* parent = nullptr);
    ~AirGapDBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Set air-gap detector
     */
    void setDetector(AirGapDetector* detector);

    /**
     * @brief Set air-gap enforcement
     */
    void setEnforcement(AirGapEnforcement* enforcement);

public slots:
    /**
     * @brief Get air-gap status
     * @return Status string ("Connected", "AirGapped", "Unknown")
     */
    QString GetStatus();

    /**
     * @brief Check if system is air-gapped
     * @return true if air-gapped, false otherwise
     */
    bool IsAirGapped();

    /**
     * @brief Force immediate check
     */
    void CheckNow();

    /**
     * @brief Get network interfaces status
     * @return Map of interface names to status
     */
    QVariantMap GetNetworkInterfacesStatus();

    /**
     * @brief Get current policy level
     * @return Policy level string ("Normal", "Enhanced", "Maximum")
     */
    QString GetPolicyLevel();

    /**
     * @brief Check if operation is allowed
     * @param operation Operation name
     * @return true if allowed, false otherwise
     */
    bool IsOperationAllowed(const QString& operation);

    /**
     * @brief Get blocked operations
     * @return List of blocked operations
     */
    QStringList GetBlockedOperations();

private:
    AirGapDetector* m_detector;
    AirGapEnforcement* m_enforcement;
};

#endif // AIR_GAP_DBUS_INTERFACE_H

