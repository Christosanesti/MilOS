#ifndef BASELINE_MANAGER_H
#define BASELINE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * @brief Baseline Information
 */
struct BaselineInfo {
    std::string baseline_id;
    std::string file_path;
    std::string hash_algorithm;
    std::string file_hash;
    size_t file_size;
    std::string permissions;
    std::string owner;
    std::string group;
    std::string created_at;
    std::string version;
    bool is_valid;
};

/**
 * @brief Baseline Manager
 * 
 * Manages file integrity baselines: creation, storage, updates, and verification.
 */
class BaselineManager {
public:
    BaselineManager();
    ~BaselineManager();

    /**
     * @brief Initialize baseline manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create baseline for file(s)
     * @param filePaths List of file paths to create baselines for
     * @return List of created baseline IDs
     */
    std::vector<std::string> createBaseline(const std::vector<std::string>& filePaths);

    /**
     * @brief Get baseline information
     * @param baselineId Baseline ID
     * @return Baseline information, or empty if not found
     */
    BaselineInfo getBaseline(const std::string& baselineId) const;

    /**
     * @brief Get baseline status
     * @param baselineId Baseline ID
     * @return Status string
     */
    std::string getBaselineStatus(const std::string& baselineId) const;

    /**
     * @brief Update baseline (creates new version)
     * @param baselineId Baseline ID
     * @return true if update successful, false otherwise
     */
    bool updateBaseline(const std::string& baselineId);

    /**
     * @brief Verify baseline integrity
     * @param baselineId Baseline ID
     * @return true if baseline is valid, false otherwise
     */
    bool verifyBaseline(const std::string& baselineId) const;

    /**
     * @brief Get baseline for file path
     * @param filePath File path
     * @return Baseline information, or empty if not found
     */
    BaselineInfo getBaselineForFile(const std::string& filePath) const;

    /**
     * @brief Get baseline versions
     * @param baselineId Baseline ID
     * @return List of baseline versions (oldest first)
     */
    std::vector<BaselineInfo> getBaselineVersions(const std::string& baselineId) const;

    /**
     * @brief Get specific baseline version
     * @param baselineId Baseline ID
     * @param version Version string (e.g., "1.0.0", "2.0.0")
     * @return Baseline information for the specified version, or empty if not found
     */
    BaselineInfo getBaselineVersion(const std::string& baselineId, const std::string& version) const;

    /**
     * @brief Rollback baseline to a specific version
     * @param baselineId Baseline ID
     * @param version Version to rollback to
     * @return true if rollback successful, false otherwise
     */
    bool rollbackBaseline(const std::string& baselineId, const std::string& version);

    /**
     * @brief Get all file paths that have baselines (monitored files)
     * @return List of file paths
     */
    std::vector<std::string> getAllMonitoredFiles() const;

private:
    bool m_initialized;
    std::string m_storagePath;
    std::string m_hashAlgorithm;
    
    // In-memory baseline cache
    std::map<std::string, BaselineInfo> m_baselines;  // baseline_id -> current baseline
    std::map<std::string, std::string> m_fileToBaseline;  // file path -> baseline ID
    std::map<std::string, std::vector<BaselineInfo>> m_baselineVersions;  // baseline_id -> list of versions

    /**
     * @brief Calculate file hash
     * @param filePath File path
     * @return File hash
     */
    std::string calculateFileHash(const std::string& filePath) const;

    /**
     * @brief Load baselines from storage
     */
    void loadBaselines();

    /**
     * @brief Save baseline to storage
     * @param baseline Baseline information
     */
    void saveBaseline(const BaselineInfo& baseline);
};

#endif // BASELINE_MANAGER_H

