#ifndef CHANGE_DETECTOR_H
#define CHANGE_DETECTOR_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

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

private:
    bool m_initialized;
    BaselineManager* m_baselineManager;
    ChangeCallback m_changeCallback;
    std::vector<FileChange> m_detectedChanges;

    /**
     * @brief Classify change severity
     * @param change File change
     * @return Severity level
     */
    std::string classifySeverity(const FileChange& change) const;
};

#endif // CHANGE_DETECTOR_H

