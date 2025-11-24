#include "honeypot_manager.h"
#include <QDebug>

HoneypotManager::HoneypotManager(QObject* parent)
    : QObject(parent)
{
}

HoneypotManager::~HoneypotManager() {
    // Clean up honeypots
    for (Honeypot* honeypot : m_honeypots.values()) {
        honeypot->stop();
        honeypot->deleteLater();
    }
    m_honeypots.clear();
}

bool HoneypotManager::initialize() {
    return true;
}

QString HoneypotManager::deployHoneypot(const HoneypotConfig& config) {
    HoneypotConfig configCopy = config;
    if (configCopy.id.isEmpty()) {
        configCopy.id = generateHoneypotId();
    }
    
    Honeypot* honeypot = new Honeypot(this);
    if (!honeypot->initialize(configCopy)) {
        delete honeypot;
        return QString();
    }
    
    m_honeypots[configCopy.id] = honeypot;
    
    // Connect signals
    connect(honeypot, &Honeypot::interactionDetected, this, [this, configCopy](const QVariantMap& interactionData) {
        emit threatDetected(configCopy.id, interactionData);
    });
    
    emit honeypotDeployed(configCopy.id);
    
    return configCopy.id;
}

bool HoneypotManager::removeHoneypot(const QString& honeypotId) {
    if (!m_honeypots.contains(honeypotId)) {
        return false;
    }
    
    Honeypot* honeypot = m_honeypots[honeypotId];
    honeypot->stop();
    honeypot->deleteLater();
    m_honeypots.remove(honeypotId);
    
    return true;
}

Honeypot* HoneypotManager::getHoneypot(const QString& honeypotId) const {
    return m_honeypots.value(honeypotId, nullptr);
}

QStringList HoneypotManager::getAllHoneypots() const {
    return m_honeypots.keys();
}

QVariantMap HoneypotManager::getThreatIntelligence() const {
    QVariantMap intelligence;
    
    int totalInteractions = 0;
    for (Honeypot* honeypot : m_honeypots.values()) {
        totalInteractions += honeypot->getInteractionCount();
    }
    
    intelligence["total_honeypots"] = m_honeypots.size();
    intelligence["total_interactions"] = totalInteractions;
    intelligence["active_honeypots"] = 0;  // Would count running honeypots
    
    return intelligence;
}

QString HoneypotManager::generateHoneypotId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}






