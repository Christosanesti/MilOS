#ifndef SEGMENT_CONFIG_H
#define SEGMENT_CONFIG_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonObject>

/**
 * @brief Network Segment Configuration
 */
struct SegmentConfig {
    QString segmentId;
    QString name;
    QString description;
    QString networkAddress;  // CIDR notation (e.g., "192.168.1.0/24")
    QStringList allowedIPs;
    QStringList blockedIPs;
    QStringList allowedPorts;
    QStringList blockedPorts;
    QStringList allowedProtocols;  // TCP, UDP, ICMP, etc.
    bool isIsolated;  // Isolated from other segments
    int priority;  // Priority for rule ordering
    QMap<QString, QVariant> customSettings;

    SegmentConfig() : isIsolated(false), priority(0) {}
    
    bool operator==(const SegmentConfig& other) const {
        return segmentId == other.segmentId;
    }
    
    QJsonObject toJson() const;
    static SegmentConfig fromJson(const QJsonObject& json);
};

Q_DECLARE_METATYPE(SegmentConfig)
Q_DECLARE_METATYPE(QList<SegmentConfig>)

/**
 * @brief Segment Configuration Manager
 * 
 * Manages network segment configurations.
 */
class SegmentConfigManager : public QObject {
    Q_OBJECT

public:
    explicit SegmentConfigManager(QObject* parent = nullptr);
    ~SegmentConfigManager();

    /**
     * @brief Initialize configuration manager
     * @param dbPath Database path for persistence
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Create segment configuration
     * @param config Segment configuration
     * @return Segment ID if successful, empty string otherwise
     */
    QString createSegment(const SegmentConfig& config);

    /**
     * @brief Update segment configuration
     * @param segmentId Segment ID
     * @param config Updated configuration
     * @return true if update successful, false otherwise
     */
    bool updateSegment(const QString& segmentId, const SegmentConfig& config);

    /**
     * @brief Delete segment
     * @param segmentId Segment ID
     * @return true if deletion successful, false otherwise
     */
    bool deleteSegment(const QString& segmentId);

    /**
     * @brief Get segment configuration
     * @param segmentId Segment ID
     * @return Segment configuration
     */
    SegmentConfig getSegment(const QString& segmentId) const;

    /**
     * @brief List all segments
     * @return List of segment configurations
     */
    QList<SegmentConfig> listSegments() const;

    /**
     * @brief Get segments by network address
     * @param networkAddress Network address (CIDR)
     * @return List of matching segments
     */
    QList<SegmentConfig> getSegmentsByNetwork(const QString& networkAddress) const;

Q_SIGNALS:
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

private:
    bool m_initialized;
    QString m_dbPath;
    QMap<QString, SegmentConfig> m_segments;

    /**
     * @brief Load segments from database
     */
    bool loadSegments();

    /**
     * @brief Save segment to database
     */
    bool saveSegment(const SegmentConfig& config);

    /**
     * @brief Remove segment from database
     */
    bool removeSegment(const QString& segmentId);
};

#endif // SEGMENT_CONFIG_H

