#ifndef THREAT_DETECTOR_H
#define THREAT_DETECTOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Threat Type
 */
enum class ThreatType {
    Intrusion,
    Malware,
    APT,
    ZeroDay,
    InsiderThreat,
    FileIntegrity,
    Unknown
};

/**
 * @brief Threat Severity
 */
enum class ThreatSeverity {
    Critical,
    High,
    Medium,
    Low
};

/**
 * @brief Detected Threat
 */
struct DetectedThreat {
    QString id;
    ThreatType type;
    ThreatSeverity severity;
    QString description;
    QString source;
    QDateTime detectedAt;
    QVariantMap details;
    QStringList affectedResources;
    bool isContained;
};

/**
 * @brief Threat Detector
 * 
 * AI-powered threat detection engine.
 */
class ThreatDetector : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isMonitoring READ isMonitoring NOTIFY monitoringChanged)

public:
    explicit ThreatDetector(QObject* parent = nullptr);
    ~ThreatDetector();

    /**
     * @brief Initialize threat detector
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start monitoring
     * @return true if start successful, false otherwise
     */
    bool startMonitoring();

    /**
     * @brief Stop monitoring
     */
    void stopMonitoring();

    /**
     * @brief Check if monitoring
     * @return true if monitoring, false otherwise
     */
    bool isMonitoring() const { return m_monitoring; }

    /**
     * @brief Analyze data for threats
     * @param data Data to analyze
     * @param dataType Type of data ("network", "process", "file", etc.)
     * @return List of detected threats
     */
    QList<DetectedThreat> analyzeData(const QVariantMap& data, const QString& dataType);

    /**
     * @brief Get detected threats
     * @return List of detected threats
     */
    QList<DetectedThreat> getDetectedThreats() const;

    /**
     * @brief Get threat statistics
     * @return Statistics map
     */
    QVariantMap getStatistics() const;

Q_SIGNALS:
    /**
     * @brief Emitted when monitoring state changes
     */
    void monitoringChanged(bool monitoring);

    /**
     * @brief Emitted when threat is detected
     */
    void threatDetected(const DetectedThreat& threat);

private slots:
    void performMonitoring();

private:
    bool m_monitoring;
    QList<DetectedThreat> m_detectedThreats;
    QTimer* m_monitoringTimer;
    
    DetectedThreat analyzeNetworkData(const QVariantMap& data) const;
    DetectedThreat analyzeProcessData(const QVariantMap& data) const;
    DetectedThreat analyzeFileData(const QVariantMap& data) const;
    bool isAnomalous(const QVariantMap& data, const QString& dataType) const;
    QString generateThreatId() const;
};

Q_DECLARE_METATYPE(ThreatType)
Q_DECLARE_METATYPE(ThreatSeverity)
Q_DECLARE_METATYPE(DetectedThreat)

#endif // THREAT_DETECTOR_H

