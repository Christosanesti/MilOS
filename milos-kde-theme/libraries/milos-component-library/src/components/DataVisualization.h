#ifndef DATAVISUALIZATION_H
#define DATAVISUALIZATION_H

#include <QObject>
#include <QQuickItem>
#include <QVariantList>

class DataVisualization : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QVariantList data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(QVariantList networkNodes READ networkNodes WRITE setNetworkNodes NOTIFY networkNodesChanged)
    Q_PROPERTY(QVariantList networkConnections READ networkConnections WRITE setNetworkConnections NOTIFY networkConnectionsChanged)

public:
    explicit DataVisualization(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QVariantList data() const { return m_data; }
    void setData(const QVariantList &data);
    
    QVariantList networkNodes() const { return m_networkNodes; }
    void setNetworkNodes(const QVariantList &nodes);
    
    QVariantList networkConnections() const { return m_networkConnections; }
    void setNetworkConnections(const QVariantList &connections);

signals:
    void variantChanged();
    void stateChanged();
    void dataChanged();
    void networkNodesChanged();
    void networkConnectionsChanged();
    void pointSelected(int index);

private:
    QString m_variant;
    QString m_state;
    QVariantList m_data;
    QVariantList m_networkNodes;
    QVariantList m_networkConnections;
};

#endif // DATAVISUALIZATION_H

