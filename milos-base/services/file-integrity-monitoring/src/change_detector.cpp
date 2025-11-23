#include "change_detector.h"
#include "baseline_manager.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <filesystem>

ChangeDetector::ChangeDetector()
    : m_initialized(false)
    , m_baselineManager(nullptr)
{
}

ChangeDetector::~ChangeDetector() {
}

bool ChangeDetector::initialize(BaselineManager* baselineManager) {
    if (m_initialized) {
        return true;
    }

    if (!baselineManager) {
        return false;
    }

    m_baselineManager = baselineManager;
    m_initialized = true;
    return true;
}

FileChange ChangeDetector::detectChange(const std::string& filePath) {
    FileChange change;
    change.file_path = filePath;

    if (!m_baselineManager) {
        return change;
    }

    // Get baseline for file
    auto baseline = m_baselineManager->getBaselineForFile(filePath);
    if (baseline.baseline_id.empty()) {
        // No baseline exists - file was created
        change.change_type = ChangeType::CREATED;
        change.severity = "low";
        change.description = "New file detected (no baseline)";
    } else {
        // Check if file still exists
        if (!std::filesystem::exists(filePath)) {
            change.change_type = ChangeType::DELETED;
            change.baseline_id = baseline.baseline_id;
            change.old_hash = baseline.file_hash;
            change.severity = "high";
            change.description = "File deleted";
        } else {
            // File exists - check for modifications
            std::string currentHash = m_baselineManager->getBaselineForFile(filePath).file_hash;
            if (currentHash != baseline.file_hash) {
                change.change_type = ChangeType::MODIFIED;
                change.baseline_id = baseline.baseline_id;
                change.old_hash = baseline.file_hash;
                change.new_hash = currentHash;
                change.severity = "medium";
                change.description = "File content modified";
            } else {
                // Check for permission/ownership changes
                struct stat fileStat;
                if (stat(filePath.c_str(), &fileStat) == 0) {
                    std::stringstream permStream;
                    permStream << std::oct << (fileStat.st_mode & 0777);
                    std::string currentPerms = permStream.str();
                    
                    if (currentPerms != baseline.permissions) {
                        change.change_type = ChangeType::PERMISSIONS_CHANGED;
                        change.baseline_id = baseline.baseline_id;
                        change.severity = "medium";
                        change.description = "File permissions changed";
                    }
                }
            }
        }
    }

    // Generate change ID
    if (!change.file_path.empty()) {
        std::stringstream ss;
        ss << std::hex << std::time(nullptr) << "-" << filePath;
        change.change_id = ss.str();

        // Format detection time
        char timeStr[64];
        std::time_t now = std::time(nullptr);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        change.detected_at = timeStr;

        // Classify severity
        change.severity = classifySeverity(change);

        // Store change
        m_detectedChanges.push_back(change);

        // Call callback if registered
        if (m_changeCallback) {
            m_changeCallback(change);
        }
    }

    return change;
}

std::vector<FileChange> ChangeDetector::getChanges(const std::vector<std::string>& filters) const {
    // TODO: Apply filters
    return m_detectedChanges;
}

void ChangeDetector::registerChangeCallback(ChangeCallback callback) {
    m_changeCallback = callback;
}

std::string ChangeDetector::classifySeverity(const FileChange& change) const {
    // Simple severity classification
    switch (change.change_type) {
        case ChangeType::DELETED:
            return "high";
        case ChangeType::MODIFIED:
            return "medium";
        case ChangeType::PERMISSIONS_CHANGED:
            return "medium";
        case ChangeType::OWNERSHIP_CHANGED:
            return "high";
        case ChangeType::CREATED:
            return "low";
        default:
            return "low";
    }
}

