#ifndef ML_THREAT_DETECTOR_H
#define ML_THREAT_DETECTOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>
#include <QList>

/**
 * @brief ML Threat Type
 */
enum class MLThreatType {
    Intrusion,
    Malware,
    APT,
    DDoS,
    PortScan,
    Anomaly,
    Unknown
};

/**
 * @brief ML Threat Detection Result
 */
struct MLThreatResult {
    QString id;
    MLThreatType type;
    QString description;
    double confidence;  // 0.0 to 1.0
    QDateTime detectedAt;
    QVariantMap features;
    QVariantMap metadata;
};

/**
 * @brief ML Threat Detector
 * 
 * Machine learning-based threat detection engine.
 */
class MLThreatDetector : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isTraining READ isTraining NOTIFY trainingChanged)

public:
    explicit MLThreatDetector(QObject* parent = nullptr);
    ~MLThreatDetector();

    /**
     * @brief Initialize ML threat detector
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Train ML model
     * @param trainingData Training data
     * @return true if training successful, false otherwise
     */
    bool trainModel(const QList<QVariantMap>& trainingData);

    /**
     * @brief Detect threats in network data
     * @param networkData Network packet/flow data
     * @return List of detected threats
     */
    QList<MLThreatResult> detectThreats(const QVariantMap& networkData);

    /**
     * @brief Check if model is training
     * @return true if training, false otherwise
     */
    bool isTraining() const { return m_training; }

    /**
     * @brief Get model accuracy
     * @return Model accuracy (0.0 to 1.0)
     */
    double getModelAccuracy() const { return m_modelAccuracy; }

Q_SIGNALS:
    /**
     * @brief Emitted when threat is detected
     */
    void threatDetected(const MLThreatResult& threat);

    /**
     * @brief Emitted when training state changes
     */
    void trainingChanged(bool training);

private:
    bool m_training;
    double m_modelAccuracy;
    
    QVariantMap extractFeatures(const QVariantMap& networkData) const;
    MLThreatResult analyzeWithModel(const QVariantMap& features) const;
    QString generateThreatId() const;
};

Q_DECLARE_METATYPE(MLThreatType)
Q_DECLARE_METATYPE(MLThreatResult)

#endif // ML_THREAT_DETECTOR_H

