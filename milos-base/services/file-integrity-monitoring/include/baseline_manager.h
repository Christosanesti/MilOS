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
     * @brief Update baseline
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

private:
    bool m_initialized;
    std::string m_storagePath;
    std::string m_hashAlgorithm;
    
    // In-memory baseline cache
    std::map<std::string, BaselineInfo> m_baselines;
    std::map<std::string, std::string> m_fileToBaseline;  // file path -> baseline ID

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

