#include "milos/pqc/algorithm_versioning.h"
#include <mutex>

AlgorithmVersioning& AlgorithmVersioning::getInstance() {
    static AlgorithmVersioning instance;
    return instance;
}

bool AlgorithmVersioning::registerVersion(const AlgorithmVersion& version) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_versions[version.algorithm_id][version.version] = version;
    return true;
}

std::unique_ptr<AlgorithmVersion> AlgorithmVersioning::getVersionInfo(
    const std::string& algorithmId, 
    const std::string& version
) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto algIt = m_versions.find(algorithmId);
    if (algIt == m_versions.end()) {
        return nullptr;
    }
    
    auto verIt = algIt->second.find(version);
    if (verIt == algIt->second.end()) {
        return nullptr;
    }
    
    return std::make_unique<AlgorithmVersion>(verIt->second);
}

bool AlgorithmVersioning::isBackwardCompatible(
    const std::string& algorithmId,
    const std::string& fromVersion,
    const std::string& toVersion
) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto algIt = m_versions.find(algorithmId);
    if (algIt == m_versions.end()) {
        return false;
    }
    
    auto fromIt = algIt->second.find(fromVersion);
    auto toIt = algIt->second.find(toVersion);
    
    if (fromIt == algIt->second.end() || toIt == algIt->second.end()) {
        return false;
    }
    
    // Check if target version is backward compatible
    return toIt->second.is_backward_compatible;
}

std::string AlgorithmVersioning::getMigrationPath(
    const std::string& algorithmId,
    const std::string& fromVersion,
    const std::string& toVersion
) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto algIt = m_versions.find(algorithmId);
    if (algIt == m_versions.end()) {
        return "";
    }
    
    auto toIt = algIt->second.find(toVersion);
    if (toIt == algIt->second.end()) {
        return "";
    }
    
    return toIt->second.migration_path;
}

std::vector<AlgorithmVersion> AlgorithmVersioning::getAlgorithmVersions(const std::string& algorithmId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto algIt = m_versions.find(algorithmId);
    if (algIt == m_versions.end()) {
        return {};
    }
    
    std::vector<AlgorithmVersion> versions;
    versions.reserve(algIt->second.size());
    
    for (const auto& pair : algIt->second) {
        versions.push_back(pair.second);
    }
    
    return versions;
}

