#include "threat_analyzer.h"
#include <QUuid>
#include <QDebug>

ThreatAnalyzer::ThreatAnalyzer(QObject* parent)
    : QObject(parent)
{
}

ThreatAnalyzer::~ThreatAnalyzer() {
}

bool ThreatAnalyzer::initialize() {
    return true;
}

ThreatAnalysisResult ThreatAnalyzer::analyzeInteraction(const QVariantMap& interactionData) {
    ThreatAnalysisResult result;
    result.id = generateThreatId();
    result.detectedAt = QDateTime::currentDateTime();
    result.indicators = interactionData;
    
    // Determine threat type
    result.threatType = determineThreatType(interactionData);
    
    // Determine severity
    result.severity = determineSeverity(interactionData);
    
    // Generate description
    result.description = QString("Threat detected: %1 from %2")
                        .arg(result.threatType)
                        .arg(interactionData.value("source_ip").toString());
    
    m_detectedThreats.append(result);
    
    // Keep only last 1000 threats
    if (m_detectedThreats.size() > 1000) {
        m_detectedThreats.removeFirst();
    }
    
    emit threatDetected(result);
    
    return result;
}

QVariantMap ThreatAnalyzer::getThreatStatistics() const {
    QVariantMap stats;
    
    int lowCount = 0, mediumCount = 0, highCount = 0, criticalCount = 0;
    
    for (const ThreatAnalysisResult& threat : m_detectedThreats) {
        if (threat.severity == "low") lowCount++;
        else if (threat.severity == "medium") mediumCount++;
        else if (threat.severity == "high") highCount++;
        else if (threat.severity == "critical") criticalCount++;
    }
    
    stats["total_threats"] = m_detectedThreats.size();
    stats["low"] = lowCount;
    stats["medium"] = mediumCount;
    stats["high"] = highCount;
    stats["critical"] = criticalCount;
    
    return stats;
}

QString ThreatAnalyzer::determineThreatType(const QVariantMap& interactionData) const {
    QString action = interactionData.value("action").toString();
    
    if (action.contains("scan")) {
        return "Port Scan";
    } else if (action.contains("bruteforce")) {
        return "Brute Force Attack";
    } else if (action.contains("exploit")) {
        return "Exploit Attempt";
    } else if (action.contains("malware")) {
        return "Malware";
    }
    
    return "Unknown Threat";
}

QString ThreatAnalyzer::determineSeverity(const QVariantMap& interactionData) const {
    QString action = interactionData.value("action").toString();
    
    if (action.contains("exploit") || action.contains("malware")) {
        return "critical";
    } else if (action.contains("bruteforce")) {
        return "high";
    } else if (action.contains("scan")) {
        return "medium";
    }
    
    return "low";
}

QString ThreatAnalyzer::generateThreatId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

