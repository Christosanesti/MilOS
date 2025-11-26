#include "change_detector.h"
#include "baseline_manager.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <filesystem>
#include <algorithm>
#include <random>

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

        // Check if change is whitelisted
        if (isWhitelisted(change)) {
            // Return empty change (whitelisted changes are not reported)
            return FileChange();
        }

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
    if (filters.empty()) {
        return m_detectedChanges;
    }
    
    std::vector<FileChange> filteredChanges;
    
    // Parse filters: format is "key:value" (e.g., "file_path:/etc/*", "severity:high", "change_type:modified")
    std::map<std::string, std::string> filterMap;
    for (const auto& filter : filters) {
        size_t colonPos = filter.find(':');
        if (colonPos != std::string::npos) {
            std::string key = filter.substr(0, colonPos);
            std::string value = filter.substr(colonPos + 1);
            filterMap[key] = value;
        }
    }
    
    // Apply filters
    for (const auto& change : m_detectedChanges) {
        bool matches = true;
        
        // Filter by file path pattern
        if (filterMap.find("file_path") != filterMap.end()) {
            if (!matchPattern(filterMap.at("file_path"), change.file_path)) {
                matches = false;
            }
        }
        
        // Filter by change type
        if (matches && filterMap.find("change_type") != filterMap.end()) {
            std::string changeTypeStr;
            switch (change.change_type) {
                case ChangeType::MODIFIED: changeTypeStr = "modified"; break;
                case ChangeType::DELETED: changeTypeStr = "deleted"; break;
                case ChangeType::CREATED: changeTypeStr = "created"; break;
                case ChangeType::PERMISSIONS_CHANGED: changeTypeStr = "permissions_changed"; break;
                case ChangeType::OWNERSHIP_CHANGED: changeTypeStr = "ownership_changed"; break;
            }
            if (changeTypeStr != filterMap.at("change_type")) {
                matches = false;
            }
        }
        
        // Filter by severity
        if (matches && filterMap.find("severity") != filterMap.end()) {
            if (change.severity != filterMap.at("severity")) {
                matches = false;
            }
        }
        
        // Filter by baseline ID
        if (matches && filterMap.find("baseline_id") != filterMap.end()) {
            if (change.baseline_id != filterMap.at("baseline_id")) {
                matches = false;
            }
        }
        
        if (matches) {
            filteredChanges.push_back(change);
        }
    }
    
    return filteredChanges;
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

std::string ChangeDetector::addWhitelistEntry(const std::string& filePath,
                                              const std::string& changeType,
                                              const std::string& description) {
    WhitelistEntry entry;
    entry.entry_id = generateWhitelistId();
    entry.file_pattern = filePath;
    entry.change_type = changeType;
    entry.description = description;

    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    entry.created_at = timeStr;

    m_whitelistEntries[entry.entry_id] = entry;
    return entry.entry_id;
}

bool ChangeDetector::removeWhitelistEntry(const std::string& whitelistId) {
    auto it = m_whitelistEntries.find(whitelistId);
    if (it != m_whitelistEntries.end()) {
        m_whitelistEntries.erase(it);
        return true;
    }
    return false;
}

std::vector<std::map<std::string, std::string>> ChangeDetector::getWhitelistEntries() const {
    std::vector<std::map<std::string, std::string>> entries;
    for (const auto& [id, entry] : m_whitelistEntries) {
        std::map<std::string, std::string> entryMap;
        entryMap["entry_id"] = entry.entry_id;
        entryMap["file_pattern"] = entry.file_pattern;
        entryMap["change_type"] = entry.change_type;
        entryMap["description"] = entry.description;
        entryMap["created_at"] = entry.created_at;
        entries.push_back(entryMap);
    }
    return entries;
}

bool ChangeDetector::isWhitelisted(const FileChange& change) const {
    // Convert change type to string
    std::string changeTypeStr;
    switch (change.change_type) {
        case ChangeType::MODIFIED: changeTypeStr = "modified"; break;
        case ChangeType::DELETED: changeTypeStr = "deleted"; break;
        case ChangeType::CREATED: changeTypeStr = "created"; break;
        case ChangeType::PERMISSIONS_CHANGED: changeTypeStr = "permissions_changed"; break;
        case ChangeType::OWNERSHIP_CHANGED: changeTypeStr = "ownership_changed"; break;
    }

    // Check each whitelist entry
    for (const auto& [id, entry] : m_whitelistEntries) {
        // Check file pattern match
        if (!matchPattern(entry.file_pattern, change.file_path)) {
            continue;
        }

        // Check change type match (empty change_type means all types)
        if (!entry.change_type.empty() && entry.change_type != changeTypeStr) {
            continue;
        }

        // Match found - change is whitelisted
        return true;
    }

    return false;
}

bool ChangeDetector::matchPattern(const std::string& pattern, const std::string& filePath) const {
    // Simple wildcard matching: * matches any sequence, ? matches any single character
    size_t patternPos = 0;
    size_t filePos = 0;
    size_t patternLen = pattern.length();
    size_t fileLen = filePath.length();

    while (patternPos < patternLen && filePos < fileLen) {
        if (pattern[patternPos] == '*') {
            // Skip consecutive *s
            while (patternPos < patternLen && pattern[patternPos] == '*') {
                patternPos++;
            }
            // If * is at the end, match everything
            if (patternPos == patternLen) {
                return true;
            }
            // Try to match the rest of the pattern
            while (filePos < fileLen) {
                if (matchPattern(pattern.substr(patternPos), filePath.substr(filePos))) {
                    return true;
                }
                filePos++;
            }
            return false;
        } else if (pattern[patternPos] == '?' || pattern[patternPos] == filePath[filePos]) {
            patternPos++;
            filePos++;
        } else {
            return false;
        }
    }

    // Handle trailing *s
    while (patternPos < patternLen && pattern[patternPos] == '*') {
        patternPos++;
    }

    return patternPos == patternLen && filePos == fileLen;
}

std::string ChangeDetector::generateWhitelistId() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    ss << "whitelist-";
    for (int i = 0; i < 16; i++) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

