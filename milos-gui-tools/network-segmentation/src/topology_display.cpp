#include "topology_display.h"
#include "segment_config.h"
#include <QJsonObject>
#include <QVariantMap>
#include <cmath>
#include <iostream>

TopologyDisplay::TopologyDisplay(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

TopologyDisplay::~TopologyDisplay() {
}

bool TopologyDisplay::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

void TopologyDisplay::updateTopology(const QList<SegmentConfig>& segments) {
    m_nodes.clear();
    m_edges.clear();

    // Create nodes for each segment
    int nodeCount = segments.size();
    double angleStep = 2.0 * M_PI / nodeCount;
    double radius = 200.0;

    for (int i = 0; i < nodeCount; i++) {
        double angle = i * angleStep;
        QPointF position(radius * cos(angle), radius * sin(angle));
        
        TopologyNode node = createNodeFromSegment(segments[i], position);
        m_nodes.append(node);
    }

    // Create edges
    createEdges(segments);

    emit nodesChanged();
    emit edgesChanged();
}

TopologyNode TopologyDisplay::createNodeFromSegment(const SegmentConfig& segment, const QPointF& position) {
    TopologyNode node;
    node.nodeId = segment.segmentId;
    node.segmentId = segment.segmentId;
    node.label = segment.name;
    node.position = position;
    node.nodeType = "segment";
    
    node.properties["networkAddress"] = segment.networkAddress;
    node.properties["isIsolated"] = segment.isIsolated;
    node.properties["priority"] = segment.priority;
    
    return node;
}

void TopologyDisplay::createEdges(const QList<SegmentConfig>& segments) {
    // Create edges between non-isolated segments
    for (int i = 0; i < segments.size(); i++) {
        const SegmentConfig& segment1 = segments[i];
        
        if (segment1.isIsolated) {
            continue;  // Isolated segments don't have connections
        }

        for (int j = i + 1; j < segments.size(); j++) {
            const SegmentConfig& segment2 = segments[j];
            
            if (segment2.isIsolated) {
                continue;
            }

            // Create connection edge
            TopologyEdge edge;
            edge.edgeId = segment1.segmentId + "_" + segment2.segmentId;
            edge.fromNodeId = segment1.segmentId;
            edge.toNodeId = segment2.segmentId;
            edge.label = "Connection";
            edge.edgeType = "connection";
            
            m_edges.append(edge);
        }
    }
}

QVariantMap TopologyDisplay::getNode(const QString& nodeId) const {
    for (const TopologyNode& node : m_nodes) {
        if (node.nodeId == nodeId) {
            QVariantMap map;
            map["nodeId"] = node.nodeId;
            map["segmentId"] = node.segmentId;
            map["label"] = node.label;
            map["x"] = node.position.x();
            map["y"] = node.position.y();
            map["nodeType"] = node.nodeType;
            return map;
        }
    }
    return QVariantMap();
}

QVariantMap TopologyDisplay::getEdge(const QString& edgeId) const {
    for (const TopologyEdge& edge : m_edges) {
        if (edge.edgeId == edgeId) {
            QVariantMap map;
            map["edgeId"] = edge.edgeId;
            map["fromNodeId"] = edge.fromNodeId;
            map["toNodeId"] = edge.toNodeId;
            map["label"] = edge.label;
            map["edgeType"] = edge.edgeType;
            return map;
        }
    }
    return QVariantMap();
}

