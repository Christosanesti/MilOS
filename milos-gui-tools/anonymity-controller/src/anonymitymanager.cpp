#include "anonymitymanager.h"
#include <QTimer>
#include <QProcess>
#include <QDebug>
#include <QDBusReply>
#include <QDBusConnection>

AnonymityManager::AnonymityManager(QObject *parent)
    : QObject(parent)
    , m_torEnabled(false)
    , m_i2pEnabled(false)
    , m_torStatus("disconnected")
    , m_i2pStatus("disconnected")
    , m_statusTimer(new QTimer(this))
    , m_bandwidthTimer(new QTimer(this))
    , m_torInterface(nullptr)
    , m_i2pInterface(nullptr)
{
    // Try to connect to Tor D-Bus service
    m_torInterface = new QDBusInterface("org.milos.Tor",
                                        "/org/milos/Tor",
                                        "org.milos.Tor",
                                        QDBusConnection::sessionBus(),
                                        this);
    
    // Try to connect to I2P D-Bus service
    m_i2pInterface = new QDBusInterface("org.milos.I2P",
                                       "/org/milos/I2P",
                                       "org.milos.I2P",
                                       QDBusConnection::sessionBus(),
                                       this);
    
    connect(m_statusTimer, &QTimer::timeout, this, &AnonymityManager::updateStatus);
    connect(m_bandwidthTimer, &QTimer::timeout, this, &AnonymityManager::updateBandwidth);
    
    m_statusTimer->setInterval(2000); // Update every 2 seconds
    m_bandwidthTimer->setInterval(5000); // Update every 5 seconds
    
    refreshStatus();
}

void AnonymityManager::toggleTor()
{
    if (m_torInterface && m_torInterface->isValid()) {
        QDBusReply<bool> reply = m_torInterface->call("Toggle");
        if (reply.isValid()) {
            m_torEnabled = reply.value();
            emit torEnabledChanged();
            updateTorStatus();
        } else {
            emit error(QString("Failed to toggle Tor: %1").arg(reply.error().message()));
        }
    } else {
        // Fallback: use systemctl or tor service directly
        QProcess process;
        if (m_torEnabled) {
            process.start("systemctl", QStringList() << "--user" << "stop" << "tor");
        } else {
            process.start("systemctl", QStringList() << "--user" << "start" << "tor");
        }
        process.waitForFinished(2000);
        
        if (process.exitCode() == 0) {
            m_torEnabled = !m_torEnabled;
            emit torEnabledChanged();
            updateTorStatus();
        } else {
            emit error("Failed to toggle Tor service");
        }
    }
}

void AnonymityManager::toggleI2P()
{
    if (m_i2pInterface && m_i2pInterface->isValid()) {
        QDBusReply<bool> reply = m_i2pInterface->call("Toggle");
        if (reply.isValid()) {
            m_i2pEnabled = reply.value();
            emit i2pEnabledChanged();
            updateI2PStatus();
        } else {
            emit error(QString("Failed to toggle I2P: %1").arg(reply.error().message()));
        }
    } else {
        // Fallback: use systemctl or i2p service directly
        QProcess process;
        if (m_i2pEnabled) {
            process.start("systemctl", QStringList() << "--user" << "stop" << "i2p");
        } else {
            process.start("systemctl", QStringList() << "--user" << "start" << "i2p");
        }
        process.waitForFinished(2000);
        
        if (process.exitCode() == 0) {
            m_i2pEnabled = !m_i2pEnabled;
            emit i2pEnabledChanged();
            updateI2PStatus();
        } else {
            emit error("Failed to toggle I2P service");
        }
    }
}

void AnonymityManager::startMonitoring()
{
    m_statusTimer->start();
    m_bandwidthTimer->start();
    refreshStatus();
}

void AnonymityManager::stopMonitoring()
{
    m_statusTimer->stop();
    m_bandwidthTimer->stop();
}

void AnonymityManager::refreshStatus()
{
    updateTorStatus();
    updateI2PStatus();
    updateRoutingInfo();
}

void AnonymityManager::updateStatus()
{
    updateTorStatus();
    updateI2PStatus();
}

void AnonymityManager::updateBandwidth()
{
    updateTorBandwidth();
    updateI2PBandwidth();
}

bool AnonymityManager::checkTorService()
{
    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "is-active" << "--quiet" << "tor");
    process.waitForFinished(1000);
    return process.exitCode() == 0;
}

bool AnonymityManager::checkI2PService()
{
    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "is-active" << "--quiet" << "i2p");
    process.waitForFinished(1000);
    return process.exitCode() == 0;
}

