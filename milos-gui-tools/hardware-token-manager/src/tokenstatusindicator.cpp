#include "tokenstatusindicator.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

TokenStatusIndicator::TokenStatusIndicator(QObject* parent)
    : QObject(parent)
    , m_connected(false)
    , m_active(false)
    , m_batteryLevel(-1)
    , m_tokenName("")
    , m_status("disconnected")
    , m_monitoringTimer(new QTimer(this))
    , m_tokenInterface(nullptr)
{
    m_tokenInterface = new QDBusInterface(
        "org.milos.HardwareToken",
        "/org/milos/HardwareToken",
        "org.milos.HardwareToken",
        QDBusConnection::systemBus(),
        this
    );
    
    m_monitoringTimer->setInterval(2000); // Check every 2 seconds
    connect(m_monitoringTimer, &QTimer::timeout, this, &TokenStatusIndicator::onMonitoringTimer);
    
    updateStatus();
}

TokenStatusIndicator::~TokenStatusIndicator()
{
    stopMonitoring();
}

void TokenStatusIndicator::updateStatus()
{
    checkTokenStatus();
    checkTokenActivity();
    checkBatteryLevel();
}

void TokenStatusIndicator::startMonitoring()
{
    if (!m_monitoringTimer->isActive()) {
        m_monitoringTimer->start();
        updateStatus();
    }
}

void TokenStatusIndicator::stopMonitoring()
{
    if (m_monitoringTimer->isActive()) {
        m_monitoringTimer->stop();
    }
}

void TokenStatusIndicator::onMonitoringTimer()
{
    updateStatus();
}

void TokenStatusIndicator::checkTokenStatus()
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        if (m_connected) {
            m_connected = false;
            m_status = "disconnected";
            emit connectedChanged();
            emit statusChanged();
        }
        return;
    }
    
    QDBusReply<QString> reply = m_tokenInterface->call("GetTokenStatus");
    if (reply.isValid()) {
        QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            bool wasConnected = m_connected;
            
            m_connected = obj.value("connected").toBool(false);
            m_tokenName = obj.value("token_name").toString("");
            m_status = m_connected ? "connected" : "disconnected";
            
            if (wasConnected != m_connected) {
                emit connectedChanged();
                emit statusChanged();
            }
            if (m_tokenName != obj.value("token_name").toString("")) {
                emit tokenNameChanged();
            }
        }
    } else {
        if (m_connected) {
            m_connected = false;
            m_status = "disconnected";
            emit connectedChanged();
            emit statusChanged();
        }
    }
}

void TokenStatusIndicator::checkTokenActivity()
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        if (m_active) {
            m_active = false;
            emit activeChanged();
        }
        return;
    }
    
    QDBusReply<bool> reply = m_tokenInterface->call("IsTokenActive");
    if (reply.isValid()) {
        bool wasActive = m_active;
        m_active = reply.value();
        
        if (wasActive != m_active) {
            emit activeChanged();
            if (m_active) {
                emit tokenActivityDetected();
            }
        }
    }
}

void TokenStatusIndicator::checkBatteryLevel()
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        if (m_batteryLevel != -1) {
            m_batteryLevel = -1;
            emit batteryLevelChanged();
        }
        return;
    }
    
    QDBusReply<int> reply = m_tokenInterface->call("GetBatteryLevel");
    if (reply.isValid()) {
        int level = reply.value();
        if (level != m_batteryLevel) {
            m_batteryLevel = level;
            emit batteryLevelChanged();
            
            // Emit low battery warning if below 20%
            if (level >= 0 && level < 20) {
                emit lowBatteryWarning(level);
            }
        }
    } else {
        // Battery not supported or error
        if (m_batteryLevel != -1) {
            m_batteryLevel = -1;
            emit batteryLevelChanged();
        }
    }
}

