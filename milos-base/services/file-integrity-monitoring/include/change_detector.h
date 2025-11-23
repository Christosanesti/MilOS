#ifndef CHANGE_DETECTOR_H
#define CHANGE_DETECTOR_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <set>

class BaselineManager;

/**
 * @brief Change Type
 */
enum class ChangeType {
    MODIFIED,
    DELETED,
    CREATED,
    PERMISSIONS_CHANGED,
    OWNERSHIP_CHANGED
};

/**
 * @brief File Change Information
 */
struct FileChange {
    std::string change_id;
    std::string file_path;
    ChangeType change_type;
    std::string baseline_id;
    std::string old_hash;
    std::string new_hash;
    std::string severity;
    std::string detected_at;
    std::string description;
};

/**
 * @brief Change Detection Callback
 */
using ChangeCallback = std::function<void(const FileChange& change)>;

/**
 * @brief Change Detector
 * 
 * Detects file changes by comparing current state with baselines.
 */
class ChangeDetector {
public:
    ChangeDetector();
    ~ChangeDetector();

    /**
     * @brief Initialize change detector
     * @param baselineManager Baseline manager instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BaselineManager* baselineManager);

    /**
     * @brief Check for changes in file
     * @param filePath File path to check
     * @return Detected change, or empty if no change
     */
    FileChange detectChange(const std::string& filePath);

    /**
     * @brief Get all detected changes
     * @param filters Optional filters
     * @return List of detected changes
     */
    std::vector<FileChange> getChanges(const std::vector<std::string>& filters = {}) const;

    /**
     * @brief Register change callback
     * @param callback Callback function
     */
    void registerChangeCallback(ChangeCallback callback);

    /**
     * @brief Add change to whitelist
     * @param filePath File path pattern (supports wildcards)
     * @param changeType Change type (empty for all types)
     * @param description Optional description
     * @return Whitelist entry ID
     */
    std::string addWhitelistEntry(const std::string& filePath,
                                  const std::string& changeType = "",
                                  const std::string& description = "");

    /**
     * @brief Remove whitelist entry
     * @param whitelistId Whitelist entry ID
     * @return true if removal successful, false otherwise
     */
    bool removeWhitelistEntry(const std::string& whitelistId);

    /**
     * @brief Get all whitelist entries
     * @return List of whitelist entries (JSON-like structure)
     */
    std::vector<std::map<std::string, std::string>> getWhitelistEntries() const;

    /**
     * @brief Check if change is whitelisted
     * @param change File change to check
     * @return true if whitelisted, false otherwise
     */
    bool isWhitelisted(const FileChange& change) const;

private:
    bool m_initialized;
    BaselineManager* m_baselineManager;
    ChangeCallback m_changeCallback;
    std::vector<FileChange> m_detectedChanges;

    /**
     * @brief Whitelist Entry
     */
    struct WhitelistEntry {
        std::string entry_id;
        std::string file_pattern;  // Supports wildcards (*, ?)
        std::string change_type;   // Empty for all types
        std::string description;
        std::string created_at;
    };

    std::map<std::string, WhitelistEntry> m_whitelistEntries;

    /**
     * @brief Classify change severity
     * @param change File change
     * @return Severity level
     */
    std::string classifySeverity(const FileChange& change) const;

    /**
     * @brief Match file path against pattern (supports wildcards)
     * @param pattern Pattern with wildcards
     * @param filePath File path to match
     * @return true if matches, false otherwise
     */
    bool matchPattern(const std::string& pattern, const std::string& filePath) const;

    /**
     * @brief Generate whitelist entry ID
     * @return Entry ID
     */
    std::string generateWhitelistId() const;
};

#endif // CHANGE_DETECTOR_H

