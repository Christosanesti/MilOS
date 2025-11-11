#include "milos/pqc/performance_optimizer.h"
#include <algorithm>
#include <cmath>

PerformanceOptimizer& PerformanceOptimizer::getInstance() {
    static PerformanceOptimizer instance;
    return instance;
}

bool PerformanceOptimizer::profileOperation(
    const std::string& algorithmId,
    const std::string& operationType,
    size_t dataSize,
    std::chrono::milliseconds duration
) {
    if (!m_profilingEnabled) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    PerformanceMetrics& metrics = m_metrics[algorithmId];
    metrics.algorithm_id = algorithmId;
    metrics.data_size = dataSize;

    if (operationType == "encrypt") {
        metrics.encryption_time = duration;
    } else if (operationType == "decrypt") {
        metrics.decryption_time = duration;
    }

    // Calculate throughput
    metrics.throughput_mbps = calculateThroughput(dataSize, duration);
    metrics.operation_count++;

    return true;
}

std::unique_ptr<PerformanceMetrics> PerformanceOptimizer::getPerformanceMetrics(
    const std::string& algorithmId
) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_metrics.find(algorithmId);
    if (it == m_metrics.end()) {
        return nullptr;
    }

    return std::make_unique<PerformanceMetrics>(it->second);
}

std::string PerformanceOptimizer::getRecommendedAlgorithm(
    size_t dataSize,
    AlgorithmCapability capability
) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Find algorithm with best performance for given data size
    std::string bestAlgorithm;
    double bestThroughput = 0.0;

    for (const auto& pair : m_metrics) {
        const auto& metrics = pair.second;
        
        // Check if algorithm supports the required capability
        // This would require access to algorithm registry
        // For now, we'll just check performance
        
        if (metrics.throughput_mbps > bestThroughput) {
            bestThroughput = metrics.throughput_mbps;
            bestAlgorithm = metrics.algorithm_id;
        }
    }

    return bestAlgorithm;
}

void PerformanceOptimizer::clearMetrics() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_metrics.clear();
}

double PerformanceOptimizer::calculateThroughput(
    size_t dataSize,
    std::chrono::milliseconds duration
) const {
    if (duration.count() == 0) {
        return 0.0;
    }

    // Convert bytes to megabytes
    double dataSizeMB = static_cast<double>(dataSize) / (1024.0 * 1024.0);
    
    // Convert milliseconds to seconds
    double durationSeconds = static_cast<double>(duration.count()) / 1000.0;
    
    // Calculate throughput in MB/s
    return dataSizeMB / durationSeconds;
}

