#ifndef SATELLITE_COMMUNICATION_SYSTEM_H
#define SATELLITE_COMMUNICATION_SYSTEM_H

#include "satcom_protocol.h"
#include "bandwidth_optimizer.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Satellite Communication System
 * 
 * Main system for satellite communication integration.
 */
class SatelliteCommunicationSystem : public QObject {
    Q_OBJECT

public:
    explicit SatelliteCommunicationSystem(QObject* parent = nullptr);
    ~SatelliteCommunicationSystem();

    /**
     * @brief Initialize satellite communication system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get bandwidth optimizer
     * @return Bandwidth optimizer
     */
    BandwidthOptimizer* getBandwidthOptimizer() const { return m_bandwidthOptimizer; }

    /**
     * @brief Add satellite protocol
     * @param protocol Satellite protocol
     * @return true if add successful, false otherwise
     */
    bool addProtocol(SatComProtocol* protocol);

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    SatelliteCommunicationDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    QList<SatComProtocol*> m_protocols;
    BandwidthOptimizer* m_bandwidthOptimizer;
    SatelliteCommunicationDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // SATELLITE_COMMUNICATION_SYSTEM_H

