#ifndef THREAT_INTELLIGENCE_SYSTEM_H
#define THREAT_INTELLIGENCE_SYSTEM_H

#include "threat_feed.h"
#include "threat_correlator.h"
#include "threat_blocker.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Threat Intelligence System
 * 
 * Main system for threat intelligence feed integration.
 */
class ThreatIntelligenceSystem : public QObject {
    Q_OBJECT

public:
    explicit ThreatIntelligenceSystem(QObject* parent = nullptr);
    ~ThreatIntelligenceSystem();

    /**
     * @brief Initialize threat intelligence system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add threat feed
     * @param feed Threat feed
     * @return true if add successful, false otherwise
     */
    bool addFeed(ThreatFeed* feed);

    /**
     * @brief Get threat correlator
     * @return Threat correlator
     */
    ThreatCorrelator* getCorrelator() const { return m_correlator; }

    /**
     * @brief Get threat blocker
     * @return Threat blocker
     */
    ThreatBlocker* getBlocker() const { return m_blocker; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    ThreatIntelligenceDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    QList<ThreatFeed*> m_feeds;
    ThreatCorrelator* m_correlator;
    ThreatBlocker* m_blocker;
    ThreatIntelligenceDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // THREAT_INTELLIGENCE_SYSTEM_H

