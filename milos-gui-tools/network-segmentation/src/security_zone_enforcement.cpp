#include "security_zone_enforcement.h"
#include "segment_config.h"
#include <QUuid>
#include <QProcess>
#include <QDebug>
#include <iostream>

SecurityZoneEnforcement::SecurityZoneEnforcement(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

SecurityZoneEnforcement::~SecurityZoneEnforcement() {
}

bool SecurityZoneEnforcement::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

QString SecurityZoneEnforcement::createZone(const SecurityZonePolicy& policy) {
    if (!m_initialized) {
        return QString();
    }

    SecurityZonePolicy newPolicy = policy;
    if (newPolicy.zoneId.isEmpty()) {
        newPolicy.zoneId = QUuid::createUuid().toString();
    }

    m_zones[newPolicy.zoneId] = newPolicy;

    // Apply zone policy rules
    if (!applyZonePolicyRules(newPolicy)) {
        std::cerr << "Failed to apply zone policy rules" << std::endl;
        m_zones.remove(newPolicy.zoneId);
        return QString();
    }

    emit zoneCreated(newPolicy.zoneId);
    return newPolicy.zoneId;
}

bool SecurityZoneEnforcement::updateZone(const QString& zoneId, const SecurityZonePolicy& policy) {
    if (!m_initialized || !m_zones.contains(zoneId)) {
        return false;
    }

    SecurityZonePolicy updatedPolicy = policy;
    updatedPolicy.zoneId = zoneId;

    m_zones[zoneId] = updatedPolicy;

    // Re-apply zone policy rules
    if (!applyZonePolicyRules(updatedPolicy)) {
        std::cerr << "Failed to apply updated zone policy rules" << std::endl;
        return false;
    }

    emit zoneUpdated(zoneId);
    return true;
}

bool SecurityZoneEnforcement::deleteZone(const QString& zoneId) {
    if (!m_initialized || !m_zones.contains(zoneId)) {
        return false;
    }

    // Remove zone from segment mappings
    for (auto it = m_segmentZones.begin(); it != m_segmentZones.end(); ++it) {
        it.value().removeAll(zoneId);
    }

    m_zones.remove(zoneId);
    emit zoneDeleted(zoneId);
    return true;
}

bool SecurityZoneEnforcement::enforceZonePolicy(const QString& segmentId, const QString& zoneId) {
    if (!m_initialized || !m_zones.contains(zoneId)) {
        return false;
    }

    SecurityZonePolicy policy = m_zones[zoneId];

    // Check if segment is allowed
    if (!isSegmentAllowed(segmentId, zoneId)) {
        std::cerr << "Segment " << segmentId.toStdString() << " is not allowed in zone " << zoneId.toStdString() << std::endl;
        return false;
    }

    // Add segment to zone
    if (!m_segmentZones[segmentId].contains(zoneId)) {
        m_segmentZones[segmentId].append(zoneId);
    }

    // Apply zone policy rules
    if (!applyZonePolicyRules(policy)) {
        std::cerr << "Failed to apply zone policy rules" << std::endl;
        return false;
    }

    emit zonePolicyEnforced(segmentId, zoneId);
    return true;
}

bool SecurityZoneEnforcement::isSegmentAllowed(const QString& segmentId, const QString& zoneId) const {
    if (!m_zones.contains(zoneId)) {
        return false;
    }

    const SecurityZonePolicy& policy = m_zones[zoneId];

    // Check if segment is explicitly blocked
    if (policy.blockedSegments.contains(segmentId)) {
        return false;
    }

    // Check if segment is explicitly allowed
    if (policy.allowedSegments.contains(segmentId)) {
        return true;
    }

    // If enforceIsolation is true, only explicitly allowed segments are allowed
    if (policy.enforceIsolation) {
        return false;
    }

    // Default: allow if not explicitly blocked
    return true;
}

QStringList SecurityZoneEnforcement::getZonesForSegment(const QString& segmentId) const {
    return m_segmentZones.value(segmentId, QStringList());
}

bool SecurityZoneEnforcement::applyZonePolicyRules(const SecurityZonePolicy& policy) {
    // Apply firewall rules based on zone policy
    // This integrates with the firewall rule system
    
    QProcess process;
    
    // Block blocked segments
    for (const QString& segmentId : policy.blockedSegments) {
        // Apply blocking rules via iptables/nftables
        // This would integrate with the firewall manager
        // For now, we'll just log the action
        std::cout << "Blocking segment " << segmentId.toStdString() << " in zone " << policy.zoneId.toStdString() << std::endl;
    }

    // Allow allowed segments
    for (const QString& segmentId : policy.allowedSegments) {
        // Apply allowing rules via iptables/nftables
        std::cout << "Allowing segment " << segmentId.toStdString() << " in zone " << policy.zoneId.toStdString() << std::endl;
    }

    return true;
}

