#include "threat_blocker.h"
#include <QDebug>

ThreatBlocker::ThreatBlocker(QObject* parent)
    : QObject(parent)
    , m_autoBlockingEnabled(false)
{
}

ThreatBlocker::~ThreatBlocker() {
}

bool ThreatBlocker::initialize() {
    return true;
}

void ThreatBlocker::setAutoBlockingEnabled(bool enabled) {
    if (m_autoBlockingEnabled != enabled) {
        m_autoBlockingEnabled = enabled;
        emit autoBlockingEnabledChanged(enabled);
    }
}

bool ThreatBlocker::shouldBlock(const ThreatIndicator& indicator) const {
    if (!m_autoBlockingEnabled) {
        return false;
    }
    
    // Auto-block critical and high severity indicators
    return isCriticalSeverity(indicator.severity);
}

bool ThreatBlocker::blockIndicator(const ThreatIndicator& indicator) {
    if (m_blockedIndicators.contains(indicator.id)) {
        return true;  // Already blocked
    }
    
    // In production, would add firewall rules, block IPs, etc.
    m_blockedIndicators.append(indicator.id);
    
    emit indicatorBlocked(indicator);
    
    return true;
}

QStringList ThreatBlocker::getBlockedIndicators() const {
    return m_blockedIndicators;
}

bool ThreatBlocker::unblockIndicator(const QString& indicatorId) {
    if (!m_blockedIndicators.contains(indicatorId)) {
        return false;
    }
    
    m_blockedIndicators.removeAll(indicatorId);
    
    emit indicatorUnblocked(indicatorId);
    
    return true;
}

bool ThreatBlocker::isCriticalSeverity(const QString& severity) const {
    return severity == "critical" || severity == "high";
}

