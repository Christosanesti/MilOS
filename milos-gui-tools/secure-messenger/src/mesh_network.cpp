#include "mesh_network.h"
#include <QUuid>
#include <QDebug>
#include <QNetworkInterface>

MeshNetwork::MeshNetwork(QObject* parent)
    : QObject(parent)
    , m_port(0)
{
    m_localPeerId = generatePeerId();
}

MeshNetwork::~MeshNetwork() {
}

bool MeshNetwork::initialize(const QHostAddress& localAddress, quint16 port) {
    m_localAddress = localAddress;
    m_port = port;
    return true;
}

bool MeshNetwork::addPeer(const PeerInfo& peerInfo) {
    if (peerInfo.peerId.isEmpty()) {
        return false;
    }
    
    m_peers[peerInfo.peerId] = peerInfo;
    updateRoutingTable();
    emit peerAdded(peerInfo.peerId);
    
    return true;
}

bool MeshNetwork::removePeer(const QString& peerId) {
    if (!m_peers.contains(peerId)) {
        return false;
    }
    
    m_peers.remove(peerId);
    updateRoutingTable();
    emit peerRemoved(peerId);
    
    return true;
}

PeerInfo MeshNetwork::getPeerInfo(const QString& peerId) const {
    return m_peers.value(peerId, PeerInfo());
}

QList<PeerInfo> MeshNetwork::getAllPeers() const {
    return m_peers.values();
}

QList<QString> MeshNetwork::findRoute(const QString& targetPeerId) const {
    if (!m_peers.contains(targetPeerId)) {
        return QList<QString>();
    }
    
    // Simple routing: direct connection if peer exists
    // In production, would implement proper mesh routing algorithm
    QList<QString> route;
    route.append(m_localPeerId);
    route.append(targetPeerId);
    
    return route;
}

bool MeshNetwork::sendMessage(const QString& targetPeerId, const QByteArray& data) {
    QList<QString> route = findRoute(targetPeerId);
    if (route.isEmpty()) {
        return false;
    }
    
    // In production, would implement actual message sending via network
    // For now, just emit signal
    emit messageReceived(targetPeerId, data);
    
    return true;
}

QString MeshNetwork::generatePeerId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void MeshNetwork::updateRoutingTable() {
    // Update routing table based on current peers
    // In production, would implement proper mesh routing algorithm
}

