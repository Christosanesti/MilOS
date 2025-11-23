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

    BaselineInfo& baseline = it->second;
    
    // Recalculate hash
    std::string newHash = calculateFileHash(baseline.file_path);
    if (newHash.empty()) {
        return false;
    }

    // Update baseline
    baseline.file_hash = newHash;
    
    // Update file size
    struct stat fileStat;
    if (stat(baseline.file_path.c_str(), &fileStat) == 0) {
        baseline.file_size = fileStat.st_size;
    }

    // Save updated baseline
    saveBaseline(baseline);

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

