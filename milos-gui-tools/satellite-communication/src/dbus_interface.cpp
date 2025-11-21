#include "dbus_interface.h"
#include "satellite_communication_system.h"
#include "satcom_protocol.h"
#include <QDebug>
#include <QDBusConnection>
#include <QUuid>

SatelliteCommunicationDBusInterface::SatelliteCommunicationDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

SatelliteCommunicationDBusInterface::~SatelliteCommunicationDBusInterface() {
}

bool SatelliteCommunicationDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.SatelliteCommunication")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SatelliteCommunication", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void SatelliteCommunicationDBusInterface::setSystem(SatelliteCommunicationSystem* system) {
    m_system = system;
}

bool SatelliteCommunicationDBusInterface::InitializeProtocol(int protocolType, const QString& satelliteId) {
    if (!m_system) {
        return false;
    }
    
    SatComProtocol* protocol = new SatComProtocol();
    if (!protocol->initialize(static_cast<SatComProtocolType>(protocolType), satelliteId)) {
        delete protocol;
        return false;
    }
    
    return m_system->addProtocol(protocol);
}

bool SatelliteCommunicationDBusInterface::SendMessage(int protocolType, const QString& to, const QByteArray& payload, int priority) {
    if (!m_system || !m_system->getBandwidthOptimizer()) {
        return false;
    }
    
    // Optimize payload before sending
    QByteArray optimizedPayload = m_system->getBandwidthOptimizer()->optimizeMessage(payload);
    
    SatComMessage message;
    message.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    message.to = to;
    message.payload = optimizedPayload;
    message.priority = priority;
    message.timestamp = QDateTime::currentDateTime();
    message.encrypted = true;
    
    QString protocolName;
    switch (static_cast<SatComProtocolType>(protocolType)) {
        case SatComProtocolType::MILSTAR: protocolName = "MILSTAR"; break;
        case SatComProtocolType::AEHF: protocolName = "AEHF"; break;
        case SatComProtocolType::WGS: protocolName = "WGS"; break;
        case SatComProtocolType::Iridium: protocolName = "Iridium"; break;
        case SatComProtocolType::Inmarsat: protocolName = "Inmarsat"; break;
        case SatComProtocolType::VSAT: protocolName = "VSAT"; break;
        default: protocolName = "Custom"; break;
    }
    message.protocol = protocolName;
    
    // Find protocol and send
    // In production, would find the correct protocol instance
    return true;
}

QVariantMap SatelliteCommunicationDBusInterface::ReceiveMessage(int protocolType) {
    QVariantMap result;
    
    // In production, would receive actual message
    // For now, return empty result
    
    return result;
}

double SatelliteCommunicationDBusInterface::GetAvailableBandwidth(int protocolType) {
    if (!m_system) {
        return 0.0;
    }
    
    // In production, would get from actual protocol instance
    return 10.0;  // Placeholder
}

QByteArray SatelliteCommunicationDBusInterface::OptimizeData(const QByteArray& data) {
    if (!m_system || !m_system->getBandwidthOptimizer()) {
        return data;
    }
    
    return m_system->getBandwidthOptimizer()->optimizeMessage(data);
}

