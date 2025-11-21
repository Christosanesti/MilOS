#include "dbus_interface.h"
#include "ai_security_monitoring_system.h"
#include "threat_detector.h"
#include "incident_response.h"
#include "system_monitor.h"
#include <QDebug>
#include <QDBusConnection>

AISecurityMonitoringDBusInterface::AISecurityMonitoringDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

AISecurityMonitoringDBusInterface::~AISecurityMonitoringDBusInterface() {
}

bool AISecurityMonitoringDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.AISecurityMonitoring")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/AISecurityMonitoring", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void AISecurityMonitoringDBusInterface::setSystem(AISecurityMonitoringSystem* system) {
    m_system = system;
}

bool AISecurityMonitoringDBusInterface::StartMonitoring() {
    if (!m_system) {
        return false;
    }
    
    return m_system->start();
}

void AISecurityMonitoringDBusInterface::StopMonitoring() {
    if (m_system) {
        m_system->stop();
    }
}

bool AISecurityMonitoringDBusInterface::IsMonitoring() {
    if (!m_system || !m_system->getThreatDetector()) {
        return false;
    }
    
    return m_system->getThreatDetector()->isMonitoring();
}

QVariantMap AISecurityMonitoringDBusInterface::GetThreats() {
    QVariantMap result;
    
    if (!m_system || !m_system->getThreatDetector()) {
        return result;
    }
    
    QList<DetectedThreat> threats = m_system->getThreatDetector()->getDetectedThreats();
    
    QVariantList threatList;
    for (const DetectedThreat& threat : threats) {
        QVariantMap threatMap;
        threatMap["id"] = threat.id;
        threatMap["type"] = static_cast<int>(threat.type);
        threatMap["severity"] = static_cast<int>(threat.severity);
        threatMap["description"] = threat.description;
        threatMap["detected_at"] = threat.detectedAt.toString(Qt::ISODate);
        threatMap["is_contained"] = threat.isContained;
        threatList.append(threatMap);
    }
    
    result["threats"] = threatList;
    result["count"] = threats.size();
    result["statistics"] = m_system->getThreatDetector()->getStatistics();
    
    return result;
}

QVariantMap AISecurityMonitoringDBusInterface::GetSystemMetrics() {
    if (!m_system || !m_system->getSystemMonitor()) {
        return QVariantMap();
    }
    
    return m_system->getSystemMonitor()->systemMetrics();
}

QVariantList AISecurityMonitoringDBusInterface::GetProcessList() {
    if (!m_system || !m_system->getSystemMonitor()) {
        return QVariantList();
    }
    
    return m_system->getSystemMonitor()->getProcessList();
}

QVariantList AISecurityMonitoringDBusInterface::GetNetworkConnections() {
    if (!m_system || !m_system->getSystemMonitor()) {
        return QVariantList();
    }
    
    return m_system->getSystemMonitor()->getNetworkConnections();
}

bool AISecurityMonitoringDBusInterface::ContainThreat(const QString& threatId) {
    if (!m_system || !m_system->getIncidentResponse()) {
        return false;
    }
    
    return m_system->getIncidentResponse()->containThreat(threatId);
}

QVariantMap AISecurityMonitoringDBusInterface::GetStatistics() {
    QVariantMap result;
    
    if (!m_system || !m_system->getThreatDetector()) {
        return result;
    }
    
    result = m_system->getThreatDetector()->getStatistics();
    result["monitoring"] = m_system->getThreatDetector()->isMonitoring();
    
    return result;
}

