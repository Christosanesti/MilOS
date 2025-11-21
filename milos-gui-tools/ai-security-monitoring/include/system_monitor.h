#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QTimer>

/**
 * @brief System Monitor
 * 
 * Monitors system resources and processes.
 */
class SystemMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap systemMetrics READ systemMetrics NOTIFY metricsUpdated)

public:
    explicit SystemMonitor(QObject* parent = nullptr);
    ~SystemMonitor();

    /**
     * @brief Initialize system monitor
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
     * @brief Get system metrics
     * @return System metrics map
     */
    QVariantMap systemMetrics() const { return m_metrics; }

    /**
     * @brief Get process list
     * @return List of processes
     */
    QVariantList getProcessList() const;

    /**
     * @brief Get network connections
     * @return List of network connections
     */
    QVariantList getNetworkConnections() const;

Q_SIGNALS:
    /**
     * @brief Emitted when metrics are updated
     */
    void metricsUpdated(const QVariantMap& metrics);

    /**
     * @brief Emitted when anomaly is detected
     */
    void anomalyDetected(const QString& type, const QVariantMap& details);

private slots:
    void collectMetrics();

private:
    QVariantMap m_metrics;
    QTimer* m_collectionTimer;
    
    void updateCPUMetrics();
    void updateMemoryMetrics();
    void updateDiskMetrics();
    void updateNetworkMetrics();
    bool detectAnomalies() const;
};

#endif // SYSTEM_MONITOR_H

