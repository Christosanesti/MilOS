#include "dbus_interface.h"
#include "advanced_network_defense_system.h"
#include "ml_threat_detector.h"
#include "network_forensics.h"
#include "packet_analyzer.h"
#include "ids_rules_engine.h"
#include "incident_response_workflow.h"
#include <QDebug>
#include <QDBusConnection>
#include <QJsonDocument>

AdvancedNetworkDefenseDBusInterface::AdvancedNetworkDefenseDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

AdvancedNetworkDefenseDBusInterface::~AdvancedNetworkDefenseDBusInterface() {
}

bool AdvancedNetworkDefenseDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.AdvancedNetworkDefense")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/AdvancedNetworkDefense", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void AdvancedNetworkDefenseDBusInterface::setSystem(AdvancedNetworkDefenseSystem* system) {
    m_system = system;
}

QVariantMap AdvancedNetworkDefenseDBusInterface::DetectThreats(const QVariantMap& networkData) {
    QVariantMap result;
    
    if (!m_system || !m_system->getMLThreatDetector()) {
        return result;
    }
    
    QList<MLThreatResult> threats = m_system->getMLThreatDetector()->detectThreats(networkData);
    
    QVariantList threatList;
    for (const MLThreatResult& threat : threats) {
        QVariantMap threatMap;
        threatMap["id"] = threat.id;
        threatMap["type"] = static_cast<int>(threat.type);
        threatMap["description"] = threat.description;
        threatMap["confidence"] = threat.confidence;
        threatMap["detected_at"] = threat.detectedAt.toString(Qt::ISODate);
        threatList.append(threatMap);
    }
    
    result["threats"] = threatList;
    result["count"] = threats.size();
    
    return result;
}

bool AdvancedNetworkDefenseDBusInterface::TrainModel(const QStringList& trainingData) {
    if (!m_system || !m_system->getMLThreatDetector()) {
        return false;
    }
    
    QList<QVariantMap> dataList;
    for (const QString& jsonStr : trainingData) {
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (!doc.isNull()) {
            dataList.append(doc.object().toVariantMap());
        }
    }
    
    return m_system->getMLThreatDetector()->trainModel(dataList);
}

double AdvancedNetworkDefenseDBusInterface::GetModelAccuracy() {
    if (!m_system || !m_system->getMLThreatDetector()) {
        return 0.0;
    }
    
    return m_system->getMLThreatDetector()->getModelAccuracy();
}

bool AdvancedNetworkDefenseDBusInterface::CaptureEvidence(const QString& type, const QString& source, const QByteArray& data) {
    if (!m_system || !m_system->getNetworkForensics()) {
        return false;
    }
    
    ForensicEvidence evidence;
    evidence.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    evidence.type = type;
    evidence.source = source;
    evidence.timestamp = QDateTime::currentDateTime();
    evidence.data = data;
    
    return m_system->getNetworkForensics()->captureEvidence(evidence);
}

QVariantMap AdvancedNetworkDefenseDBusInterface::GetEvidence(const QString& evidenceId) {
    QVariantMap result;
    
    if (!m_system || !m_system->getNetworkForensics()) {
        return result;
    }
    
    ForensicEvidence evidence = m_system->getNetworkForensics()->getEvidence(evidenceId);
    
    result["id"] = evidence.id;
    result["type"] = evidence.type;
    result["source"] = evidence.source;
    result["timestamp"] = evidence.timestamp.toString(Qt::ISODate);
    result["data_size"] = evidence.data.size();
    result["metadata"] = evidence.metadata;
    
    return result;
}

QVariantMap AdvancedNetworkDefenseDBusInterface::AnalyzePacket(const QByteArray& packetData) {
    QVariantMap result;
    
    if (!m_system || !m_system->getPacketAnalyzer()) {
        return result;
    }
    
    PacketAnalysisResult analysis = m_system->getPacketAnalyzer()->analyzePacket(packetData);
    
    result["protocol"] = analysis.protocol;
    result["source"] = analysis.source;
    result["destination"] = analysis.destination;
    result["source_port"] = analysis.sourcePort;
    result["destination_port"] = analysis.destinationPort;
    result["is_suspicious"] = analysis.isSuspicious;
    result["anomalies"] = analysis.anomalies;
    result["analysis"] = analysis.analysis;
    
    return result;
}

QStringList AdvancedNetworkDefenseDBusInterface::DetectAnomalies(const QByteArray& packetData) {
    if (!m_system || !m_system->getPacketAnalyzer()) {
        return QStringList();
    }
    
    return m_system->getPacketAnalyzer()->detectAnomalies(packetData);
}

bool AdvancedNetworkDefenseDBusInterface::AddIDSRule(const QString& name, const QString& pattern, const QString& action) {
    if (!m_system || !m_system->getIDSRulesEngine()) {
        return false;
    }
    
    IDSRule rule;
    rule.name = name;
    rule.pattern = pattern;
    rule.action = action;
    rule.priority = 100;
    rule.enabled = true;
    
    return m_system->getIDSRulesEngine()->addRule(rule);
}

QStringList AdvancedNetworkDefenseDBusInterface::GetIDSRules() {
    QStringList result;
    
    if (!m_system || !m_system->getIDSRulesEngine()) {
        return result;
    }
    
    QList<IDSRule> rules = m_system->getIDSRulesEngine()->getAllRules();
    for (const IDSRule& rule : rules) {
        result.append(rule.id);
    }
    
    return result;
}

QString AdvancedNetworkDefenseDBusInterface::CreateWorkflow(const QString& name, const QStringList& steps) {
    if (!m_system || !m_system->getIncidentResponseWorkflow()) {
        return QString();
    }
    
    QList<WorkflowStep> workflowSteps;
    for (const QString& stepJson : steps) {
        QJsonDocument doc = QJsonDocument::fromJson(stepJson.toUtf8());
        if (!doc.isNull()) {
            QVariantMap stepMap = doc.object().toVariantMap();
            WorkflowStep step;
            step.id = stepMap["id"].toString();
            step.name = stepMap["name"].toString();
            step.action = stepMap["action"].toString();
            step.parameters = stepMap["parameters"].toMap();
            workflowSteps.append(step);
        }
    }
    
    return m_system->getIncidentResponseWorkflow()->createWorkflow(name, workflowSteps);
}

bool AdvancedNetworkDefenseDBusInterface::ExecuteWorkflow(const QString& workflowId, const QVariantMap& threat) {
    if (!m_system || !m_system->getIncidentResponseWorkflow()) {
        return false;
    }
    
    // Convert threat map to MLThreatResult
    MLThreatResult mlThreat;
    mlThreat.id = threat["id"].toString();
    mlThreat.type = static_cast<MLThreatType>(threat["type"].toInt());
    mlThreat.description = threat["description"].toString();
    mlThreat.confidence = threat["confidence"].toDouble();
    
    return m_system->getIncidentResponseWorkflow()->executeWorkflow(workflowId, mlThreat);
}

