#include "incident_response.h"
#include <QDebug>

IncidentResponse::IncidentResponse(QObject* parent)
    : QObject(parent)
{
}

IncidentResponse::~IncidentResponse() {
}

bool IncidentResponse::initialize() {
    return true;
}

bool IncidentResponse::respondToThreat(const DetectedThreat& threat) {
    ResponseAction action = determineResponseAction(threat);
    
    switch (action) {
        case ResponseAction::Contain:
            return containThreat(threat.id);
        case ResponseAction::Quarantine:
            if (!threat.affectedResources.isEmpty()) {
                return quarantineResource(threat.affectedResources.first());
            }
            break;
        case ResponseAction::Block:
            if (threat.details.contains("source") && threat.details.contains("destination")) {
                return blockConnection(threat.details["source"].toString(),
                                     threat.details["destination"].toString());
            }
            break;
        default:
            break;
    }
    
    return true;
}

bool IncidentResponse::containThreat(const QString& threatId) {
    if (m_containedThreats.contains(threatId)) {
        return true;  // Already contained
    }
    
    // In production, would implement actual containment
    m_containedThreats.append(threatId);
    
    emit threatContained(threatId);
    
    return true;
}

bool IncidentResponse::quarantineResource(const QString& resourcePath) {
    if (m_quarantinedResources.contains(resourcePath)) {
        return true;  // Already quarantined
    }
    
    // In production, would move file to quarantine directory
    m_quarantinedResources.append(resourcePath);
    
    emit resourceQuarantined(resourcePath);
    
    return true;
}

bool IncidentResponse::blockConnection(const QString& source, const QString& destination) {
    // In production, would add firewall rule
    qDebug() << "Blocking connection from" << source << "to" << destination;
    
    emit connectionBlocked(source, destination);
    
    return true;
}

ResponseAction IncidentResponse::determineResponseAction(const DetectedThreat& threat) const {
    // Determine response based on threat severity and type
    if (threat.severity == ThreatSeverity::Critical) {
        return ResponseAction::Contain;
    } else if (threat.severity == ThreatSeverity::High) {
        if (threat.type == ThreatType::Malware) {
            return ResponseAction::Quarantine;
        }
        return ResponseAction::Block;
    } else if (threat.severity == ThreatSeverity::Medium) {
        return ResponseAction::Notify;
    }
    
    return ResponseAction::Log;
}

