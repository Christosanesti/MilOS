#include "network_topology.h"
#include "packet_capture.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QHash>
#include <iostream>

NetworkTopologyManager::NetworkTopologyManager(QObject* parent)
    : QObject(parent)
{
}

NetworkTopologyManager::~NetworkTopologyManager() {
}

bool NetworkTopologyManager::initialize() {
    // Create default security zones
    SecurityZone zone1;
    zone1.id = "zone_internal";
    zone1.name = "Internal Network";
    zone1.description = "Internal trusted network";
    zone1.color = "#00ff00";
    m_securityZones[zone1.id] = zone1;

    SecurityZone zone2;
    zone2.id = "zone_dmz";
    zone2.name = "DMZ";
    zone2.description = "Demilitarized zone";
    zone2.color = "#ffff00";
    m_securityZones[zone2.id] = zone2;

    SecurityZone zone3;
    zone3.id = "zone_external";
    zone3.name = "External Network";
    zone3.description = "External untrusted network";
    zone3.color = "#ff0000";
    m_securityZones[zone3.id] = zone3;

    return true;
}

void NetworkTopologyManager::discoverTopology(const PacketData& packet) {
    // Find or create source node
    QString sourceId = findOrCreateNode(
        QString::fromStdString(packet.source_address),
        QString::fromStdString(packet.protocol)
    );

    // Find or create destination node
    QString destId = findOrCreateNode(
        QString::fromStdString(packet.destination_address),
        QString::fromStdString(packet.protocol)
    );

    // Find or create edge
    QString edgeId = findOrCreateEdge(sourceId, destId, QString::fromStdString(packet.protocol));

    // Update edge statistics
    if (m_edges.find(edgeId) != m_edges.end()) {
        NetworkEdge& edge = m_edges[edgeId];
        edge.packet_count++;
        edge.byte_count += packet.length;
    }

    emit topologyUpdated();
}

QVariantList NetworkTopologyManager::nodes() const {
    QVariantList nodeList;
    for (const auto& pair : m_nodes) {
        QJsonObject nodeObj;
        nodeObj["id"] = pair.second.id;
        nodeObj["name"] = pair.second.name;
        nodeObj["type"] = pair.second.type;
        nodeObj["address"] = pair.second.address;
        nodeObj["x"] = pair.second.x;
        nodeObj["y"] = pair.second.y;
        nodeObj["security_zone"] = pair.second.security_zone;
        nodeObj["metadata"] = pair.second.metadata;
        nodeList.append(QJsonValue(nodeObj).toVariant());
    }
    return nodeList;
}

QVariantList NetworkTopologyManager::edges() const {
    QVariantList edgeList;
    for (const auto& pair : m_edges) {
        QJsonObject edgeObj;
        edgeObj["id"] = pair.second.id;
        edgeObj["source_id"] = pair.second.source_id;
        edgeObj["destination_id"] = pair.second.destination_id;
        edgeObj["protocol"] = pair.second.protocol;
        edgeObj["packet_count"] = static_cast<qint64>(pair.second.packet_count);
        edgeObj["byte_count"] = static_cast<qint64>(pair.second.byte_count);
        edgeObj["has_threat"] = pair.second.has_threat;
        edgeObj["metadata"] = pair.second.metadata;
        edgeList.append(QJsonValue(edgeObj).toVariant());
    }
    return edgeList;
}

QVariantList NetworkTopologyManager::securityZones() const {
    QVariantList zoneList;
    for (const auto& pair : m_securityZones) {
        QJsonObject zoneObj;
        zoneObj["id"] = pair.second.id;
        zoneObj["name"] = pair.second.name;
        zoneObj["description"] = pair.second.description;
        zoneObj["color"] = pair.second.color;
        zoneObj["node_ids"] = pair.second.node_ids;
        zoneObj["metadata"] = pair.second.metadata;
        zoneList.append(QJsonValue(zoneObj).toVariant());
    }
    return zoneList;
}

QJsonObject NetworkTopologyManager::getNetworkTopology() const {
    QJsonObject topology;
    topology["nodes"] = QJsonValue::fromVariant(nodes());
    topology["edges"] = QJsonValue::fromVariant(edges());
    topology["security_zones"] = QJsonValue::fromVariant(securityZones());
    return topology;
}

