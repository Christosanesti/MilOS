#include "ml_threat_detector.h"
#include <QUuid>
#include <QDebug>
#include <QRandomGenerator>

MLThreatDetector::MLThreatDetector(QObject* parent)
    : QObject(parent)
    , m_training(false)
    , m_modelAccuracy(0.0)
{
}

MLThreatDetector::~MLThreatDetector() {
}

bool MLThreatDetector::initialize() {
    // In production, would load pre-trained model
    m_modelAccuracy = 0.85;  // Placeholder accuracy
    return true;
}

bool MLThreatDetector::trainModel(const QList<QVariantMap>& trainingData) {
    if (m_training) {
        return false;  // Already training
    }
    
    m_training = true;
    emit trainingChanged(true);
    
    // In production, would train actual ML model
    // For now, simulate training
    QTimer::singleShot(5000, this, [this]() {
        m_training = false;
        m_modelAccuracy = 0.90;  // Improved accuracy after training
        emit trainingChanged(false);
    });
    
    return true;
}

QList<MLThreatResult> MLThreatDetector::detectThreats(const QVariantMap& networkData) {
    QList<MLThreatResult> threats;
    
    // Extract features from network data
    QVariantMap features = extractFeatures(networkData);
    
    // Analyze with ML model
    MLThreatResult result = analyzeWithModel(features);
    
    if (result.confidence > 0.7) {  // Threshold for threat detection
        threats.append(result);
        emit threatDetected(result);
    }
    
    return threats;
}

QVariantMap MLThreatDetector::extractFeatures(const QVariantMap& networkData) const {
    QVariantMap features;
    
    // Extract features for ML model
    features["packet_size"] = networkData.value("size", 0);
    features["protocol"] = networkData.value("protocol", "unknown");
    features["source_port"] = networkData.value("source_port", 0);
    features["destination_port"] = networkData.value("destination_port", 0);
    features["flags"] = networkData.value("flags", 0);
    
    // Calculate additional features
    int packetSize = features["packet_size"].toInt();
    features["is_large_packet"] = (packetSize > 1500);
    features["is_small_packet"] = (packetSize < 64);
    
    return features;
}

MLThreatResult MLThreatDetector::analyzeWithModel(const QVariantMap& features) const {
    MLThreatResult result;
    result.id = generateThreatId();
    result.detectedAt = QDateTime::currentDateTime();
    result.features = features;
    
    // In production, would use actual ML model (TensorFlow, PyTorch, etc.)
    // For now, rule-based simulation
    
    QString protocol = features["protocol"].toString();
    int destPort = features["destination_port"].toInt();
    
    // Detect port scan
    if (destPort < 1024 && protocol == "TCP") {
        result.type = MLThreatType::PortScan;
        result.description = QString("Port scan detected on port %1").arg(destPort);
        result.confidence = 0.85;
    }
    // Detect DDoS
    else if (features["is_small_packet"].toBool() && features["packet_size"].toInt() < 100) {
        result.type = MLThreatType::DDoS;
        result.description = "Potential DDoS attack detected";
        result.confidence = 0.75;
    }
    // Detect intrusion
    else if (destPort == 22 || destPort == 3389) {
        result.type = MLThreatType::Intrusion;
        result.description = QString("Suspicious connection to port %1").arg(destPort);
        result.confidence = 0.80;
    }
    // Anomaly detection
    else {
        result.type = MLThreatType::Anomaly;
        result.description = "Network anomaly detected";
        result.confidence = 0.70;
    }
    
    return result;
}

QString MLThreatDetector::generateThreatId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

