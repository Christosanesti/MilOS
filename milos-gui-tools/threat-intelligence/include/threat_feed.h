#ifndef THREAT_FEED_H
#define THREAT_FEED_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>
#include <QUrl>
#include <QTimer>

/**
 * @brief Threat Feed Type
 */
enum class ThreatFeedType {
    Public,
    Military,
    Custom,
    Internal
};

/**
 * @brief Threat Indicator
 */
struct ThreatIndicator {
    QString id;
    QString type;  // "ip", "domain", "hash", "url", etc.
    QString value;
    QString severity;  // "critical", "high", "medium", "low"
    QString description;
    QDateTime firstSeen;
    QDateTime lastSeen;
    QStringList tags;
    QVariantMap metadata;
};

/**
 * @brief Threat Feed
 * 
 * Represents a threat intelligence feed source.
 */
class ThreatFeed : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString feedId READ feedId CONSTANT)
    Q_PROPERTY(QString feedName READ feedName CONSTANT)
    Q_PROPERTY(bool isEnabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)

public:
    explicit ThreatFeed(QObject* parent = nullptr);
    ~ThreatFeed();

    /**
     * @brief Initialize threat feed
     * @param feedId Feed identifier
     * @param feedName Feed name
     * @param feedType Feed type
     * @param feedUrl Feed URL or source
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& feedId, const QString& feedName,
                    ThreatFeedType feedType, const QUrl& feedUrl);

    /**
     * @brief Get feed ID
     * @return Feed ID
     */
    QString feedId() const { return m_feedId; }

    /**
     * @brief Get feed name
     * @return Feed name
     */
    QString feedName() const { return m_feedName; }

    /**
     * @brief Check if feed is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return m_enabled; }

    /**
     * @brief Set feed enabled state
     * @param enabled Enabled state
     */
    void setEnabled(bool enabled);

    /**
     * @brief Get update interval (seconds)
     * @return Update interval
     */
    int updateInterval() const { return m_updateInterval; }

    /**
     * @brief Set update interval
     * @param interval Update interval in seconds
     */
    void setUpdateInterval(int interval);

    /**
     * @brief Fetch threat indicators from feed
     * @return List of threat indicators
     */
    QList<ThreatIndicator> fetchIndicators();

    /**
     * @brief Get last update time
     * @return Last update time
     */
    QDateTime lastUpdateTime() const { return m_lastUpdateTime; }

    /**
     * @brief Get feed statistics
     * @return Statistics map
     */
    QVariantMap getStatistics() const;

Q_SIGNALS:
    /**
     * @brief Emitted when feed is enabled/disabled
     */
    void enabledChanged(bool enabled);

    /**
     * @brief Emitted when update interval changes
     */
    void updateIntervalChanged(int interval);

    /**
     * @brief Emitted when new indicators are fetched
     */
    void indicatorsUpdated(const QList<ThreatIndicator>& indicators);

private slots:
    void performUpdate();

private:
    QString m_feedId;
    QString m_feedName;
    ThreatFeedType m_feedType;
    QUrl m_feedUrl;
    bool m_enabled;
    int m_updateInterval;
    QDateTime m_lastUpdateTime;
    QTimer* m_updateTimer;
    QList<ThreatIndicator> m_cachedIndicators;
    
    QList<ThreatIndicator> parseFeedData(const QByteArray& data) const;
};

Q_DECLARE_METATYPE(ThreatFeedType)
Q_DECLARE_METATYPE(ThreatIndicator)

#endif // THREAT_FEED_H

