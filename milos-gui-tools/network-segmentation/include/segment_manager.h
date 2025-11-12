#ifndef SEGMENT_MANAGER_H
#define SEGMENT_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVariantMap>
#include "segment_config.h"

/**
 * @brief Segment Manager
 * 
 * Manages network segments and their organization.
 */
class SegmentManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<SegmentConfig> segments READ segments NOTIFY segmentsChanged)

public:
    explicit SegmentManager(QObject* parent = nullptr);
    ~SegmentManager();

    /**
     * @brief Initialize segment manager
     * @param dbPath Database path for persistence
     * @return true if initialization successful, false otherwise
     */
    Q_INVOKABLE bool initialize(const QString& dbPath = QString());

    /**
     * @brief Get all segments
     */
    QList<SegmentConfig> segments() const { return m_segments; }

    /**
     * @brief Create segment
     * @param name Segment name
     * @param networkAddress Network address (CIDR)
     * @param description Segment description
     * @return Segment ID if successful, empty string otherwise
     */
    Q_INVOKABLE QString createSegment(const QString& name, const QString& networkAddress, const QString& description = QString());

    /**
     * @brief Update segment
     * @param segmentId Segment ID
     * @param name Segment name
     * @param networkAddress Network address
     * @param description Segment description
     * @return true if update successful, false otherwise
     */
    Q_INVOKABLE bool updateSegment(const QString& segmentId, const QString& name, const QString& networkAddress, const QString& description = QString());

    /**
     * @brief Delete segment
     * @param segmentId Segment ID
     * @return true if deletion successful, false otherwise
     */
    Q_INVOKABLE bool deleteSegment(const QString& segmentId);

    /**
     * @brief Get segment
     * @param segmentId Segment ID
     * @return Segment configuration
     */
    Q_INVOKABLE QVariantMap getSegment(const QString& segmentId) const;

    /**
     * @brief Get segments by network
     * @param networkAddress Network address
     * @return List of segment IDs
     */
    Q_INVOKABLE QStringList getSegmentsByNetwork(const QString& networkAddress) const;

Q_SIGNALS:
    /**
     * @brief Emitted when segments change
     */
    void segmentsChanged();

    /**
     * @brief Emitted when segment is created
     */
    void segmentCreated(const QString& segmentId);

    /**
     * @brief Emitted when segment is updated
     */
    void segmentUpdated(const QString& segmentId);

    /**
     * @brief Emitted when segment is deleted
     */
    void segmentDeleted(const QString& segmentId);

private slots:
    /**
     * @brief Handle segment created
     */
    void onSegmentCreated(const QString& segmentId);

    /**
     * @brief Handle segment updated
     */
    void onSegmentUpdated(const QString& segmentId);

    /**
     * @brief Handle segment deleted
     */
    void onSegmentDeleted(const QString& segmentId);

private:
    bool m_initialized;
    SegmentConfigManager* m_configManager;
    QList<SegmentConfig> m_segments;

    /**
     * @brief Update segments list
     */
    void updateSegmentsList();
};

#endif // SEGMENT_MANAGER_H