bool NetworkTopologyManager::addNode(const QJsonObject& node) {
    NetworkNode newNode;
    newNode.id = node["id"].toString();
    newNode.name = node["name"].toString();
    newNode.type = node["type"].toString();
    newNode.address = node["address"].toString();
    newNode.x = node["x"].toDouble();
    newNode.y = node["y"].toDouble();
    newNode.security_zone = node["security_zone"].toString();
    newNode.metadata = node["metadata"].toObject();

    if (newNode.id.isEmpty()) {
        return false;
    }

    m_nodes[newNode.id] = newNode;
    emit topologyUpdated();
    return true;
}

bool NetworkTopologyManager::removeNode(const QString& nodeId) {
    auto it = m_nodes.find(nodeId);
    if (it != m_nodes.end()) {
        m_nodes.erase(it);
        
        // Remove associated edges
        auto edgeIt = m_edges.begin();
        while (edgeIt != m_edges.end()) {
            if (edgeIt->second.source_id == nodeId || edgeIt->second.destination_id == nodeId) {
                edgeIt = m_edges.erase(edgeIt);
            } else {
                ++edgeIt;
            }
        }
        
        emit topologyUpdated();
        return true;
    }
    return false;
}

bool NetworkTopologyManager::updateNode(const QString& nodeId, const QJsonObject& node) {
    auto it = m_nodes.find(nodeId);
    if (it != m_nodes.end()) {
        NetworkNode& existingNode = it->second;
        if (node.contains("name")) existingNode.name = node["name"].toString();
        if (node.contains("type")) existingNode.type = node["type"].toString();
        if (node.contains("x")) existingNode.x = node["x"].toDouble();
        if (node.contains("y")) existingNode.y = node["y"].toDouble();
        if (node.contains("security_zone")) existingNode.security_zone = node["security_zone"].toString();
        if (node.contains("metadata")) existingNode.metadata = node["metadata"].toObject();
        
        emit topologyUpdated();
        return true;
    }
    return false;
}

bool NetworkTopologyManager::addSecurityZone(const QJsonObject& zone) {
    SecurityZone newZone;
    newZone.id = zone["id"].toString();
    newZone.name = zone["name"].toString();
    newZone.description = zone["description"].toString();
    newZone.color = zone["color"].toString();
    newZone.node_ids = zone["node_ids"].toArray();
    newZone.metadata = zone["metadata"].toObject();

    if (newZone.id.isEmpty()) {
        return false;
    }

    m_securityZones[newZone.id] = newZone;
    emit topologyUpdated();
    return true;
}

bool NetworkTopologyManager::removeSecurityZone(const QString& zoneId) {
    auto it = m_securityZones.find(zoneId);
    if (it != m_securityZones.end()) {
        m_securityZones.erase(it);
        emit topologyUpdated();
        return true;
    }
    return false;
}

QString NetworkTopologyManager::findOrCreateNode(const QString& address, const QString& protocol) {
    // Search for existing node with this address
    for (const auto& pair : m_nodes) {
        if (pair.second.address == address) {
            return pair.second.id;
        }
    }

    // Create new node
    NetworkNode node;
    node.id = "node_" + address + "_" + protocol;
    node.name = address;
    node.type = "device";
    node.address = address;
    node.x = (m_nodes.size() % 10) * 100.0;  // Simple layout
    node.y = (m_nodes.size() / 10) * 100.0;
    node.security_zone = "zone_internal";  // Default zone

    m_nodes[node.id] = node;
    return node.id;
}

QString NetworkTopologyManager::findOrCreateEdge(const QString& sourceId, const QString& destId, const QString& protocol) {
    // Search for existing edge
    for (const auto& pair : m_edges) {
        if (pair.second.source_id == sourceId && 
            pair.second.destination_id == destId && 
            pair.second.protocol == protocol) {
            return pair.second.id;
        }
    }

    // Create new edge
    NetworkEdge edge;
    edge.id = "edge_" + sourceId + "_" + destId + "_" + protocol;
    edge.source_id = sourceId;
    edge.destination_id = destId;
    edge.protocol = protocol;
    edge.packet_count = 0;
    edge.byte_count = 0;
    edge.has_threat = false;

    m_edges[edge.id] = edge;
    return edge.id;
}

