#include "dbus_interface.h"
#include "honeypot_management_system.h"
#include "honeypot.h"
#include <QDebug>
#include <QDBusConnection>

HoneypotManagementDBusInterface::HoneypotManagementDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

HoneypotManagementDBusInterface::~HoneypotManagementDBusInterface() {
}

bool HoneypotManagementDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.HoneypotManagement")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/HoneypotManagement", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void HoneypotManagementDBusInterface::setSystem(HoneypotManagementSystem* system) {
    m_system = system;
}

QString HoneypotManagementDBusInterface::DeployHoneypot(int type, const QString& name, const QString& host, int port) {
    if (!m_system || !m_system->getHoneypotManager()) {
        return QString();
    }
    
    HoneypotConfig config;
    config.name = name;
    config.type = static_cast<HoneypotType>(type);
    config.host = host;
    config.port = port;
    config.enabled = true;
    
    return m_system->getHoneypotManager()->deployHoneypot(config);
}

bool HoneypotManagementDBusInterface::RemoveHoneypot(const QString& honeypotId) {
    if (!m_system || !m_system->getHoneypotManager()) {
        return false;
    }
    
    return m_system->getHoneypotManager()->removeHoneypot(honeypotId);
}

bool HoneypotManagementDBusInterface::StartHoneypot(const QString& honeypotId) {
    if (!m_system || !m_system->getHoneypotManager()) {
        return false;
    }
    
    Honeypot* honeypot = m_system->getHoneypotManager()->getHoneypot(honeypotId);
    if (!honeypot) {
        return false;
    }
    
    return honeypot->start();
}

bool HoneypotManagementDBusInterface::StopHoneypot(const QString& honeypotId) {
    if (!m_system || !m_system->getHoneypotManager()) {
        return false;
    }
    
    Honeypot* honeypot = m_system->getHoneypotManager()->getHoneypot(honeypotId);
    if (!honeypot) {
        return false;
    }
    
    honeypot->stop();
    return true;
}

QStringList HoneypotManagementDBusInterface::GetHoneypots() {
    if (!m_system || !m_system->getHoneypotManager()) {
        return QStringList();
    }
    
    return m_system->getHoneypotManager()->getAllHoneypots();
}

QVariantMap HoneypotManagementDBusInterface::GetHoneypotStatus(const QString& honeypotId) {
    QVariantMap result;
    
    if (!m_system || !m_system->getHoneypotManager()) {
        return result;
    }
    
    Honeypot* honeypot = m_system->getHoneypotManager()->getHoneypot(honeypotId);
    if (!honeypot) {
        return result;
    }
    
    result["id"] = honeypot->id();
    result["status"] = static_cast<int>(honeypot->status());
    result["interaction_count"] = honeypot->getInteractionCount();
    result["threat_data"] = honeypot->getThreatData();
    
    return result;
}

QVariantMap HoneypotManagementDBusInterface::GetThreatIntelligence() {
    QVariantMap result;
    
    if (!m_system || !m_system->getHoneypotManager() || !m_system->getThreatAnalyzer()) {
        return result;
    }
    
    result["honeypot_intelligence"] = m_system->getHoneypotManager()->getThreatIntelligence();
    result["threat_statistics"] = m_system->getThreatAnalyzer()->getThreatStatistics();
    
    return result;
}

