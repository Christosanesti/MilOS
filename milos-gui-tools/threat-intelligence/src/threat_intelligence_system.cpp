#include "threat_intelligence_system.h"
#include <QDebug>

ThreatIntelligenceSystem::ThreatIntelligenceSystem(QObject* parent)
    : QObject(parent)
    , m_correlator(new ThreatCorrelator(this))
    , m_blocker(new ThreatBlocker(this))
    , m_dbusInterface(new ThreatIntelligenceDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

ThreatIntelligenceSystem::~ThreatIntelligenceSystem() {
}

bool ThreatIntelligenceSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize correlator
    if (!m_correlator->initialize()) {
        qWarning() << "Failed to initialize threat correlator";
        return false;
    }
    
    // Initialize blocker
    if (!m_blocker->initialize()) {
        qWarning() << "Failed to initialize threat blocker";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_correlator, &ThreatCorrelator::correlationDetected, this, [this](const ThreatCorrelation& correlation) {
        m_auditLogger->logCorrelationDetected(correlation.correlationId);
    });
    
    connect(m_blocker, &ThreatBlocker::indicatorBlocked, this, [this](const ThreatIndicator& indicator) {
        m_auditLogger->logIndicatorBlocked(indicator.id, "Auto-blocked based on threat intelligence");
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

bool ThreatIntelligenceSystem::addFeed(ThreatFeed* feed) {
    if (!feed) {
        return false;
    }
    
    m_feeds.append(feed);
    
    // Connect feed signals
    connect(feed, &ThreatFeed::indicatorsUpdated, this, [this, feed](const QList<ThreatIndicator>& indicators) {
        // Add indicators to correlator
        for (const ThreatIndicator& indicator : indicators) {
            m_correlator->addIndicator(indicator);
            
            // Auto-block if enabled
            if (m_blocker->shouldBlock(indicator)) {
                m_blocker->blockIndicator(indicator);
            }
        }
        
        // Log feed update
        m_auditLogger->logFeedUpdate(feed->feedId(), indicators.size());
    });
    
    return true;
}

