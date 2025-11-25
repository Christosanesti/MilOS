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
    std::string dbPath = m_storagePath + "/baselines.db";
    
    // Check if database exists
    if (!std::filesystem::exists(dbPath)) {
        // Create database and table
        sqlite3* db;
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Failed to create database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return;
        }
        
        std::string sql = R"(
            CREATE TABLE IF NOT EXISTS baselines (
                baseline_id TEXT PRIMARY KEY,
                file_path TEXT NOT NULL,
                hash_algorithm TEXT NOT NULL,
                file_hash TEXT NOT NULL,
                file_size INTEGER NOT NULL,
                permissions TEXT NOT NULL,
                owner TEXT NOT NULL,
                group_name TEXT NOT NULL,
                created_at TEXT NOT NULL,
                version TEXT NOT NULL,
                is_valid INTEGER NOT NULL DEFAULT 1
            )
        )";
        
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Failed to create baselines table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            sqlite3_close(db);
            return;
        }
        
        sqlite3_close(db);
        return; // Database created, no baselines to load yet
    }
    
    // Load baselines from database
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }
    
    // Load latest version of each baseline
    std::string sql = R"(
        SELECT baseline_id, file_path, hash_algorithm, file_hash, file_size,
               permissions, owner, group_name, created_at, version, is_valid
        FROM baselines
        WHERE (baseline_id, version) IN (
            SELECT baseline_id, MAX(version) 
            FROM baselines 
            GROUP BY baseline_id
        )
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            BaselineInfo baseline;
            baseline.baseline_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            baseline.file_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            baseline.hash_algorithm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            baseline.file_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            baseline.file_size = static_cast<size_t>(sqlite3_column_int64(stmt, 4));
            baseline.permissions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            baseline.owner = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            baseline.group = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            baseline.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            baseline.version = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            baseline.is_valid = (sqlite3_column_int(stmt, 10) != 0);
            
            m_baselines[baseline.baseline_id] = baseline;
            m_fileToBaseline[baseline.file_path] = baseline.baseline_id;
        }
        sqlite3_finalize(stmt);
    }
    
    // Load all versions for versioning support
    sql = "SELECT baseline_id, file_path, hash_algorithm, file_hash, file_size, "
          "permissions, owner, group_name, created_at, version, is_valid "
          "FROM baselines ORDER BY baseline_id, version";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            BaselineInfo baseline;
            baseline.baseline_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            baseline.file_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            baseline.hash_algorithm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            baseline.file_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            baseline.file_size = static_cast<size_t>(sqlite3_column_int64(stmt, 4));
            baseline.permissions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            baseline.owner = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            baseline.group = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            baseline.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            baseline.version = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            baseline.is_valid = (sqlite3_column_int(stmt, 10) != 0);
            
            m_baselineVersions[baseline.baseline_id].push_back(baseline);
        }
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    std::cout << "Loaded " << m_baselines.size() << " baselines from database" << std::endl;
}

void BaselineManager::saveBaseline(const BaselineInfo& baseline) {
    std::string dbPath = m_storagePath + "/baselines.db";
    
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }
    
    std::string sql = R"(
        INSERT OR REPLACE INTO baselines 
        (baseline_id, file_path, hash_algorithm, file_hash, file_size, 
         permissions, owner, group_name, created_at, version, is_valid)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, baseline.baseline_id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, baseline.file_path.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, baseline.hash_algorithm.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, baseline.file_hash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(baseline.file_size));
        sqlite3_bind_text(stmt, 6, baseline.permissions.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, baseline.owner.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, baseline.group.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 9, baseline.created_at.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 10, baseline.version.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 11, baseline.is_valid ? 1 : 0);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to save baseline: " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << "Baseline saved: " << baseline.baseline_id << " for " << baseline.file_path << std::endl;
        }
        
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    
    // Update in-memory cache
    m_baselines[baseline.baseline_id] = baseline;
    m_fileToBaseline[baseline.file_path] = baseline.baseline_id;
    m_baselineVersions[baseline.baseline_id].push_back(baseline);
}

