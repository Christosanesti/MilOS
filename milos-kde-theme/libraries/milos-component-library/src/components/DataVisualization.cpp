#include "DataVisualization.h"

DataVisualization::DataVisualization(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("line")
    , m_state("default")
{
}

void DataVisualization::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void DataVisualization::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void DataVisualization::setData(const QVariantList &data)
{
    if (m_data != data) {
        m_data = data;
        emit dataChanged();
    }
}

void DataVisualization::setNetworkNodes(const QVariantList &nodes)
{
    if (m_networkNodes != nodes) {
        m_networkNodes = nodes;
        emit networkNodesChanged();
    }
}

void DataVisualization::setNetworkConnections(const QVariantList &connections)
{
    if (m_networkConnections != connections) {
        m_networkConnections = connections;
        emit networkConnectionsChanged();
    }
}

