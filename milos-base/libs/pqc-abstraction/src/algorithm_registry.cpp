#include "milos/pqc/algorithm_registry.h"
#include <algorithm>

AlgorithmRegistry& AlgorithmRegistry::getInstance() {
    static AlgorithmRegistry instance;
    return instance;
}

bool AlgorithmRegistry::registerAlgorithm(const AlgorithmMetadata& metadata) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_algorithms[metadata.algorithm_id] = metadata;
    return true;
}

std::unique_ptr<AlgorithmMetadata> AlgorithmRegistry::getAlgorithmMetadata(const std::string& algorithmId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        return nullptr;
    }
    
    return std::make_unique<AlgorithmMetadata>(it->second);
}

std::vector<AlgorithmMetadata> AlgorithmRegistry::getAllAlgorithms() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<AlgorithmMetadata> algorithms;
    algorithms.reserve(m_algorithms.size());
    
    for (const auto& pair : m_algorithms) {
        algorithms.push_back(pair.second);
    }
    
    return algorithms;
}

std::vector<AlgorithmMetadata> AlgorithmRegistry::getAlgorithmsByCapability(AlgorithmCapability capability) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<AlgorithmMetadata> algorithms;
    
    for (const auto& pair : m_algorithms) {
        const auto& metadata = pair.second;
        if (std::find(metadata.capabilities.begin(), metadata.capabilities.end(), capability) 
            != metadata.capabilities.end()) {
            algorithms.push_back(metadata);
        }
    }
    
    return algorithms;
}

std::vector<AlgorithmMetadata> AlgorithmRegistry::getNISTApprovedAlgorithms() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<AlgorithmMetadata> algorithms;
    
    for (const auto& pair : m_algorithms) {
        if (pair.second.is_nist_approved) {
            algorithms.push_back(pair.second);
        }
    }
    
    return algorithms;
}

bool AlgorithmRegistry::updatePerformanceMetrics(const std::string& algorithmId, double performanceScore) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        return false;
    }
    
    it->second.performance_score = performanceScore;
    return true;
}

std::string AlgorithmRegistry::getAlgorithmVersion(const std::string& algorithmId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        return "";
    }
    
    return it->second.version;
}

