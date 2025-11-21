#include "dbus_interface.h"
#include "threat_intelligence_system.h"
#include "threat_feed.h"
#include "threat_correlator.h"
#include "threat_blocker.h"
#include <QDebug>
#include <QDBusConnection>

ThreatIntelligenceDBusInterface::ThreatIntelligenceDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

ThreatIntelligenceDBusInterface::~ThreatIntelligenceDBusInterface() {
}

bool ThreatIntelligenceDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.ThreatIntelligence")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/ThreatIntelligence", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void ThreatIntelligenceDBusInterface::setSystem(ThreatIntelligenceSystem* system) {
    m_system = system;
}

QStringList ThreatIntelligenceDBusInterface::GetFeeds() {
    if (!m_system) {
        return QStringList();
    }
    
    // In production, would get actual feed list
    return QStringList() << "feed-001" << "feed-002";
}

QVariantMap ThreatIntelligenceDBusInterface::GetFeedStatus(const QString& feedId) {
    if (!m_system) {
        return QVariantMap();
    }
    
    // In production, would get actual feed status
    QVariantMap status;
    status["feed_id"] = feedId;
    status["enabled"] = true;
    return status;
}

bool ThreatIntelligenceDBusInterface::EnableFeed(const QString& feedId, bool enabled) {
    if (!m_system) {
        return false;
    }
    
    // In production, would enable/disable feed
    Q_UNUSED(feedId);
    Q_UNUSED(enabled);
    return true;
}

QVariantMap ThreatIntelligenceDBusInterface::GetThreatIndicators(const QString& feedId) {
    QVariantMap result;
    
    if (!m_system) {
        return result;
    }
    
    // In production, would get actual indicators
    Q_UNUSED(feedId);
    return result;
}

QVariantMap ThreatIntelligenceDBusInterface::CorrelateThreats() {
    QVariantMap result;
    
    if (!m_system || !m_system->getCorrelator()) {
        return result;
    }
    
    QList<ThreatCorrelation> correlations = m_system->getCorrelator()->correlateIndicators();
    
    QVariantList correlationList;
    for (const ThreatCorrelation& corr : correlations) {
        QVariantMap corrMap;
        corrMap["correlation_id"] = corr.correlationId;
        corrMap["related_indicators"] = corr.relatedIndicators;
        corrMap["severity"] = corr.severity;
        corrMap["description"] = corr.description;
        correlationList.append(corrMap);
    }
    
    result["correlations"] = correlationList;
    result["count"] = correlations.size();
    
    return result;
}

bool ThreatIntelligenceDBusInterface::BlockIndicator(const QString& indicatorId) {
    if (!m_system || !m_system->getBlocker()) {
        return false;
    }
    
    // In production, would get actual indicator and block it
    Q_UNUSED(indicatorId);
    return true;
}

bool ThreatIntelligenceDBusInterface::UnblockIndicator(const QString& indicatorId) {
    if (!m_system || !m_system->getBlocker()) {
        return false;
    }
    
    return m_system->getBlocker()->unblockIndicator(indicatorId);
}

QStringList ThreatIntelligenceDBusInterface::GetBlockedIndicators() {
    if (!m_system || !m_system->getBlocker()) {
        return QStringList();
    }
    
    return m_system->getBlocker()->getBlockedIndicators();
}

bool ThreatIntelligenceDBusInterface::SetAutoBlocking(bool enabled) {
    if (!m_system || !m_system->getBlocker()) {
        return false;
    }
    
    m_system->getBlocker()->setAutoBlockingEnabled(enabled);
    return true;
}

