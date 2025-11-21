#include "threat_detector.h"
#include <QTimer>
#include <QUuid>
#include <QDebug>

ThreatDetector::ThreatDetector(QObject* parent)
    : QObject(parent)
    , m_monitoring(false)
    , m_monitoringTimer(new QTimer(this))
{
    connect(m_monitoringTimer, &QTimer::timeout, this, &ThreatDetector::performMonitoring);
}

ThreatDetector::~ThreatDetector() {
}

bool ThreatDetector::initialize() {
    m_monitoringTimer->setInterval(5000);  // Check every 5 seconds
    return true;
}

bool ThreatDetector::startMonitoring() {
    if (m_monitoring) {
        return true;
    }
    
    m_monitoring = true;
    m_monitoringTimer->start();
    emit monitoringChanged(true);
    
    return true;
}

void ThreatDetector::stopMonitoring() {
    if (!m_monitoring) {
        return;
    }
    
    m_monitoringTimer->stop();
    m_monitoring = false;
    emit monitoringChanged(false);
}

QList<DetectedThreat> ThreatDetector::analyzeData(const QVariantMap& data, const QString& dataType) {
    QList<DetectedThreat> threats;
    
    DetectedThreat threat;
    
    if (dataType == "network") {
        threat = analyzeNetworkData(data);
    } else if (dataType == "process") {
        threat = analyzeProcessData(data);
    } else if (dataType == "file") {
        threat = analyzeFileData(data);
    }
    
    if (!threat.id.isEmpty()) {
        threats.append(threat);
        m_detectedThreats.append(threat);
        
        // Keep only last 1000 threats
        if (m_detectedThreats.size() > 1000) {
            m_detectedThreats.removeFirst();
        }
        
        emit threatDetected(threat);
    }
    
    return threats;
}

QList<DetectedThreat> ThreatDetector::getDetectedThreats() const {
    return m_detectedThreats;
}

QVariantMap ThreatDetector::getStatistics() const {
    QVariantMap stats;
    
    int critical = 0, high = 0, medium = 0, low = 0;
    int intrusion = 0, malware = 0, apt = 0, other = 0;
    
    for (const DetectedThreat& threat : m_detectedThreats) {
        switch (threat.severity) {
            case ThreatSeverity::Critical: critical++; break;
            case ThreatSeverity::High: high++; break;
            case ThreatSeverity::Medium: medium++; break;
            case ThreatSeverity::Low: low++; break;
        }
        
        switch (threat.type) {
            case ThreatType::Intrusion: intrusion++; break;
            case ThreatType::Malware: malware++; break;
            case ThreatType::APT: apt++; break;
            default: other++; break;
        }
    }
    
    stats["total_threats"] = m_detectedThreats.size();
    stats["critical"] = critical;
    stats["high"] = high;
    stats["medium"] = medium;
    stats["low"] = low;
    stats["intrusion"] = intrusion;
    stats["malware"] = malware;
    stats["apt"] = apt;
    stats["other"] = other;
    
    return stats;
}

void ThreatDetector::performMonitoring() {
    // In production, would collect data from system monitor and analyze
    // For now, simulate threat detection
    
    QVariantMap sampleData;
    sampleData["source"] = "192.168.1.100";
    sampleData["destination"] = "10.0.0.1";
    sampleData["port"] = 22;
    
    if (isAnomalous(sampleData, "network")) {
        analyzeData(sampleData, "network");
    }
}

DetectedThreat ThreatDetector::analyzeNetworkData(const QVariantMap& data) const {
    DetectedThreat threat;
    
    // In production, would use ML models to detect anomalies
    // For now, simple rule-based detection
    
    QString source = data["source"].toString();
    int port = data["port"].toInt();
    
    // Detect suspicious port scans
    if (port < 1024 && source.startsWith("192.168")) {
        threat.id = generateThreatId();
        threat.type = ThreatType::Intrusion;
        threat.severity = ThreatSeverity::High;
        threat.description = QString("Suspicious network activity from %1 on port %2").arg(source).arg(port);
        threat.source = source;
        threat.detectedAt = QDateTime::currentDateTime();
        threat.details = data;
        threat.affectedResources = QStringList() << source;
        threat.isContained = false;
    }
    
    return threat;
}

DetectedThreat ThreatDetector::analyzeProcessData(const QVariantMap& data) const {
    DetectedThreat threat;
    
    // In production, would analyze process behavior
    QString processName = data["name"].toString();
    double cpuUsage = data["cpu"].toDouble();
    
    // Detect high CPU usage (potential malware)
    if (cpuUsage > 80.0) {
        threat.id = generateThreatId();
        threat.type = ThreatType::Malware;
        threat.severity = ThreatSeverity::Medium;
        threat.description = QString("Suspicious process: %1 (CPU: %2%)").arg(processName).arg(cpuUsage);
        threat.source = processName;
        threat.detectedAt = QDateTime::currentDateTime();
        threat.details = data;
        threat.isContained = false;
    }
    
    return threat;
}

DetectedThreat ThreatDetector::analyzeFileData(const QVariantMap& data) const {
    DetectedThreat threat;
    
    // In production, would analyze file integrity
    QString filePath = data["path"].toString();
    bool modified = data["modified"].toBool();
    
    // Detect unauthorized file modifications
    if (modified && filePath.contains("/etc/")) {
        threat.id = generateThreatId();
        threat.type = ThreatType::FileIntegrity;
        threat.severity = ThreatSeverity::High;
        threat.description = QString("Unauthorized modification detected: %1").arg(filePath);
        threat.source = filePath;
        threat.detectedAt = QDateTime::currentDateTime();
        threat.details = data;
        threat.affectedResources = QStringList() << filePath;
        threat.isContained = false;
    }
    
    return threat;
}

bool ThreatDetector::isAnomalous(const QVariantMap& data, const QString& dataType) const {
    // In production, would use ML models for anomaly detection
    // For now, simple heuristics
    Q_UNUSED(data);
    Q_UNUSED(dataType);
    
    // Randomly detect some anomalies for demonstration
    return (QRandomGenerator::global()->bounded(100) < 5);  // 5% chance
}

QString ThreatDetector::generateThreatId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

