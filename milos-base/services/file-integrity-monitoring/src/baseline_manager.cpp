#include "baseline_manager.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <filesystem>
#include <sqlite3.h>
#include <iostream>

BaselineManager::BaselineManager()
    : m_initialized(false)
    , m_storagePath("/var/lib/milos/fim/baselines")
    , m_hashAlgorithm("sha256")
{
}

BaselineManager::~BaselineManager() {
}

bool BaselineManager::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create storage directory if it doesn't exist
    std::filesystem::create_directories(m_storagePath);

    // Load existing baselines
    loadBaselines();

    m_initialized = true;
    return true;
}

std::vector<std::string> BaselineManager::createBaseline(const std::vector<std::string>& filePaths) {
    std::vector<std::string> baselineIds;

    for (const auto& filePath : filePaths) {
        // Check if file exists
        if (!std::filesystem::exists(filePath)) {
            std::cerr << "File does not exist: " << filePath << std::endl;
            continue;
        }

        // Calculate file hash
        std::string fileHash = calculateFileHash(filePath);
        if (fileHash.empty()) {
            std::cerr << "Failed to calculate hash for: " << filePath << std::endl;
            continue;
        }

        // Get file metadata
        struct stat fileStat;
        if (stat(filePath.c_str(), &fileStat) != 0) {
            std::cerr << "Failed to get file stats: " << filePath << std::endl;
            continue;
        }

        // Get owner and group
        struct passwd* pw = getpwuid(fileStat.st_uid);
        struct group* gr = getgrgid(fileStat.st_gid);
        
        std::string owner = pw ? pw->pw_name : std::to_string(fileStat.st_uid);
        std::string group = gr ? gr->gr_name : std::to_string(fileStat.st_gid);

        // Create baseline ID (UUID-like)
        std::stringstream ss;
        ss << std::hex << std::time(nullptr) << "-" << fileStat.st_ino;
        std::string baselineId = ss.str();

        // Create baseline info
        BaselineInfo baseline;
        baseline.baseline_id = baselineId;
        baseline.file_path = filePath;
        baseline.hash_algorithm = m_hashAlgorithm;
        baseline.file_hash = fileHash;
        baseline.file_size = fileStat.st_size;
        
        // Format permissions
        std::stringstream permStream;
        permStream << std::oct << (fileStat.st_mode & 0777);
        baseline.permissions = permStream.str();
        
        baseline.owner = owner;
        baseline.group = group;
        
        // Format creation time
        char timeStr[64];
        std::time_t now = std::time(nullptr);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        baseline.created_at = timeStr;
        
        baseline.version = "1.0.0";
        baseline.is_valid = true;

        // Initialize version history
        m_baselineVersions[baselineId].push_back(baseline);

        // Store baseline
        m_baselines[baselineId] = baseline;
        m_fileToBaseline[filePath] = baselineId;

        // Save to storage
        saveBaseline(baseline);

        baselineIds.push_back(baselineId);
    }

    return baselineIds;
}

BaselineInfo BaselineManager::getBaseline(const std::string& baselineId) const {
    auto it = m_baselines.find(baselineId);
    if (it != m_baselines.end()) {
        return it->second;
    }
    return BaselineInfo();  // Return empty baseline
}

std::string BaselineManager::getBaselineStatus(const std::string& baselineId) const {
    auto it = m_baselines.find(baselineId);
    if (it == m_baselines.end()) {
        return "not_found";
    }

    if (!it->second.is_valid) {
        return "invalid";
    }

    // Check if file still exists and matches
    if (std::filesystem::exists(it->second.file_path)) {
        std::string currentHash = calculateFileHash(it->second.file_path);
        if (currentHash == it->second.file_hash) {
            return "valid";
        } else {
            return "modified";
        }
    } else {
        return "deleted";
    }
}

bool BaselineManager::updateBaseline(const std::string& baselineId) {
    auto it = m_baselines.find(baselineId);
    if (it == m_baselines.end()) {
        return false;
    }

    BaselineInfo& currentBaseline = it->second;
    
    // Recalculate hash
    std::string newHash = calculateFileHash(currentBaseline.file_path);
    if (newHash.empty()) {
        return false;
    }

    // Create new version (increment version number)
    BaselineInfo newVersion = currentBaseline;
    
    // Parse current version and increment
    std::string currentVersion = currentBaseline.version;
    size_t dotPos = currentVersion.find_last_of('.');
    if (dotPos != std::string::npos) {
        int minorVersion = std::stoi(currentVersion.substr(dotPos + 1));
        newVersion.version = currentVersion.substr(0, dotPos + 1) + std::to_string(minorVersion + 1);
    } else {
        // If no dot, assume it's a major version
        int majorVersion = std::stoi(currentVersion);
        newVersion.version = std::to_string(majorVersion + 1) + ".0.0";
    }

    // Update baseline data
    newVersion.file_hash = newHash;
    
    // Update file size
    struct stat fileStat;
    if (stat(newVersion.file_path.c_str(), &fileStat) == 0) {
        newVersion.file_size = fileStat.st_size;
    }

    // Update creation time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    newVersion.created_at = timeStr;

    // Store new version in history
    m_baselineVersions[baselineId].push_back(newVersion);

    // Update current baseline
    m_baselines[baselineId] = newVersion;

    // Save updated baseline
    saveBaseline(newVersion);

    return true;
}

