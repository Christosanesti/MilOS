#ifndef PERFORMANCE_INDICATORS_H
#define PERFORMANCE_INDICATORS_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QTimer>

/**
 * @brief Performance Indicators Manager
 * 
 * Manages visual indicators and status display for performance modes:
 * - Visual indicator when performance mode is active
 * - Performance status display (current mode, frame rate, metrics)
 * - Performance mode suggestions
 */
class PerformanceIndicators : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool indicatorVisible READ indicatorVisible NOTIFY indicatorVisibleChanged)
    Q_PROPERTY(QString currentMode READ currentMode NOTIFY currentModeChanged)
    Q_PROPERTY(double frameRate READ frameRate NOTIFY frameRateChanged)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(double gpuUsage READ gpuUsage NOTIFY gpuUsageChanged)
    Q_PROPERTY(QString suggestedMode READ suggestedMode NOTIFY suggestedModeChanged)
    Q_PROPERTY(QVariantMap metrics READ metrics NOTIFY metricsChanged)

public:
    explicit PerformanceIndicators(QObject *parent = nullptr);
    ~PerformanceIndicators();
    
    bool indicatorVisible() const { return m_indicatorVisible; }
    QString currentMode() const { return m_currentMode; }
    double frameRate() const { return m_frameRate; }
    double cpuUsage() const { return m_cpuUsage; }
    double gpuUsage() const { return m_gpuUsage; }
    QString suggestedMode() const { return m_suggestedMode; }
    QVariantMap metrics() const { return m_metrics; }
    
    Q_INVOKABLE void showIndicator();
    Q_INVOKABLE void hideIndicator();
    Q_INVOKABLE void updateMode(const QString &mode);
    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();
    Q_INVOKABLE QString getSuggestion();

signals:
    void indicatorVisibleChanged();
    void currentModeChanged();
    void frameRateChanged();
    void cpuUsageChanged();
    void gpuUsageChanged();
    void suggestedModeChanged();
    void metricsChanged();

private slots:
    void updateMetrics();

private:
    void calculateSuggestedMode();
    double getCurrentFrameRate();
    double getCurrentCpuUsage();
    double getCurrentGpuUsage();
    
    bool m_indicatorVisible;
    QString m_currentMode;
    double m_frameRate;
    double m_cpuUsage;
    double m_gpuUsage;
    QString m_suggestedMode;
    QVariantMap m_metrics;
    QTimer* m_monitoringTimer;
    bool m_monitoring;
};

#endif // PERFORMANCE_INDICATORS_H

