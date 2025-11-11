#ifndef ALGORITHM_REGISTRY_H
#define ALGORITHM_REGISTRY_H

#include "algorithm_interface.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <algorithm>

/**
 * @brief Algorithm Registry
 * 
 * Manages algorithm metadata, versioning, capabilities, and performance metrics.
 */
class AlgorithmRegistry {
public:
    /**
     * @brief Get singleton instance
     * @return Registry instance
     */
    static AlgorithmRegistry& getInstance();

    /**
     * @brief Register algorithm metadata
     * @param metadata Algorithm metadata
     * @return true if registration successful, false otherwise
     */
    bool registerAlgorithm(const AlgorithmMetadata& metadata);

    /**
     * @brief Get algorithm metadata
     * @param algorithmId Algorithm ID
     * @return Algorithm metadata, nullptr if not found
     */
    std::unique_ptr<AlgorithmMetadata> getAlgorithmMetadata(const std::string& algorithmId) const;

    /**
     * @brief Get all registered algorithms
     * @return Vector of algorithm metadata
     */
    std::vector<AlgorithmMetadata> getAllAlgorithms() const;

    /**
     * @brief Get algorithms by capability
     * @param capability Algorithm capability
     * @return Vector of algorithm metadata
     */
    std::vector<AlgorithmMetadata> getAlgorithmsByCapability(AlgorithmCapability capability) const;

    /**
     * @brief Get NIST-approved algorithms
     * @return Vector of algorithm metadata
     */
    std::vector<AlgorithmMetadata> getNISTApprovedAlgorithms() const;

    /**
     * @brief Update algorithm performance metrics
     * @param algorithmId Algorithm ID
     * @param performanceScore Performance score
     * @return true if update successful, false otherwise
     */
    bool updatePerformanceMetrics(const std::string& algorithmId, double performanceScore);

    /**
     * @brief Get algorithm version
     * @param algorithmId Algorithm ID
     * @return Algorithm version, empty string if not found
     */
    std::string getAlgorithmVersion(const std::string& algorithmId) const;

private:
    AlgorithmRegistry() = default;
    ~AlgorithmRegistry() = default;
    AlgorithmRegistry(const AlgorithmRegistry&) = delete;
    AlgorithmRegistry& operator=(const AlgorithmRegistry&) = delete;

    std::map<std::string, AlgorithmMetadata> m_algorithms;
    mutable std::mutex m_mutex;
};

#endif // ALGORITHM_REGISTRY_H

