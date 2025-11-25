#include "indicators.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QRegExp>
#include <QtMath>

PerformanceIndicators::PerformanceIndicators(QObject *parent)
    : QObject(parent)
    , m_indicatorVisible(false)
    , m_currentMode("standard")
    , m_frameRate(60.0)
    , m_cpuUsage(0.0)
    , m_gpuUsage(0.0)
    , m_suggestedMode("standard")
    , m_monitoringTimer(new QTimer(this))
    , m_monitoring(false)
{
    connect(m_monitoringTimer, &QTimer::timeout, this, &PerformanceIndicators::updateMetrics);
    m_monitoringTimer->setInterval(1000); // Update every second
}

PerformanceIndicators::~PerformanceIndicators()
{
    stopMonitoring();
}

void PerformanceIndicators::showIndicator()
{
    if (!m_indicatorVisible) {
        m_indicatorVisible = true;
        emit indicatorVisibleChanged();
    }
}

void PerformanceIndicators::hideIndicator()
{
    if (m_indicatorVisible) {
        m_indicatorVisible = false;
        emit indicatorVisibleChanged();
    }
}

void PerformanceIndicators::updateMode(const QString &mode)
{
    if (m_currentMode != mode) {
        m_currentMode = mode;
        calculateSuggestedMode();
        emit currentModeChanged();
    }
}

void PerformanceIndicators::startMonitoring()
{
    if (!m_monitoring) {
        m_monitoring = true;
        m_monitoringTimer->start();
        updateMetrics(); // Initial update
    }
}

void PerformanceIndicators::stopMonitoring()
{
    if (m_monitoring) {
        m_monitoring = false;
        m_monitoringTimer->stop();
    }
}

QString PerformanceIndicators::getSuggestion()
{
    calculateSuggestedMode();
    return m_suggestedMode;
}

void PerformanceIndicators::updateMetrics()
{
    m_frameRate = getCurrentFrameRate();
    m_cpuUsage = getCurrentCpuUsage();
    m_gpuUsage = getCurrentGpuUsage();
    
    m_metrics["frameRate"] = m_frameRate;
    m_metrics["cpuUsage"] = m_cpuUsage;
    m_metrics["gpuUsage"] = m_gpuUsage;
    m_metrics["currentMode"] = m_currentMode;
    
    emit frameRateChanged();
    emit cpuUsageChanged();
    emit gpuUsageChanged();
    emit metricsChanged();
    
    calculateSuggestedMode();
}

void PerformanceIndicators::calculateSuggestedMode()
{
    QString suggestion = "standard";
    
    // Suggest performance mode if frame rate is low or CPU/GPU usage is high
    if (m_frameRate < 30.0 || m_cpuUsage > 80.0 || m_gpuUsage > 80.0) {
        suggestion = "performance";
    }
    
    // Suggest low-power mode if CPU/GPU usage is very high
    if (m_cpuUsage > 90.0 || m_gpuUsage > 90.0) {
        suggestion = "low-power";
    }
    
    // Suggest standard mode if performance is good
    if (m_frameRate >= 60.0 && m_cpuUsage < 50.0 && m_gpuUsage < 50.0) {
        suggestion = "standard";
    }
    
    if (m_suggestedMode != suggestion) {
        m_suggestedMode = suggestion;
        emit suggestedModeChanged();
    }
}

double PerformanceIndicators::getCurrentFrameRate()
{
    // In a real implementation, this would get frame rate from the compositor
    // For now, return a simulated value based on mode
    static int counter = 0;
    counter++;
    
    if (m_currentMode == "low-power") {
        return 45.0 + (counter % 10); // 45-55 fps
    } else if (m_currentMode == "performance") {
        return 55.0 + (counter % 10); // 55-65 fps
    }
    return 60.0; // Standard mode target
}

double PerformanceIndicators::getCurrentCpuUsage()
{
    // Read CPU usage from /proc/stat
    static qint64 lastTotal = 0;
    static qint64 lastIdle = 0;
    
    QFile statFile("/proc/stat");
    if (!statFile.open(QIODevice::ReadOnly)) {
        return 0.0;
    }
    
    QTextStream stream(&statFile);
    QString line = stream.readLine();
    statFile.close();
    
    if (!line.startsWith("cpu ")) {
        return 0.0;
    }
    
    QStringList parts = line.split(QRegExp("\\s+"));
    if (parts.size() < 8) {
        return 0.0;
    }
    
    qint64 user = parts[1].toLongLong();
    qint64 nice = parts[2].toLongLong();
    qint64 system = parts[3].toLongLong();
    qint64 idle = parts[4].toLongLong();
    qint64 iowait = parts[5].toLongLong();
    
    qint64 total = user + nice + system + idle + iowait;
    qint64 totalIdle = idle + iowait;
    
    if (lastTotal == 0) {
        lastTotal = total;
        lastIdle = totalIdle;
        return 0.0;
    }
    
    qint64 totalDiff = total - lastTotal;
    qint64 idleDiff = totalIdle - lastIdle;
    
    lastTotal = total;
    lastIdle = totalIdle;
    
    if (totalDiff == 0) {
        return 0.0;
    }
    
    double usage = 100.0 * (1.0 - static_cast<double>(idleDiff) / static_cast<double>(totalDiff));
    return qBound(0.0, usage, 100.0);
}

double PerformanceIndicators::getCurrentGpuUsage()
{
    // GPU usage detection would require GPU-specific tools
    // For NVIDIA: nvidia-smi
    // For AMD: radeontop or similar
    // For Intel: intel_gpu_top
    
    // Try nvidia-smi first
    QProcess process;
    process.start("nvidia-smi", QStringList() << "--query-gpu=utilization.gpu" << "--format=csv,noheader,nounits");
    process.waitForFinished(1000);
    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput().trimmed();
        bool ok;
        double usage = output.toDouble(&ok);
        if (ok) {
            return usage;
        }
    }
    
    // Fallback: return 0 or simulate based on mode
    return 0.0;
}

