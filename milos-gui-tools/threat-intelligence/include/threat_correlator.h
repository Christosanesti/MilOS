#ifndef THREAT_CORRELATOR_H
#define THREAT_CORRELATOR_H

#include "threat_feed.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

/**
 * @brief Threat Correlation Result
 */
struct ThreatCorrelation {
    QString correlationId;
    QStringList relatedIndicators;
    QString severity;
    QString description;
    QDateTime detectedAt;
    QVariantMap analysis;
};

/**
 * @brief Threat Correlator
 * 
 * Correlates threat indicators from multiple feeds.
 */
class ThreatCorrelator : public QObject {
    Q_OBJECT

public:
    explicit ThreatCorrelator(QObject* parent = nullptr);
    ~ThreatCorrelator();

    /**
     * @brief Initialize threat correlator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add threat indicator
     * @param indicator Threat indicator
     * @return true if add successful, false otherwise
     */
    bool addIndicator(const ThreatIndicator& indicator);

    /**
     * @brief Correlate indicators
     * @return List of correlations
     */
    QList<ThreatCorrelation> correlateIndicators();

    /**
     * @brief Get correlations for indicator
     * @param indicatorId Indicator ID
     * @return List of correlations
     */
    QList<ThreatCorrelation> getCorrelations(const QString& indicatorId) const;

    /**
     * @brief Get threat analysis
     * @param indicatorId Indicator ID
     * @return Analysis data
     */
    QVariantMap analyzeThreat(const QString& indicatorId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when correlation is detected
     */
    void correlationDetected(const ThreatCorrelation& correlation);

private:
    QList<ThreatIndicator> m_indicators;
    QList<ThreatCorrelation> m_correlations;
    
    bool areIndicatorsRelated(const ThreatIndicator& a, const ThreatIndicator& b) const;
    QString generateCorrelationId() const;
};

Q_DECLARE_METATYPE(ThreatCorrelation)

#endif // THREAT_CORRELATOR_H