bool BaselineManager::verifyBaseline(const std::string& baselineId) const {
    auto it = m_baselines.find(baselineId);
    if (it == m_baselines.end()) {
        return false;
    }

    const BaselineInfo& baseline = it->second;
    
    if (!std::filesystem::exists(baseline.file_path)) {
        return false;
    }

    std::string currentHash = calculateFileHash(baseline.file_path);
    return currentHash == baseline.file_hash;
}

BaselineInfo BaselineManager::getBaselineForFile(const std::string& filePath) const {
    auto it = m_fileToBaseline.find(filePath);
    if (it != m_fileToBaseline.end()) {
        return getBaseline(it->second);
    }
    return BaselineInfo();  // Return empty baseline
}

std::vector<BaselineInfo> BaselineManager::getBaselineVersions(const std::string& baselineId) const {
    auto it = m_baselineVersions.find(baselineId);
    if (it != m_baselineVersions.end()) {
        return it->second;  // Return all versions (already in chronological order)
    }
    return std::vector<BaselineInfo>();  // Return empty if not found
}

BaselineInfo BaselineManager::getBaselineVersion(const std::string& baselineId, const std::string& version) const {
    auto it = m_baselineVersions.find(baselineId);
    if (it != m_baselineVersions.end()) {
        for (const auto& baseline : it->second) {
            if (baseline.version == version) {
                return baseline;
            }
        }
    }
    return BaselineInfo();  // Return empty if not found
}

bool BaselineManager::rollbackBaseline(const std::string& baselineId, const std::string& version) {
    auto versionsIt = m_baselineVersions.find(baselineId);
    if (versionsIt == m_baselineVersions.end()) {
        return false;
    }

    // Find the version to rollback to
    BaselineInfo targetVersion;
    bool found = false;
    for (const auto& baseline : versionsIt->second) {
        if (baseline.version == version) {
            targetVersion = baseline;
            found = true;
            break;
        }
    }

    if (!found) {
        return false;
    }

    // Create a new version from the target version (rollback creates a new version)
    BaselineInfo rollbackVersion = targetVersion;
    
    // Update version number (increment from current)
    auto currentIt = m_baselines.find(baselineId);
    if (currentIt != m_baselines.end()) {
        std::string currentVersion = currentIt->second.version;
        size_t dotPos = currentVersion.find_last_of('.');
        if (dotPos != std::string::npos) {
            int minorVersion = std::stoi(currentVersion.substr(dotPos + 1));
            rollbackVersion.version = currentVersion.substr(0, dotPos + 1) + std::to_string(minorVersion + 1);
        } else {
            int majorVersion = std::stoi(currentVersion);
            rollbackVersion.version = std::to_string(majorVersion + 1) + ".0.0";
        }
    }

    // Update creation time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    rollbackVersion.created_at = timeStr;

    // Add rollback version to history
    versionsIt->second.push_back(rollbackVersion);

    // Update current baseline
    m_baselines[baselineId] = rollbackVersion;

    // Save rollback baseline
    saveBaseline(rollbackVersion);

    return true;
}

std::string BaselineManager::calculateFileHash(const std::string& filePath) const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        return "";
    }

    const EVP_MD* md = EVP_sha256();
    if (EVP_DigestInit_ex(mdctx, md, nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    // Read file in chunks
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        if (EVP_DigestUpdate(mdctx, buffer, file.gcount()) != 1) {
            EVP_MD_CTX_free(mdctx);
            return "";
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    if (EVP_DigestFinal_ex(mdctx, hash, &hashLen) != 1) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

    // Convert to hex string
    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

void BaselineManager::loadBaselines() {
    // TODO: Load baselines from SQLite database
    // For now, baselines are created fresh on each run
}

void BaselineManager::saveBaseline(const BaselineInfo& baseline) {
    // TODO: Save baseline to SQLite database
    // For now, baselines are kept in memory
    std::cout << "Baseline saved: " << baseline.baseline_id << " for " << baseline.file_path << std::endl;
}

