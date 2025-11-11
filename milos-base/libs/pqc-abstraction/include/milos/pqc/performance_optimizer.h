#ifndef PERFORMANCE_OPTIMIZER_H
#define PERFORMANCE_OPTIMIZER_H

#include "algorithm_interface.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>

/**
 * @brief Performance Metrics
 */
struct PerformanceMetrics {
    std::string algorithm_id;
    size_t data_size;
    std::chrono::milliseconds encryption_time;
    std::chrono::milliseconds decryption_time;
    double throughput_mbps;  // Megabytes per second
    size_t memory_usage;
    int operation_count;
};

/**
 * @brief Performance Optimizer
 * 
 * Profiles algorithm performance and optimizes algorithm selection.
 */
class PerformanceOptimizer {
public:
    /**
     * @brief Get singleton instance
     * @return Optimizer instance
     */
    static PerformanceOptimizer& getInstance();

    /**
     * @brief Profile algorithm operation
     * @param algorithmId Algorithm ID
     * @param operationType Operation type ("encrypt", "decrypt")
     * @param dataSize Data size in bytes
     * @param duration Operation duration
     * @return true if profiling successful, false otherwise
     */
    bool profileOperation(
        const std::string& algorithmId,
        const std::string& operationType,
        size_t dataSize,
        std::chrono::milliseconds duration
    );

    /**
     * @brief Get algorithm performance metrics
     * @param algorithmId Algorithm ID
     * @return Performance metrics, nullptr if not found
     */
    std::unique_ptr<PerformanceMetrics> getPerformanceMetrics(const std::string& algorithmId) const;

    /**
     * @brief Get recommended algorithm for data size
     * @param dataSize Data size in bytes
     * @param capability Required algorithm capability
     * @return Recommended algorithm ID, empty string if not found
     */
    std::string getRecommendedAlgorithm(size_t dataSize, AlgorithmCapability capability) const;

    /**
     * @brief Enable/disable performance profiling
     * @param enabled Enable profiling
     */
    void setProfilingEnabled(bool enabled) { m_profilingEnabled = enabled; }

    /**
     * @brief Check if profiling is enabled
     * @return true if enabled, false otherwise
     */
    bool isProfilingEnabled() const { return m_profilingEnabled; }

    /**
     * @brief Clear performance metrics
     */
    void clearMetrics();

private:
    PerformanceOptimizer() : m_profilingEnabled(true) {}
    ~PerformanceOptimizer() = default;
    PerformanceOptimizer(const PerformanceOptimizer&) = delete;
    PerformanceOptimizer& operator=(const PerformanceOptimizer&) = delete;

    bool m_profilingEnabled;
    std::map<std::string, PerformanceMetrics> m_metrics;
    mutable std::mutex m_mutex;

    /**
     * @brief Calculate throughput
     * @param dataSize Data size in bytes
     * @param duration Duration in milliseconds
     * @return Throughput in MB/s
     */
    double calculateThroughput(size_t dataSize, std::chrono::milliseconds duration) const;
};

#endif // PERFORMANCE_OPTIMIZER_H

