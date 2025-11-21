#ifndef SPECTRUM_ANALYZER_H
#define SPECTRUM_ANALYZER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QTimer>

/**
 * @brief Frequency Data Point
 */
struct FrequencyDataPoint {
    double frequency;  // MHz
    double power;      // dBm
    QDateTime timestamp;
};

/**
 * @brief Spectrum Analyzer
 * 
 * Analyzes RF spectrum and visualizes frequency usage.
 */
class SpectrumAnalyzer : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isAnalyzing READ isAnalyzing NOTIFY analyzingChanged)

public:
    explicit SpectrumAnalyzer(QObject* parent = nullptr);
    ~SpectrumAnalyzer();

    /**
     * @brief Initialize spectrum analyzer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start spectrum analysis
     * @param frequencyStart Start frequency (MHz)
     * @param frequencyEnd End frequency (MHz)
     * @return true if start successful, false otherwise
     */
    bool startAnalysis(double frequencyStart, double frequencyEnd);

    /**
     * @brief Stop spectrum analysis
     */
    void stopAnalysis();

    /**
     * @brief Get spectrum data
     * @return List of frequency data points
     */
    QList<FrequencyDataPoint> getSpectrumData() const;

    /**
     * @brief Check if analyzing
     * @return true if analyzing, false otherwise
     */
    bool isAnalyzing() const { return m_analyzing; }

    /**
     * @brief Get frequency usage statistics
     * @return Statistics map
     */
    QVariantMap getFrequencyStatistics() const;

Q_SIGNALS:
    /**
     * @brief Emitted when analysis state changes
     */
    void analyzingChanged(bool analyzing);

    /**
     * @brief Emitted when spectrum data is updated
     */
    void spectrumDataUpdated(const QList<FrequencyDataPoint>& data);

private slots:
    void performAnalysis();

private:
    bool m_analyzing;
    double m_frequencyStart;
    double m_frequencyEnd;
    QList<FrequencyDataPoint> m_spectrumData;
    QTimer* m_analysisTimer;
    
    FrequencyDataPoint analyzeFrequency(double frequency) const;
};

Q_DECLARE_METATYPE(FrequencyDataPoint)

#endif // SPECTRUM_ANALYZER_H

