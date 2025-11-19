#ifndef NETWORK_ISOLATION_H
#define NETWORK_ISOLATION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "segment_config.h"

/**
 * @brief Network Isolation Manager
 * 
 * Manages kernel-level network isolation for network segments.
 */
class NetworkIsolation : public QObject {
    Q_OBJECT

public:
    explicit NetworkIsolation(QObject* parent = nullptr);
    ~NetworkIsolation();

    /**
     * @brief Initialize network isolation
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Isolate network segment
     * @param segment Segment configuration
     * @return true if isolation successful, false otherwise
     */
    bool isolateSegment(const SegmentConfig& segment);

    /**
     * @brief Remove isolation for segment
     * @param segmentId Segment ID
     * @return true if removal successful, false otherwise
     */
    bool removeIsolation(const QString& segmentId);

    /**
     * @brief Verify segment isolation
     * @param segmentId Segment ID
     * @return true if segment is isolated, false otherwise
     */
    bool verifyIsolation(const QString& segmentId) const;

    /**
     * @brief Get isolated segments
     * @return List of isolated segment IDs
     */
    QStringList getIsolatedSegments() const;

Q_SIGNALS:
    /**
     * @brief Emitted when segment is isolated
     */
    void segmentIsolated(const QString& segmentId);

    /**
     * @brief Emitted when segment isolation is removed
     */
    void isolationRemoved(const QString& segmentId);

private:
    bool m_initialized;
    QStringList m_isolatedSegments;

    /**
     * @brief Create network namespace for segment
     */
    bool createNetworkNamespace(const QString& segmentId, const QString& networkAddress);

    /**
     * @brief Remove network namespace for segment
     */
    bool removeNetworkNamespace(const QString& segmentId);

    /**
     * @brief Configure network namespace isolation
     */
    bool configureNamespaceIsolation(const QString& segmentId, const SegmentConfig& segment);
};

#endif // NETWORK_ISOLATION_H

