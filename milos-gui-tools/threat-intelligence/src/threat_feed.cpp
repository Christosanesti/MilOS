#include "threat_feed.h"
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ThreatFeed::ThreatFeed(QObject* parent)
    : QObject(parent)
    , m_enabled(false)
    , m_updateInterval(3600)  // Default: 1 hour
    , m_updateTimer(new QTimer(this))
{
    connect(m_updateTimer, &QTimer::timeout, this, &ThreatFeed::performUpdate);
}

ThreatFeed::~ThreatFeed() {
}

bool ThreatFeed::initialize(const QString& feedId, const QString& feedName,
                            ThreatFeedType feedType, const QUrl& feedUrl) {
    m_feedId = feedId;
    m_feedName = feedName;
    m_feedType = feedType;
    m_feedUrl = feedUrl;
    
    return true;
}

void ThreatFeed::setEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        
        if (enabled) {
            m_updateTimer->start(m_updateInterval * 1000);
            performUpdate();  // Immediate update
        } else {
            m_updateTimer->stop();
        }
        
        emit enabledChanged(enabled);
    }
}

void ThreatFeed::setUpdateInterval(int interval) {
    if (m_updateInterval != interval && interval > 0) {
        m_updateInterval = interval;
        
        if (m_enabled) {
            m_updateTimer->setInterval(interval * 1000);
        }
        
        emit updateIntervalChanged(interval);
    }
}

QList<ThreatIndicator> ThreatFeed::fetchIndicators() {
    // In production, would fetch from actual feed URL
    // For now, simulate feed data
    
    QList<ThreatIndicator> indicators;
    
    // Simulate some threat indicators
    ThreatIndicator indicator1;
    indicator1.id = "threat-001";
    indicator1.type = "ip";
    indicator1.value = "192.168.1.100";
    indicator1.severity = "high";
    indicator1.description = "Malicious IP address";
    indicator1.firstSeen = QDateTime::currentDateTime().addDays(-5);
    indicator1.lastSeen = QDateTime::currentDateTime();
    indicator1.tags = QStringList() << "malware" << "botnet";
    indicators.append(indicator1);
    
    ThreatIndicator indicator2;
    indicator2.id = "threat-002";
    indicator2.type = "domain";
    indicator2.value = "malicious.example.com";
    indicator2.severity = "critical";
    indicator2.description = "Malicious domain";
    indicator2.firstSeen = QDateTime::currentDateTime().addDays(-2);
    indicator2.lastSeen = QDateTime::currentDateTime();
    indicator2.tags = QStringList() << "phishing" << "malware";
    indicators.append(indicator2);
    
    m_cachedIndicators = indicators;
    m_lastUpdateTime = QDateTime::currentDateTime();
    
    emit indicatorsUpdated(indicators);
    
    return indicators;
}

QVariantMap ThreatFeed::getStatistics() const {
    QVariantMap stats;
    stats["feed_id"] = m_feedId;
    stats["feed_name"] = m_feedName;
    stats["enabled"] = m_enabled;
    stats["update_interval"] = m_updateInterval;
    stats["last_update"] = m_lastUpdateTime.toString(Qt::ISODate);
    stats["indicator_count"] = m_cachedIndicators.size();
    
    int critical = 0, high = 0, medium = 0, low = 0;
    for (const ThreatIndicator& indicator : m_cachedIndicators) {
        if (indicator.severity == "critical") critical++;
        else if (indicator.severity == "high") high++;
        else if (indicator.severity == "medium") medium++;
        else low++;
    }
    
    stats["critical_count"] = critical;
    stats["high_count"] = high;
    stats["medium_count"] = medium;
    stats["low_count"] = low;
    
    return stats;
}

void ThreatFeed::performUpdate() {
    if (!m_enabled) {
        return;
    }
    
    fetchIndicators();
}

QList<ThreatIndicator> ThreatFeed::parseFeedData(const QByteArray& data) const {
    QList<ThreatIndicator> indicators;
    
    // In production, would parse actual feed format (JSON, CSV, STIX, etc.)
    // For now, return empty list
    Q_UNUSED(data);
    
    return indicators;
}

