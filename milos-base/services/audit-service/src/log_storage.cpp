#include "log_storage.h"
#include "config_parser.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>

LogStorage::LogStorage()
    : m_initialized(false)
    , m_configParser(nullptr)
    , m_database(nullptr)
{
}

LogStorage::~LogStorage() {
    cleanupDatabase();
}

bool LogStorage::initialize(ConfigParser* configParser) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;

    if (!initializeDatabase()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return false;
    }

    if (!createSchema()) {
        std::cerr << "Failed to create database schema" << std::endl;
        cleanupDatabase();
        return false;
    }

    m_initialized = true;
    return true;
}

bool LogStorage::initializeDatabase() {
    if (!m_configParser || !m_configParser->isLoaded()) {
        std::cerr << "Configuration not loaded" << std::endl;
        return false;
    }

    // Get database type and path from configuration
    std::string dbType = m_configParser->getString("service.database_type");
    if (dbType.empty()) {
        dbType = "sqlite";
    }

    if (dbType == "sqlite") {
        std::string dbPath = m_configParser->getString("service.sqlite_path");
        if (dbPath.empty()) {
            dbPath = "/var/lib/milos/audit-service/audit.db";
        }

        // Create directory if it doesn't exist
        std::string dirPath = dbPath.substr(0, dbPath.find_last_of('/'));
        if (!dirPath.empty()) {
            std::filesystem::create_directories(dirPath);
        }

        sqlite3* db = nullptr;
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return false;
        }

        m_database = db;
        std::cout << "SQLite database opened: " << dbPath << std::endl;
        return true;
    } else if (dbType == "postgresql") {
        // PostgreSQL support is planned for enterprise deployments
        // Requires libpq (PostgreSQL client library) integration
        // This would provide better scalability and advanced querying capabilities
        std::cerr << "PostgreSQL support not yet implemented. Use SQLite for now." << std::endl;
        return false;
    } else {
        std::cerr << "Unknown database type: " << dbType << std::endl;
        return false;
    }
}

bool LogStorage::createSchema() {
    if (!m_database) {
        return false;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS audit_logs (
            log_id TEXT PRIMARY KEY,
            event_type TEXT NOT NULL,
            event_severity TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            user_id TEXT,
            application_name TEXT NOT NULL,
            event_description TEXT,
            event_data TEXT,
            ip_address TEXT,
            file_paths TEXT,
            policy_id TEXT,
            action_taken TEXT,
            log_hash TEXT NOT NULL,
            previous_log_hash TEXT,
            created_at INTEGER DEFAULT (strftime('%s', 'now'))
        );

        CREATE INDEX IF NOT EXISTS idx_timestamp ON audit_logs(timestamp);
        CREATE INDEX IF NOT EXISTS idx_event_type ON audit_logs(event_type);
        CREATE INDEX IF NOT EXISTS idx_user_id ON audit_logs(user_id);
        CREATE INDEX IF NOT EXISTS idx_log_hash ON audit_logs(log_hash);
    )";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool LogStorage::storeLogEntry(const AuditLogEntry& entry) {
    if (!m_database || !m_initialized) {
        return false;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);

    // Convert file_paths vector to JSON array string
    std::ostringstream filePathsJson;
    filePathsJson << "[";
    for (size_t i = 0; i < entry.file_paths.size(); ++i) {
        if (i > 0) filePathsJson << ",";
        filePathsJson << "\"" << entry.file_paths[i] << "\"";
    }
    filePathsJson << "]";

    const char* insertSQL = R"(
        INSERT INTO audit_logs (
            log_id, event_type, event_severity, timestamp,
            user_id, application_name, event_description, event_data,
            ip_address, file_paths, policy_id, action_taken,
            log_hash, previous_log_hash
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, entry.log_id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, entry.event_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, entry.event_severity.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, entry.timestamp.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, entry.user_id.empty() ? nullptr : entry.user_id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, entry.application_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, entry.event_description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, entry.event_data.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, entry.ip_address.empty() ? nullptr : entry.ip_address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, filePathsJson.str().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, entry.policy_id.empty() ? nullptr : entry.policy_id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, entry.action_taken.empty() ? nullptr : entry.action_taken.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, entry.log_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 14, entry.previous_log_hash.empty() ? nullptr : entry.previous_log_hash.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert log entry: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

std::vector<AuditLogEntry> LogStorage::queryLogEntries(
    const std::map<std::string, std::string>& filters,
    int limit,
    int offset
) {
    std::vector<AuditLogEntry> entries;

    if (!m_database || !m_initialized) {
        return entries;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);

    // Build query with filters
    std::ostringstream query;
    query << "SELECT log_id, event_type, event_severity, timestamp, user_id, "
          << "application_name, event_description, event_data, ip_address, "
          << "file_paths, policy_id, action_taken, log_hash, previous_log_hash "
          << "FROM audit_logs WHERE 1=1";

    // Add filters
    for (const auto& filter : filters) {
        if (filter.first == "event_type") {
            query << " AND event_type = ?";
        } else if (filter.first == "event_severity") {
            query << " AND event_severity = ?";
        } else if (filter.first == "user_id") {
            query << " AND user_id = ?";
        } else if (filter.first == "timestamp_from") {
            query << " AND timestamp >= ?";
        } else if (filter.first == "timestamp_to") {
            query << " AND timestamp <= ?";
        }
    }

    query << " ORDER BY timestamp DESC LIMIT ? OFFSET ?";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare query: " << sqlite3_errmsg(db) << std::endl;
        return entries;
    }

    // Bind filter parameters
    int paramIndex = 1;
    for (const auto& filter : filters) {
        if (filter.first == "event_type" || filter.first == "event_severity" || 
            filter.first == "user_id" || filter.first == "timestamp_from" || 
            filter.first == "timestamp_to") {
            sqlite3_bind_text(stmt, paramIndex++, filter.second.c_str(), -1, SQLITE_STATIC);
        }
    }

    sqlite3_bind_int(stmt, paramIndex++, limit);
    sqlite3_bind_int(stmt, paramIndex++, offset);

    // Execute query and collect results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        AuditLogEntry entry;
        entry.log_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        entry.event_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.event_severity = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        entry.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        if (userId) entry.user_id = userId;
        entry.application_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        if (desc) entry.event_description = desc;
        const char* data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        if (data) entry.event_data = data;
        const char* ip = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        if (ip) entry.ip_address = ip;
        
        // Parse file_paths JSON array
        const char* filePathsJson = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        if (filePathsJson && strlen(filePathsJson) > 0) {
            // Simple JSON array parsing: ["path1", "path2", ...]
            std::string jsonStr = filePathsJson;
            // Remove brackets
            if (jsonStr.front() == '[' && jsonStr.back() == ']') {
                jsonStr = jsonStr.substr(1, jsonStr.length() - 2);
            }
            // Split by comma and remove quotes
            std::istringstream iss(jsonStr);
            std::string path;
            while (std::getline(iss, path, ',')) {
                // Remove quotes and whitespace
                path.erase(std::remove(path.begin(), path.end(), '"'), path.end());
                path.erase(0, path.find_first_not_of(" \t"));
                path.erase(path.find_last_not_of(" \t") + 1);
                if (!path.empty()) {
                    entry.file_paths.push_back(path);
                }
            }
        }
        
        const char* policyId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        if (policyId) entry.policy_id = policyId;
        const char* action = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (action) entry.action_taken = action;
        entry.log_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        const char* prevHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
        if (prevHash) entry.previous_log_hash = prevHash;

        entries.push_back(entry);
    }

    sqlite3_finalize(stmt);
    return entries;
}

