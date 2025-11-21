#ifndef THREAT_ANALYZER_H
#define THREAT_ANALYZER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>

/**
 * @brief Threat Analysis Result
 */
struct ThreatAnalysisResult {
    QString id;
    QString threatType;
    QString severity;  // "low", "medium", "high", "critical"
    QString description;
    QVariantMap indicators;
    QDateTime detectedAt;
};

/**
 * @brief Threat Analyzer
 * 
 * Analyzes threats from honeypot interactions.
 */
class ThreatAnalyzer : public QObject {
    Q_OBJECT

public:
    explicit ThreatAnalyzer(QObject* parent = nullptr);
    ~ThreatAnalyzer();

    /**
     * @brief Initialize threat analyzer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Analyze interaction data
     * @param interactionData Interaction data from honeypot
     * @return Threat analysis result
     */
    ThreatAnalysisResult analyzeInteraction(const QVariantMap& interactionData);

    /**
     * @brief Get threat statistics
     * @return Threat statistics
     */
    QVariantMap getThreatStatistics() const;

Q_SIGNALS:
    /**
     * @brief Emitted when threat is detected
     */
    void threatDetected(const ThreatAnalysisResult& result);

private:
    QList<ThreatAnalysisResult> m_detectedThreats;
    
    QString determineThreatType(const QVariantMap& interactionData) const;
    QString determineSeverity(const QVariantMap& interactionData) const;
    QString generateThreatId() const;
};

Q_DECLARE_METATYPE(ThreatAnalysisResult)

#endif // THREAT_ANALYZER_H

