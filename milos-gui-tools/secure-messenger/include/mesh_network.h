#ifndef MESH_NETWORK_H
#define MESH_NETWORK_H

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QList>
#include <QMap>
#include <QVariantMap>

/**
 * @brief Peer Connection Status
 */
enum class PeerStatus {
    Connected,
    Disconnected,
    Connecting,
    Error
};

/**
 * @brief Peer Information
 */
struct PeerInfo {
    QString peerId;
    QHostAddress address;
    quint16 port;
    PeerStatus status;
    QString userId;
    QDateTime lastSeen;
    int hopCount;
    QList<QString> route;
};

/**
 * @brief Mesh Network
 * 
 * Manages mesh network topology and peer-to-peer routing.
 */
class MeshNetwork : public QObject {
    Q_OBJECT

public:
    explicit MeshNetwork(QObject* parent = nullptr);
    ~MeshNetwork();

    /**
     * @brief Initialize mesh network
     * @param localAddress Local network address
     * @param port Port for mesh communication
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QHostAddress& localAddress, quint16 port);

    /**
     * @brief Add peer to mesh
     * @param peerInfo Peer information
     * @return true if add successful, false otherwise
     */
    bool addPeer(const PeerInfo& peerInfo);

    /**
     * @brief Remove peer from mesh
     * @param peerId Peer ID
     * @return true if remove successful, false otherwise
     */
    bool removePeer(const QString& peerId);

    /**
     * @brief Get peer information
     * @param peerId Peer ID
     * @return Peer information or empty if not found
     */
    PeerInfo getPeerInfo(const QString& peerId) const;

    /**
     * @brief Get all peers
     * @return List of peer information
     */
    QList<PeerInfo> getAllPeers() const;

    /**
     * @brief Find route to peer
     * @param targetPeerId Target peer ID
     * @return Route (list of peer IDs) or empty if no route
     */
    QList<QString> findRoute(const QString& targetPeerId) const;

    /**
     * @brief Send message via mesh
     * @param targetPeerId Target peer ID
     * @param data Message data
     * @return true if send successful, false otherwise
     */
    bool sendMessage(const QString& targetPeerId, const QByteArray& data);

Q_SIGNALS:
    /**
     * @brief Emitted when peer is added
     */
    void peerAdded(const QString& peerId);

    /**
     * @brief Emitted when peer is removed
     */
    void peerRemoved(const QString& peerId);

    /**
     * @brief Emitted when message is received
     */
    void messageReceived(const QString& sourcePeerId, const QByteArray& data);

private:
    QMap<QString, PeerInfo> m_peers;
    QHostAddress m_localAddress;
    quint16 m_port;
    QString m_localPeerId;
    
    QString generatePeerId() const;
    void updateRoutingTable();
};

#endif // MESH_NETWORK_H