std::unique_ptr<AuditLogEntry> LogStorage::getLogEntry(const std::string& logId) {
    if (!m_database || !m_initialized) {
        return nullptr;
    }

    std::map<std::string, std::string> filters;
    filters["log_id"] = logId;

    std::vector<AuditLogEntry> entries = queryLogEntries(filters, 1, 0);
    if (entries.empty()) {
        return nullptr;
    }

    return std::make_unique<AuditLogEntry>(entries[0]);
}

std::map<std::string, int> LogStorage::getEventStatistics(
    const std::map<std::string, std::string>& filters
) {
    std::map<std::string, int> stats;

    if (!m_database || !m_initialized) {
        return stats;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);

    // Build statistics query
    std::ostringstream query;
    query << "SELECT event_type, COUNT(*) as count FROM audit_logs WHERE 1=1";

    // Add filters
    for (const auto& filter : filters) {
        if (filter.first == "timestamp_from") {
            query << " AND timestamp >= ?";
        } else if (filter.first == "timestamp_to") {
            query << " AND timestamp <= ?";
        }
    }

    query << " GROUP BY event_type";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statistics query: " << sqlite3_errmsg(db) << std::endl;
        return stats;
    }

    // Bind filter parameters
    int paramIndex = 1;
    for (const auto& filter : filters) {
        if (filter.first == "timestamp_from" || filter.first == "timestamp_to") {
            sqlite3_bind_text(stmt, paramIndex++, filter.second.c_str(), -1, SQLITE_STATIC);
        }
    }

    // Execute query and collect statistics
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string eventType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int count = sqlite3_column_int(stmt, 1);
        stats[eventType] = count;
    }

    sqlite3_finalize(stmt);
    return stats;
}

int LogStorage::enforceRetentionPolicies() {
    if (!m_database || !m_initialized || !m_configParser) {
        return 0;
    }

    int retentionDays = m_configParser->getInt("retention.retention_days", 365);
    
    // Calculate cutoff date
    std::time_t now = std::time(nullptr);
    std::time_t cutoff = now - (retentionDays * 24 * 60 * 60);

    sqlite3* db = static_cast<sqlite3*>(m_database);

    const char* deleteSQL = "DELETE FROM audit_logs WHERE created_at < ?";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, deleteSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare retention query: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    }

    sqlite3_bind_int64(stmt, 1, cutoff);
    rc = sqlite3_step(stmt);
    int deletedCount = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to enforce retention policies: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    }

    return deletedCount;
}

bool LogStorage::isHealthy() const {
    if (!m_database || !m_initialized) {
        return false;
    }

    // Check database connection
    sqlite3* db = static_cast<sqlite3*>(m_database);
    int rc = sqlite3_exec(db, "SELECT 1", nullptr, nullptr, nullptr);
    return rc == SQLITE_OK;
}

std::string LogStorage::getLastLogHash() const {
    if (!m_database || !m_initialized) {
        return "";
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    const char* querySQL = "SELECT log_hash FROM audit_logs ORDER BY timestamp DESC LIMIT 1";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, querySQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return "";
    }

    std::string lastHash;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (hash) {
            lastHash = hash;
        }
    }

    sqlite3_finalize(stmt);
    return lastHash;
}

void LogStorage::cleanupDatabase() {
    if (m_database) {
        sqlite3_close(static_cast<sqlite3*>(m_database));
        m_database = nullptr;
    }
}

