#ifndef TOPOLOGY_DISPLAY_H
#define TOPOLOGY_DISPLAY_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPointF>
#include <QVariantMap>
#include "segment_config.h"

/**
 * @brief Network Topology Node
 */
struct TopologyNode {
    QString nodeId;
    QString segmentId;
    QString label;
    QPointF position;
    QString nodeType;  // "segment", "router", "host", etc.
    QMap<QString, QVariant> properties;
};

/**
 * @brief Network Topology Edge
 */
struct TopologyEdge {
    QString edgeId;
    QString fromNodeId;
    QString toNodeId;
    QString label;
    QString edgeType;  // "connection", "isolation", etc.
    QMap<QString, QVariant> properties;
};

/**
 * @brief Topology Display Manager
 * 
 * Manages network topology visualization.
 */
class TopologyDisplay : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<TopologyNode> nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(QList<TopologyEdge> edges READ edges NOTIFY edgesChanged)

public:
    explicit TopologyDisplay(QObject* parent = nullptr);
    ~TopologyDisplay();

    /**
     * @brief Initialize topology display
     * @return true if initialization successful, false otherwise
     */
    Q_INVOKABLE bool initialize();

    /**
     * @brief Update topology from segments
     * @param segments List of segment configurations
     */
    Q_INVOKABLE void updateTopology(const QList<SegmentConfig>& segments);

    /**
     * @brief Get nodes
     */
    QList<TopologyNode> nodes() const { return m_nodes; }

    /**
     * @brief Get edges
     */
    QList<TopologyEdge> edges() const { return m_edges; }

    /**
     * @brief Get node by ID
     * @param nodeId Node ID
     * @return Node if found, empty node otherwise
     */
    Q_INVOKABLE QVariantMap getNode(const QString& nodeId) const;

    /**
     * @brief Get edge by ID
     * @param edgeId Edge ID
     * @return Edge if found, empty edge otherwise
     */
    Q_INVOKABLE QVariantMap getEdge(const QString& edgeId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when nodes change
     */
    void nodesChanged();

    /**
     * @brief Emitted when edges change
     */
    void edgesChanged();

private:
    bool m_initialized;
    QList<TopologyNode> m_nodes;
    QList<TopologyEdge> m_edges;

    /**
     * @brief Create node from segment
     */
    TopologyNode createNodeFromSegment(const SegmentConfig& segment, const QPointF& position);

    /**
     * @brief Create edges between segments
     */
    void createEdges(const QList<SegmentConfig>& segments);
};

#endif // TOPOLOGY_DISPLAY_H

