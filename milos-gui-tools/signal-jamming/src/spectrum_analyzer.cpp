#include "spectrum_analyzer.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

SpectrumAnalyzer::SpectrumAnalyzer(QObject* parent)
    : QObject(parent)
    , m_analyzing(false)
    , m_frequencyStart(0)
    , m_frequencyEnd(0)
    , m_analysisTimer(new QTimer(this))
{
    connect(m_analysisTimer, &QTimer::timeout, this, &SpectrumAnalyzer::performAnalysis);
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
}

bool SpectrumAnalyzer::initialize() {
    m_analysisTimer->setInterval(100);  // Update every 100ms
    return true;
}

bool SpectrumAnalyzer::startAnalysis(double frequencyStart, double frequencyEnd) {
    if (m_analyzing) {
        stopAnalysis();
    }
    
    m_frequencyStart = frequencyStart;
    m_frequencyEnd = frequencyEnd;
    m_spectrumData.clear();
    
    m_analyzing = true;
    m_analysisTimer->start();
    emit analyzingChanged(true);
    
    // Perform initial analysis
    performAnalysis();
    
    return true;
}

void SpectrumAnalyzer::stopAnalysis() {
    if (!m_analyzing) {
        return;
    }
    
    m_analysisTimer->stop();
    m_analyzing = false;
    emit analyzingChanged(false);
}

QList<FrequencyDataPoint> SpectrumAnalyzer::getSpectrumData() const {
    return m_spectrumData;
}

QVariantMap SpectrumAnalyzer::getFrequencyStatistics() const {
    QVariantMap stats;
    
    if (m_spectrumData.isEmpty()) {
        return stats;
    }
    
    double maxPower = -1000;
    double minPower = 1000;
    double avgPower = 0;
    
    for (const FrequencyDataPoint& point : m_spectrumData) {
        if (point.power > maxPower) {
            maxPower = point.power;
        }
        if (point.power < minPower) {
            minPower = point.power;
        }
        avgPower += point.power;
    }
    
    avgPower /= m_spectrumData.size();
    
    stats["max_power"] = maxPower;
    stats["min_power"] = minPower;
    stats["avg_power"] = avgPower;
    stats["data_points"] = m_spectrumData.size();
    
    return stats;
}

void SpectrumAnalyzer::performAnalysis() {
    // In production, would use actual spectrum analyzer hardware
    // For now, generate simulated data
    
    m_spectrumData.clear();
    
    double step = (m_frequencyEnd - m_frequencyStart) / 100.0;  // 100 points
    
    for (int i = 0; i < 100; ++i) {
        double frequency = m_frequencyStart + (step * i);
        FrequencyDataPoint point = analyzeFrequency(frequency);
        m_spectrumData.append(point);
    }
    
    emit spectrumDataUpdated(m_spectrumData);
}

FrequencyDataPoint SpectrumAnalyzer::analyzeFrequency(double frequency) const {
    FrequencyDataPoint point;
    point.frequency = frequency;
    point.timestamp = QDateTime::currentDateTime();
    
    // Simulate power reading
    // In production, would read from hardware
    point.power = -80.0 + (QRandomGenerator::global()->bounded(40.0));
    
    return point;
}

