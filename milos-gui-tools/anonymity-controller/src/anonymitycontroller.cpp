#include "anonymitycontroller.h"
#include <QDebug>
#include <QDBusReply>

AnonymityController::AnonymityController(QObject* parent)
    : QObject(parent)
    , m_torConnected(false)
    , m_i2pConnected(false)
    , m_dbusConnection(QDBusConnection::sessionBus())
{
    updateStatus();
}

QString AnonymityController::connectionStatus() const
{
    if (m_torConnected && m_i2pConnected) {
        return "Both Connected";
    } else if (m_torConnected) {
        return "Tor Connected";
    } else if (m_i2pConnected) {
        return "I2P Connected";
    }
    return "Disconnected";
}

void AnonymityController::toggleTor()
{
    QDBusInterface torInterface("org.milos.Tor", "/org/milos/Tor", "org.milos.Tor", m_dbusConnection);
    
    if (!torInterface.isValid()) {
        qWarning() << "Tor service not available";
        return;
    }
    
    QDBusReply<bool> reply = torInterface.call("Toggle");
    if (reply.isValid()) {
        m_torConnected = reply.value();
        emit torStatusChanged(m_torConnected);
        emit statusChanged();
    }
}

void AnonymityController::toggleI2P()
{
    QDBusInterface i2pInterface("org.milos.I2P", "/org/milos/I2P", "org.milos.I2P", m_dbusConnection);
    
    if (!i2pInterface.isValid()) {
        qWarning() << "I2P service not available";
        return;
    }
    
    QDBusReply<bool> reply = i2pInterface.call("Toggle");
    if (reply.isValid()) {
        m_i2pConnected = reply.value();
        emit i2pStatusChanged(m_i2pConnected);
        emit statusChanged();
    }
}

void AnonymityController::updateStatus()
{
    // Query Tor status
    QDBusInterface torInterface("org.milos.Tor", "/org/milos/Tor", "org.milos.Tor", m_dbusConnection);
    if (torInterface.isValid()) {
        QDBusReply<bool> torReply = torInterface.call("IsConnected");
        if (torReply.isValid()) {
            m_torConnected = torReply.value();
        }
    }
    
    // Query I2P status
    QDBusInterface i2pInterface("org.milos.I2P", "/org/milos/I2P", "org.milos.I2P", m_dbusConnection);
    if (i2pInterface.isValid()) {
        QDBusReply<bool> i2pReply = i2pInterface.call("IsConnected");
        if (i2pReply.isValid()) {
            m_i2pConnected = i2pReply.value();
        }
    }
    
    emit statusChanged();
}

