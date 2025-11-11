#include "milos/pqc/algorithm_factory.h"
#include <mutex>
#include <algorithm>

AlgorithmFactory& AlgorithmFactory::getInstance() {
    static AlgorithmFactory instance;
    return instance;
}

bool AlgorithmFactory::registerAlgorithm(const std::string& algorithmId, AlgorithmCreator creator) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_creators.find(algorithmId) != m_creators.end()) {
        return false;  // Already registered
    }
    
    m_creators[algorithmId] = creator;
    return true;
}

std::unique_ptr<IAlgorithm> AlgorithmFactory::createAlgorithm(const std::string& algorithmId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_creators.find(algorithmId);
    if (it == m_creators.end()) {
        return nullptr;
    }
    
    return it->second();
}

std::vector<std::string> AlgorithmFactory::getRegisteredAlgorithms() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::string> algorithms;
    algorithms.reserve(m_creators.size());
    
    for (const auto& pair : m_creators) {
        algorithms.push_back(pair.first);
    }
    
    return algorithms;
}

bool AlgorithmFactory::isAlgorithmRegistered(const std::string& algorithmId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_creators.find(algorithmId) != m_creators.end();
}

bool AlgorithmFactory::unregisterAlgorithm(const std::string& algorithmId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_creators.find(algorithmId);
    if (it == m_creators.end()) {
        return false;
    }
    
    m_creators.erase(it);
    return true;
}

