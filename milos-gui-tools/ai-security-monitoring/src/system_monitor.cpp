#include "system_monitor.h"
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

SystemMonitor::SystemMonitor(QObject* parent)
    : QObject(parent)
    , m_collectionTimer(new QTimer(this))
{
    connect(m_collectionTimer, &QTimer::timeout, this, &SystemMonitor::collectMetrics);
}

SystemMonitor::~SystemMonitor() {
}

bool SystemMonitor::initialize() {
    m_collectionTimer->setInterval(1000);  // Collect every second
    return true;
}

bool SystemMonitor::startMonitoring() {
    m_collectionTimer->start();
    collectMetrics();  // Initial collection
    return true;
}

void SystemMonitor::stopMonitoring() {
    m_collectionTimer->stop();
}

QVariantList SystemMonitor::getProcessList() const {
    QVariantList processes;
    
    // In production, would read from /proc or use system APIs
    // For now, simulate process list
    QVariantMap proc1;
    proc1["pid"] = 1001;
    proc1["name"] = "systemd";
    proc1["cpu"] = 2.5;
    proc1["memory"] = 150.0;
    processes.append(proc1);
    
    return processes;
}

QVariantList SystemMonitor::getNetworkConnections() const {
    QVariantList connections;
    
    // In production, would read from /proc/net/tcp or use netstat
    // For now, simulate connections
    QVariantMap conn1;
    conn1["local"] = "127.0.0.1:22";
    conn1["remote"] = "192.168.1.100:54321";
    conn1["state"] = "ESTABLISHED";
    connections.append(conn1);
    
    return connections;
}

void SystemMonitor::collectMetrics() {
    updateCPUMetrics();
    updateMemoryMetrics();
    updateDiskMetrics();
    updateNetworkMetrics();
    
    emit metricsUpdated(m_metrics);
    
    if (detectAnomalies()) {
        emit anomalyDetected("system", m_metrics);
    }
}

void SystemMonitor::updateCPUMetrics() {
    // In production, would read from /proc/stat
    m_metrics["cpu_usage"] = QRandomGenerator::global()->bounded(100);
    m_metrics["cpu_cores"] = 4;
}

void SystemMonitor::updateMemoryMetrics() {
    // In production, would read from /proc/meminfo
    m_metrics["memory_total"] = 8192;  // MB
    m_metrics["memory_used"] = QRandomGenerator::global()->bounded(4096);
    m_metrics["memory_free"] = 8192 - m_metrics["memory_used"].toInt();
    m_metrics["memory_percent"] = (m_metrics["memory_used"].toInt() * 100) / 8192;
}

void SystemMonitor::updateDiskMetrics() {
    // In production, would read from df or /proc/diskstats
    m_metrics["disk_total"] = 500000;  // MB
    m_metrics["disk_used"] = QRandomGenerator::global()->bounded(250000);
    m_metrics["disk_free"] = 500000 - m_metrics["disk_used"].toInt();
    m_metrics["disk_percent"] = (m_metrics["disk_used"].toInt() * 100) / 500000;
}

void SystemMonitor::updateNetworkMetrics() {
    // In production, would read from /proc/net/dev
    m_metrics["network_rx"] = QRandomGenerator::global()->bounded(1000000);
    m_metrics["network_tx"] = QRandomGenerator::global()->bounded(1000000);
}

bool SystemMonitor::detectAnomalies() const {
    // Simple anomaly detection
    int cpuUsage = m_metrics["cpu_usage"].toInt();
    int memoryPercent = m_metrics["memory_percent"].toInt();
    
    return (cpuUsage > 90 || memoryPercent > 90);
}

