#ifndef ADVANCED_NETWORK_DEFENSE_SYSTEM_H
#define ADVANCED_NETWORK_DEFENSE_SYSTEM_H

#include "ml_threat_detector.h"
#include "network_forensics.h"
#include "packet_analyzer.h"
#include "ids_rules_engine.h"
#include "incident_response_workflow.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Advanced Network Defense System
 * 
 * Main system for advanced network defense capabilities.
 */
class AdvancedNetworkDefenseSystem : public QObject {
    Q_OBJECT

public:
    explicit AdvancedNetworkDefenseSystem(QObject* parent = nullptr);
    ~AdvancedNetworkDefenseSystem();

    /**
     * @brief Initialize advanced network defense system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get ML threat detector
     * @return ML threat detector
     */
    MLThreatDetector* getMLThreatDetector() const { return m_mlDetector; }

    /**
     * @brief Get network forensics
     * @return Network forensics
     */
    NetworkForensics* getNetworkForensics() const { return m_forensics; }

    /**
     * @brief Get packet analyzer
     * @return Packet analyzer
     */
    PacketAnalyzer* getPacketAnalyzer() const { return m_packetAnalyzer; }

    /**
     * @brief Get IDS rules engine
     * @return IDS rules engine
     */
    IDSRulesEngine* getIDSRulesEngine() const { return m_idsEngine; }

    /**
     * @brief Get incident response workflow
     * @return Incident response workflow
     */
    IncidentResponseWorkflow* getIncidentResponseWorkflow() const { return m_incidentWorkflow; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    AdvancedNetworkDefenseDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    MLThreatDetector* m_mlDetector;
    NetworkForensics* m_forensics;
    PacketAnalyzer* m_packetAnalyzer;
    IDSRulesEngine* m_idsEngine;
    IncidentResponseWorkflow* m_incidentWorkflow;
    AdvancedNetworkDefenseDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // ADVANCED_NETWORK_DEFENSE_SYSTEM_H

