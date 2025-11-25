#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantMap>

/**
 * @brief Performance Monitor
 * 
 * Monitors system performance metrics (frame rate, CPU, GPU, effect cost)
 */
class PerformanceMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double frameRate READ frameRate NOTIFY metricsUpdated)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY metricsUpdated)
    Q_PROPERTY(double gpuUsage READ gpuUsage NOTIFY metricsUpdated)
    Q_PROPERTY(double effectCost READ effectCost NOTIFY metricsUpdated)
    Q_PROPERTY(int responseTime READ responseTime NOTIFY metricsUpdated)

public:
    explicit PerformanceMonitor(QObject* parent = nullptr);
    ~PerformanceMonitor();

    double frameRate() const { return m_frameRate; }
    double cpuUsage() const { return m_cpuUsage; }
    double gpuUsage() const { return m_gpuUsage; }
    double effectCost() const { return m_effectCost; }
    int responseTime() const { return m_responseTime; }

    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();
    Q_INVOKABLE QVariantMap getMetrics() const;
    Q_INVOKABLE bool isDegraded() const;

signals:
    void metricsUpdated();
    void performanceDegraded();
    void performanceRecovered();

private slots:
    void updateMetrics();

private:
    void updateFrameRate();
    void updateCpuUsage();
    void updateGpuUsage();
    void updateEffectCost();
    void updateResponseTime();
    void checkDegradation();

    double m_frameRate;
    double m_cpuUsage;
    double m_gpuUsage;
    double m_effectCost;
    int m_responseTime;
    
    bool m_degraded;
    QTimer* m_monitorTimer;
    
    // Thresholds
    static constexpr double FRAME_RATE_THRESHOLD = 30.0;
    static constexpr double CPU_USAGE_THRESHOLD = 80.0;
    static constexpr double GPU_USAGE_THRESHOLD = 80.0;
    static constexpr int RESPONSE_TIME_THRESHOLD = 200;
};

#endif // PERFORMANCEMONITOR_H