void AnonymityManager::updateTorStatus()
{
    bool wasEnabled = m_torEnabled;
    m_torEnabled = checkTorService();
    
    if (wasEnabled != m_torEnabled) {
        emit torEnabledChanged();
    }
    
    QString newStatus;
    if (m_torInterface && m_torInterface->isValid()) {
        QDBusReply<QString> reply = m_torInterface->call("GetStatus");
        if (reply.isValid()) {
            newStatus = reply.value();
        }
    }
    
    if (newStatus.isEmpty()) {
        // Determine status from service state
        if (m_torEnabled) {
            // Check if actually connected
            QProcess process;
            process.start("curl", QStringList() << "--socks5-hostname" << "127.0.0.1:9050" << 
                         "--max-time" << "3" << "https://check.torproject.org/api/ip");
            process.waitForFinished(3000);
            if (process.exitCode() == 0) {
                newStatus = "connected";
            } else {
                newStatus = "connecting";
            }
        } else {
            newStatus = "disconnected";
        }
    }
    
    if (m_torStatus != newStatus) {
        m_torStatus = newStatus;
        emit torStatusChanged();
    }
}

void AnonymityManager::updateI2PStatus()
{
    bool wasEnabled = m_i2pEnabled;
    m_i2pEnabled = checkI2PService();
    
    if (wasEnabled != m_i2pEnabled) {
        emit i2pEnabledChanged();
    }
    
    QString newStatus;
    if (m_i2pInterface && m_i2pInterface->isValid()) {
        QDBusReply<QString> reply = m_i2pInterface->call("GetStatus");
        if (reply.isValid()) {
            newStatus = reply.value();
        }
    }
    
    if (newStatus.isEmpty()) {
        if (m_i2pEnabled) {
            // Check I2P router status
            QProcess process;
            process.start("curl", QStringList() << "--proxy" << "http://127.0.0.1:4444" << 
                         "--max-time" << "3" << "http://127.0.0.1:7657");
            process.waitForFinished(3000);
            if (process.exitCode() == 0) {
                newStatus = "connected";
            } else {
                newStatus = "connecting";
            }
        } else {
            newStatus = "disconnected";
        }
    }
    
    if (m_i2pStatus != newStatus) {
        m_i2pStatus = newStatus;
        emit i2pStatusChanged();
    }
}

void AnonymityManager::updateTorBandwidth()
{
    QVariantMap bandwidth;
    
    if (m_torInterface && m_torInterface->isValid()) {
        QDBusReply<QVariantMap> reply = m_torInterface->call("GetBandwidth");
        if (reply.isValid()) {
            bandwidth = reply.value();
        }
    }
    
    if (bandwidth.isEmpty() && m_torEnabled) {
        // Fallback: read from Tor control port or stats
        bandwidth["upload"] = 0;
        bandwidth["download"] = 0;
        bandwidth["uploadRate"] = 0;
        bandwidth["downloadRate"] = 0;
    }
    
    m_torBandwidth = bandwidth;
    emit torBandwidthChanged();
}

void AnonymityManager::updateI2PBandwidth()
{
    QVariantMap bandwidth;
    
    if (m_i2pInterface && m_i2pInterface->isValid()) {
        QDBusReply<QVariantMap> reply = m_i2pInterface->call("GetBandwidth");
        if (reply.isValid()) {
            bandwidth = reply.value();
        }
    }
    
    if (bandwidth.isEmpty() && m_i2pEnabled) {
        // Fallback: read from I2P router stats
        bandwidth["upload"] = 0;
        bandwidth["download"] = 0;
        bandwidth["uploadRate"] = 0;
        bandwidth["downloadRate"] = 0;
    }
    
    m_i2pBandwidth = bandwidth;
    emit i2pBandwidthChanged();
}

void AnonymityManager::updateRoutingInfo()
{
    QVariantMap routing;
    
    if (m_torEnabled) {
        routing["tor"] = QVariantMap({
            {"enabled", true},
            {"status", m_torStatus},
            {"socksPort", 9050},
            {"controlPort", 9051}
        });
    } else {
        routing["tor"] = QVariantMap({{"enabled", false}});
    }
    
    if (m_i2pEnabled) {
        routing["i2p"] = QVariantMap({
            {"enabled", true},
            {"status", m_i2pStatus},
            {"httpProxy", "127.0.0.1:4444"},
            {"routerPort", 7657}
        });
    } else {
        routing["i2p"] = QVariantMap({{"enabled", false}});
    }
    
    m_routingInfo = routing;
    emit routingInfoChanged();
}

