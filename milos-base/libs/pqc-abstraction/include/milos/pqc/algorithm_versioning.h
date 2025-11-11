#ifndef ALGORITHM_VERSIONING_H
#define ALGORITHM_VERSIONING_H

#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @brief Algorithm Version Information
 */
struct AlgorithmVersion {
    std::string algorithm_id;
    std::string version;
    std::string previous_version;  // For backward compatibility
    bool is_backward_compatible;
    std::string migration_path;  // Migration instructions
};

/**
 * @brief Algorithm Versioning Manager
 * 
 * Manages algorithm versioning, backward compatibility, and migration.
 */
class AlgorithmVersioning {
public:
    /**
     * @brief Get singleton instance
     * @return Versioning manager instance
     */
    static AlgorithmVersioning& getInstance();

    /**
     * @brief Register algorithm version
     * @param version Algorithm version information
     * @return true if registration successful, false otherwise
     */
    bool registerVersion(const AlgorithmVersion& version);

    /**
     * @brief Get algorithm version information
     * @param algorithmId Algorithm ID
     * @param version Version string
     * @return Version information, nullptr if not found
     */
    std::unique_ptr<AlgorithmVersion> getVersionInfo(const std::string& algorithmId, const std::string& version) const;

    /**
     * @brief Check if version is backward compatible
     * @param algorithmId Algorithm ID
     * @param fromVersion Source version
     * @param toVersion Target version
     * @return true if backward compatible, false otherwise
     */
    bool isBackwardCompatible(const std::string& algorithmId, 
                             const std::string& fromVersion, 
                             const std::string& toVersion) const;

    /**
     * @brief Get migration path
     * @param algorithmId Algorithm ID
     * @param fromVersion Source version
     * @param toVersion Target version
     * @return Migration path, empty string if not available
     */
    std::string getMigrationPath(const std::string& algorithmId,
                                 const std::string& fromVersion,
                                 const std::string& toVersion) const;

    /**
     * @brief Get all versions for algorithm
     * @param algorithmId Algorithm ID
     * @return Vector of version information
     */
    std::vector<AlgorithmVersion> getAlgorithmVersions(const std::string& algorithmId) const;

private:
    AlgorithmVersioning() = default;
    ~AlgorithmVersioning() = default;
    AlgorithmVersioning(const AlgorithmVersioning&) = delete;
    AlgorithmVersioning& operator=(const AlgorithmVersioning&) = delete;

    // Map: algorithm_id -> version -> AlgorithmVersion
    std::map<std::string, std::map<std::string, AlgorithmVersion>> m_versions;
    mutable std::mutex m_mutex;
};

#endif // ALGORITHM_VERSIONING_H

