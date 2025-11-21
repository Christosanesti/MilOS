#include "satcom_protocol.h"
#include <QUuid>
#include <QDebug>

SatComProtocol::SatComProtocol(QObject* parent)
    : QObject(parent)
    , m_protocolType(SatComProtocolType::VSAT)
    , m_availableBandwidth(10.0)  // 10 Mbps default
{
}

SatComProtocol::~SatComProtocol() {
}

bool SatComProtocol::initialize(SatComProtocolType protocolType, const QString& satelliteId) {
    m_protocolType = protocolType;
    m_satelliteId = satelliteId;
    
    // Set bandwidth based on protocol
    switch (protocolType) {
        case SatComProtocolType::MILSTAR:
            m_availableBandwidth = 1.5;  // Low data rate
            break;
        case SatComProtocolType::AEHF:
            m_availableBandwidth = 8.0;
            break;
        case SatComProtocolType::WGS:
            m_availableBandwidth = 3.0;
            break;
        case SatComProtocolType::Iridium:
            m_availableBandwidth = 0.128;  // Very low
            break;
        case SatComProtocolType::Inmarsat:
            m_availableBandwidth = 0.5;
            break;
        case SatComProtocolType::VSAT:
            m_availableBandwidth = 10.0;
            break;
        default:
            m_availableBandwidth = 5.0;
            break;
    }
    
    emit bandwidthChanged(m_availableBandwidth);
    
    return true;
}

bool SatComProtocol::sendMessage(const SatComMessage& message) {
    // Calculate bandwidth usage
    double bandwidthUsed = calculateBandwidthUsage(message.payload);
    
    if (bandwidthUsed > m_availableBandwidth) {
        qWarning() << "Message exceeds available bandwidth";
        return false;
    }
    
    // In production, would send via actual satellite link
    qDebug() << "Sending satellite message via" << message.protocol << "to" << message.to;
    
    // Update available bandwidth
    m_availableBandwidth -= bandwidthUsed;
    emit bandwidthChanged(m_availableBandwidth);
    
    return true;
}

SatComMessage SatComProtocol::receiveMessage() {
    SatComMessage message;
    
    // In production, would receive from actual satellite link
    // For now, return empty message
    
    return message;
}

double SatComProtocol::calculateBandwidthUsage(const QByteArray& payload) const {
    // Calculate bandwidth in Mbps (simplified)
    // Assuming transmission time and overhead
    int sizeBytes = payload.size();
    double sizeMbits = (sizeBytes * 8.0) / 1000000.0;  // Convert to Mbps
    
    // Add protocol overhead (typically 10-20%)
    return sizeMbits * 1.15;
}

