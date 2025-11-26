#include "performancemonitor.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QElapsedTimer>
#include <QThread>

PerformanceMonitor::PerformanceMonitor(QObject* parent)
    : QObject(parent)
    , m_frameRate(60.0)
    , m_cpuUsage(0.0)
    , m_gpuUsage(0.0)
    , m_effectCost(0.0)
    , m_responseTime(0)
    , m_degraded(false)
    , m_monitorTimer(new QTimer(this))
{
    m_monitorTimer->setInterval(1000); // Update every second
    connect(m_monitorTimer, &QTimer::timeout, this, &PerformanceMonitor::updateMetrics);
}

PerformanceMonitor::~PerformanceMonitor()
{
    stopMonitoring();
}

void PerformanceMonitor::startMonitoring()
{
    if (m_monitorTimer->isActive()) {
        return;
    }
    m_monitorTimer->start();
    updateMetrics();
}

void PerformanceMonitor::stopMonitoring()
{
    m_monitorTimer->stop();
}

QVariantMap PerformanceMonitor::getMetrics() const
{
    QVariantMap metrics;
    metrics["frameRate"] = m_frameRate;
    metrics["cpuUsage"] = m_cpuUsage;
    metrics["gpuUsage"] = m_gpuUsage;
    metrics["effectCost"] = m_effectCost;
    metrics["responseTime"] = m_responseTime;
    metrics["degraded"] = m_degraded;
    return metrics;
}

bool PerformanceMonitor::isDegraded() const
{
    return m_degraded;
}

void PerformanceMonitor::updateMetrics()
{
    updateFrameRate();
    updateCpuUsage();
    updateGpuUsage();
    updateEffectCost();
    updateResponseTime();
    checkDegradation();
    
    emit metricsUpdated();
}

void PerformanceMonitor::updateFrameRate()
{
    // Frame rate tracking - simplified implementation
    // In a real implementation, this would track actual frame rendering
    // For now, we'll use a placeholder that simulates frame rate
    static qint64 lastTime = 0;
    qint64 currentTime = QElapsedTimer().elapsed();
    
    if (lastTime > 0) {
        qint64 delta = currentTime - lastTime;
        if (delta > 0) {
            m_frameRate = 1000.0 / delta; // Simplified calculation
        }
    }
    lastTime = currentTime;
}

void PerformanceMonitor::updateCpuUsage()
{
    // Read CPU usage from /proc/stat
    QFile file("/proc/stat");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString line = stream.readLine();
        if (line.startsWith("cpu ")) {
            QStringList parts = line.split(QRegExp("\\s+"));
            if (parts.size() >= 8) {
                // Simplified CPU usage calculation
                // In a real implementation, this would track CPU time deltas
                static qint64 lastIdle = 0, lastTotal = 0;
                qint64 idle = parts[4].toLongLong();
                qint64 total = 0;
                for (int i = 1; i < parts.size(); ++i) {
                    total += parts[i].toLongLong();
                }
                
                if (lastTotal > 0) {
                    qint64 idleDelta = idle - lastIdle;
                    qint64 totalDelta = total - lastTotal;
                    if (totalDelta > 0) {
                        m_cpuUsage = 100.0 * (1.0 - (double)idleDelta / totalDelta);
                    }
                }
                
                lastIdle = idle;
                lastTotal = total;
            }
        }
        file.close();
    }
}

void PerformanceMonitor::updateGpuUsage()
{
    // GPU usage monitoring - simplified
    // In a real implementation, this would use GPU-specific APIs (nvidia-smi, etc.)
    // For now, we'll use a placeholder
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface gpuInterface("org.milos.GPU", "/GPU", "org.milos.GPU", connection);
    
    if (gpuInterface.isValid()) {
        QDBusReply<double> reply = gpuInterface.call("GetUsage");
        if (reply.isValid()) {
            m_gpuUsage = reply.value();
        }
    } else {
        // Fallback: estimate based on CPU usage
        m_gpuUsage = m_cpuUsage * 0.8;
    }
}

void PerformanceMonitor::updateEffectCost()
{
    // Get xenon effect performance cost from xenon effects library
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface xenonInterface("org.milos.XenonEffects", "/XenonEffects", "org.milos.XenonEffects", connection);
    
    if (xenonInterface.isValid()) {
        QDBusReply<double> reply = xenonInterface.call("GetEffectCost");
        if (reply.isValid()) {
            m_effectCost = reply.value();
        }
    } else {
        m_effectCost = 0.0;
    }
}

void PerformanceMonitor::updateResponseTime()
{
    // Measure response time - simplified
    // In a real implementation, this would measure actual user interaction response times
    static QElapsedTimer timer;
    if (!timer.isValid()) {
        timer.start();
    }
    
    // Simulate response time measurement
    m_responseTime = timer.elapsed() % 300; // Placeholder
}

void PerformanceMonitor::checkDegradation()
{
    bool wasDegraded = m_degraded;
    
    // Check if performance is degraded
    m_degraded = (m_frameRate < FRAME_RATE_THRESHOLD) ||
                 (m_cpuUsage > CPU_USAGE_THRESHOLD) ||
                 (m_gpuUsage > GPU_USAGE_THRESHOLD) ||
                 (m_responseTime > RESPONSE_TIME_THRESHOLD);
    
    if (m_degraded && !wasDegraded) {
        emit performanceDegraded();
    } else if (!m_degraded && wasDegraded) {
        emit performanceRecovered();
    }
}


