#include "peer_discovery.h"
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>
#include <QUuid>

PeerDiscovery::PeerDiscovery(QObject* parent)
    : QObject(parent)
    , m_discoveryPort(0)
    , m_discoveryTimer(new QTimer(this))
{
    connect(m_discoveryTimer, &QTimer::timeout, this, &PeerDiscovery::onDiscoveryTimeout);
}

PeerDiscovery::~PeerDiscovery() {
    stop();
}

bool PeerDiscovery::initialize(const QString& networkSegment, quint16 discoveryPort) {
    m_networkSegment = networkSegment;
    m_discoveryPort = discoveryPort;
    return true;
}

bool PeerDiscovery::start() {
    if (m_discoveryTimer->isActive()) {
        return true;
    }
    
    // Start periodic discovery
    m_discoveryTimer->start(5000);  // Discover every 5 seconds
    broadcastDiscovery();
    
    return true;
}

void PeerDiscovery::stop() {
    if (m_discoveryTimer->isActive()) {
        m_discoveryTimer->stop();
    }
}

QList<QString> PeerDiscovery::discoverPeers() const {
    return m_discoveredPeers;
}

bool PeerDiscovery::validatePeer(const QString& peerId, const QString& userId) const {
    // Validate peer using USB authorization
    // For now, basic validation
    return !peerId.isEmpty() && !userId.isEmpty();
}

void PeerDiscovery::onDiscoveryTimeout() {
    broadcastDiscovery();
}

void PeerDiscovery::broadcastDiscovery() {
    // Broadcast discovery packet on network segment
    // In production, would use UDP broadcast
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& iface : interfaces) {
        if (!iface.isUp() || iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }
        
        QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry& entry : entries) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                // Check if in network segment
                QString ipStr = entry.ip().toString();
                if (ipStr.startsWith(m_networkSegment) || m_networkSegment.isEmpty()) {
                    // Broadcast discovery packet
                    // In production, would send UDP broadcast
                }
            }
        }
    }
}

void PeerDiscovery::processDiscoveryResponse(const QByteArray& data, const QHostAddress& sender) {
    // Process discovery response
    // In production, would parse response and add peer
    QString peerId = QString::fromUtf8(data);
    if (!m_discoveredPeers.contains(peerId)) {
        m_discoveredPeers.append(peerId);
        m_peerAddresses[peerId] = sender;
        emit peerDiscovered(peerId, sender);
    }
}

