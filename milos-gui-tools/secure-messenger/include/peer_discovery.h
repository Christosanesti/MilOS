#ifndef PEER_DISCOVERY_H
#define PEER_DISCOVERY_H

#include "mesh_network.h"
#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QList>
#include <QTimer>

/**
 * @brief Peer Discovery
 * 
 * Discovers peers within the authorized network segment.
 */
class PeerDiscovery : public QObject {
    Q_OBJECT

public:
    explicit PeerDiscovery(QObject* parent = nullptr);
    ~PeerDiscovery();

    /**
     * @brief Initialize peer discovery
     * @param networkSegment Network segment/subnet
     * @param discoveryPort Port for discovery protocol
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& networkSegment, quint16 discoveryPort);

    /**
     * @brief Start peer discovery
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop peer discovery
     */
    void stop();

    /**
     * @brief Discover peers
     * @return List of discovered peer IDs
     */
    QList<QString> discoverPeers() const;

    /**
     * @brief Validate peer
     * @param peerId Peer ID
     * @param userId User ID from USB key
     * @return true if peer is valid, false otherwise
     */
    bool validatePeer(const QString& peerId, const QString& userId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when peer is discovered
     */
    void peerDiscovered(const QString& peerId, const QHostAddress& address);

    /**
     * @brief Emitted when peer is lost
     */
    void peerLost(const QString& peerId);

private Q_SLOTS:
    void onDiscoveryTimeout();

private:
    QString m_networkSegment;
    quint16 m_discoveryPort;
    QTimer* m_discoveryTimer;
    QList<QString> m_discoveredPeers;
    QMap<QString, QHostAddress> m_peerAddresses;
    
    void broadcastDiscovery();
    void processDiscoveryResponse(const QByteArray& data, const QHostAddress& sender);
};

#endif // PEER_DISCOVERY_H

