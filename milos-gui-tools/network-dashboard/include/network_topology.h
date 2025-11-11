#ifndef NETWORK_TOPOLOGY_H
#define NETWORK_TOPOLOGY_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include <map>
#include <cstdint>

/**
 * @brief Network Node
 */
struct NetworkNode {
    QString id;
    QString name;
    QString type;  // "device", "router", "switch", "server", "client"
    QString address;
    double x;
    double y;
    QString security_zone;
    QJsonObject metadata;
};

/**
 * @brief Network Edge
 */
struct NetworkEdge {
    QString id;
    QString source_id;
    QString destination_id;
    QString protocol;
    uint64_t packet_count;
    uint64_t byte_count;
    bool has_threat;
    QJsonObject metadata;
};

/**
 * @brief Security Zone
 */
struct SecurityZone {
    QString id;
    QString name;
    QString description;
    QString color;
    QJsonArray node_ids;
    QJsonObject metadata;
};

/**
 * @brief Network Topology Manager
 * 
 * Manages network topology: nodes, edges, security zones.
 */
class NetworkTopologyManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList nodes READ nodes NOTIFY topologyUpdated)
    Q_PROPERTY(QVariantList edges READ edges NOTIFY topologyUpdated)
    Q_PROPERTY(QVariantList securityZones READ securityZones NOTIFY topologyUpdated)

public:
    explicit NetworkTopologyManager(QObject* parent = nullptr);
    ~NetworkTopologyManager();

    /**
     * @brief Initialize topology manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Discover network topology from packet data
     * @param packetData Packet data to analyze
     */
    void discoverTopology(const PacketData& packet);

    /**
     * @brief Get network topology as JSON
     * @return Network topology JSON
     */
    Q_INVOKABLE QJsonObject getNetworkTopology() const;

    // Property getters
    QVariantList nodes() const;
    QVariantList edges() const;
    QVariantList securityZones() const;

    /**
     * @brief Add network node
     * @param node Node to add
     * @return true if add successful, false otherwise
     */
    Q_INVOKABLE bool addNode(const QJsonObject& node);

    /**
     * @brief Remove network node
     * @param nodeId Node ID to remove
     * @return true if remove successful, false otherwise
     */
    Q_INVOKABLE bool removeNode(const QString& nodeId);

    /**
     * @brief Update network node
     * @param nodeId Node ID to update
     * @param node Updated node data
     * @return true if update successful, false otherwise
     */
    Q_INVOKABLE bool updateNode(const QString& nodeId, const QJsonObject& node);

    /**
     * @brief Add security zone
     * @param zone Zone to add
     * @return true if add successful, false otherwise
     */
    Q_INVOKABLE bool addSecurityZone(const QJsonObject& zone);

    /**
     * @brief Remove security zone
     * @param zoneId Zone ID to remove
     * @return true if remove successful, false otherwise
     */
    Q_INVOKABLE bool removeSecurityZone(const QString& zoneId);

Q_SIGNALS:
    /**
     * @brief Emitted when topology is updated
     */
    void topologyUpdated();

    /**
     * @brief Emitted when node is selected
     */
    void nodeSelected(const QString& nodeId);

private:
    std::map<QString, NetworkNode> m_nodes;
    std::map<QString, NetworkEdge> m_edges;
    std::map<QString, SecurityZone> m_securityZones;

    /**
     * @brief Find or create node for address
     */
    QString findOrCreateNode(const QString& address, const QString& protocol);

    /**
     * @brief Find or create edge between nodes
     */
    QString findOrCreateEdge(const QString& sourceId, const QString& destId, const QString& protocol);
};

#endif // NETWORK_TOPOLOGY_H

