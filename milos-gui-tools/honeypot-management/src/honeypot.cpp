#include "honeypot.h"
#include <QUuid>
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>

Honeypot::Honeypot(QObject* parent)
    : QObject(parent)
    , m_status(HoneypotStatus::Stopped)
    , m_interactionCount(0)
{
}

Honeypot::~Honeypot() {
    stop();
}

bool Honeypot::initialize(const HoneypotConfig& config) {
    m_config = config;
    return true;
}

bool Honeypot::start() {
    if (m_status == HoneypotStatus::Running) {
        return true;
    }
    
    updateStatus(HoneypotStatus::Starting);
    
    // In production, would start actual honeypot service
    // For now, simulate starting
    QTimer::singleShot(2000, this, [this]() {
        updateStatus(HoneypotStatus::Running);
        
        // Simulate periodic interactions
        QTimer* interactionTimer = new QTimer(this);
        connect(interactionTimer, &QTimer::timeout, this, &Honeypot::simulateInteraction);
        interactionTimer->start(30000);  // Every 30 seconds
    });
    
    return true;
}

void Honeypot::stop() {
    if (m_status == HoneypotStatus::Stopped) {
        return;
    }
    
    updateStatus(HoneypotStatus::Stopping);
    
    // In production, would stop actual honeypot service
    QTimer::singleShot(1000, this, [this]() {
        updateStatus(HoneypotStatus::Stopped);
    });
}

QVariantMap Honeypot::getThreatData() const {
    QVariantMap data;
    
    data["honeypot_id"] = m_config.id;
    data["type"] = static_cast<int>(m_config.type);
    data["interaction_count"] = m_interactionCount;
    data["status"] = static_cast<int>(m_status);
    
    return data;
}

void Honeypot::updateStatus(HoneypotStatus status) {
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

void Honeypot::simulateInteraction() {
    if (m_status != HoneypotStatus::Running) {
        return;
    }
    
    // Simulate interaction
    m_interactionCount++;
    
    QVariantMap interactionData;
    interactionData["honeypot_id"] = m_config.id;
    interactionData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    interactionData["source_ip"] = QString("192.168.%1.%2")
                                   .arg(QRandomGenerator::global()->bounded(1, 255))
                                   .arg(QRandomGenerator::global()->bounded(1, 255));
    interactionData["action"] = "connection_attempt";
    
    emit interactionDetected(interactionData);
}






