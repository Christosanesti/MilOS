#include "threat_correlator.h"
#include <QUuid>
#include <QDebug>

ThreatCorrelator::ThreatCorrelator(QObject* parent)
    : QObject(parent)
{
}

ThreatCorrelator::~ThreatCorrelator() {
}

bool ThreatCorrelator::initialize() {
    return true;
}

bool ThreatCorrelator::addIndicator(const ThreatIndicator& indicator) {
    m_indicators.append(indicator);
    return true;
}

QList<ThreatCorrelation> ThreatCorrelator::correlateIndicators() {
    QList<ThreatCorrelation> correlations;
    
    // Find related indicators
    for (int i = 0; i < m_indicators.size(); ++i) {
        for (int j = i + 1; j < m_indicators.size(); ++j) {
            if (areIndicatorsRelated(m_indicators[i], m_indicators[j])) {
                ThreatCorrelation correlation;
                correlation.correlationId = generateCorrelationId();
                correlation.relatedIndicators = QStringList() << m_indicators[i].id << m_indicators[j].id;
                
                // Determine severity based on indicators
                if (m_indicators[i].severity == "critical" || m_indicators[j].severity == "critical") {
                    correlation.severity = "critical";
                } else if (m_indicators[i].severity == "high" || m_indicators[j].severity == "high") {
                    correlation.severity = "high";
                } else {
                    correlation.severity = "medium";
                }
                
                correlation.description = QString("Correlated threat: %1 and %2")
                                         .arg(m_indicators[i].value, m_indicators[j].value);
                correlation.detectedAt = QDateTime::currentDateTime();
                
                correlations.append(correlation);
                m_correlations.append(correlation);
                
                emit correlationDetected(correlation);
            }
        }
    }
    
    return correlations;
}

QList<ThreatCorrelation> ThreatCorrelator::getCorrelations(const QString& indicatorId) const {
    QList<ThreatCorrelation> result;
    
    for (const ThreatCorrelation& correlation : m_correlations) {
        if (correlation.relatedIndicators.contains(indicatorId)) {
            result.append(correlation);
        }
    }
    
    return result;
}

QVariantMap ThreatCorrelator::analyzeThreat(const QString& indicatorId) const {
    QVariantMap analysis;
    
    // Find indicator
    ThreatIndicator indicator;
    for (const ThreatIndicator& ind : m_indicators) {
        if (ind.id == indicatorId) {
            indicator = ind;
            break;
        }
    }
    
    if (indicator.id.isEmpty()) {
        return analysis;
    }
    
    analysis["indicator_id"] = indicator.id;
    analysis["type"] = indicator.type;
    analysis["value"] = indicator.value;
    analysis["severity"] = indicator.severity;
    analysis["description"] = indicator.description;
    analysis["first_seen"] = indicator.firstSeen.toString(Qt::ISODate);
    analysis["last_seen"] = indicator.lastSeen.toString(Qt::ISODate);
    analysis["tags"] = indicator.tags;
    
    // Get correlations
    QList<ThreatCorrelation> correlations = getCorrelations(indicatorId);
    QVariantList correlationList;
    for (const ThreatCorrelation& corr : correlations) {
        QVariantMap corrMap;
        corrMap["correlation_id"] = corr.correlationId;
        corrMap["related_indicators"] = corr.relatedIndicators;
        corrMap["severity"] = corr.severity;
        corrMap["description"] = corr.description;
        correlationList.append(corrMap);
    }
    analysis["correlations"] = correlationList;
    
    return analysis;
}

bool ThreatCorrelator::areIndicatorsRelated(const ThreatIndicator& a, const ThreatIndicator& b) const {
    // Check if indicators share tags
    for (const QString& tag : a.tags) {
        if (b.tags.contains(tag)) {
            return true;
        }
    }
    
    // Check if same type and similar value (for IPs, check same subnet)
    if (a.type == b.type && a.type == "ip") {
        // Simple check: same first three octets
        QStringList aParts = a.value.split(".");
        QStringList bParts = b.value.split(".");
        if (aParts.size() >= 3 && bParts.size() >= 3) {
            if (aParts[0] == bParts[0] && aParts[1] == bParts[1] && aParts[2] == bParts[2]) {
                return true;
            }
        }
    }
    
    return false;
}

QString ThreatCorrelator::generateCorrelationId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

