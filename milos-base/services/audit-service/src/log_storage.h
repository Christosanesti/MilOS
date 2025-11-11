#ifndef LOG_STORAGE_H
#define LOG_STORAGE_H

#include <memory>
#include <string>
#include <vector>
#include <map>

class ConfigParser;

/**
 * @brief Audit Log Entry Structure
 */
struct AuditLogEntry {
    std::string log_id;           // UUID
    std::string event_type;        // ENCRYPTION, NETWORK_TRANSMISSION, etc.
    std::string event_severity;    // INFO, WARNING, ERROR, CRITICAL
    std::string timestamp;         // ISO 8601 timestamp
    std::string user_id;            // Optional
    std::string application_name;
    std::string event_description;
    std::string event_data;        // JSON string
    std::string ip_address;         // Optional
    std::vector<std::string> file_paths;  // Optional
    std::string policy_id;          // Optional
    std::string action_taken;       // Optional
    std::string log_hash;           // SHA-256 hash of this entry
    std::string previous_log_hash;  // Hash of previous entry
};

/**
 * @brief Log Storage
 * 
 * Manages audit log storage in SQLite database with support
 * for PostgreSQL (enterprise). Handles log retention policies.
 */
class LogStorage {
public:
    LogStorage();
    ~LogStorage();

    /**
     * @brief Initialize log storage
     * @param configParser Configuration parser instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser);

    /**
     * @brief Store audit log entry
     * @param entry Audit log entry to store
     * @return true if stored successfully, false otherwise
     */
    bool storeLogEntry(const AuditLogEntry& entry);

    /**
     * @brief Query audit log entries
     * @param filters Query filters (timestamp range, event type, etc.)
     * @param limit Maximum number of results
     * @param offset Result offset for pagination
     * @return Vector of audit log entries
     */
    std::vector<AuditLogEntry> queryLogEntries(
        const std::map<std::string, std::string>& filters,
        int limit = 100,
        int offset = 0
    );

    /**
     * @brief Get log entry by ID
     * @param logId Log entry ID
     * @return Audit log entry if found, nullptr otherwise
     */
    std::unique_ptr<AuditLogEntry> getLogEntry(const std::string& logId);

    /**
     * @brief Get event statistics
     * @param filters Query filters
     * @return Statistics map (event_type -> count)
     */
    std::map<std::string, int> getEventStatistics(
        const std::map<std::string, std::string>& filters
    );

    /**
     * @brief Enforce log retention policies
     * @return Number of entries removed
     */
    int enforceRetentionPolicies();

    /**
     * @brief Check if log storage is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

    /**
     * @brief Get last log entry hash (for hash chain)
     * @return Hash of last log entry, empty if no entries
     */
    std::string getLastLogHash() const;

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    void* m_database;  // SQLite database handle or PostgreSQL connection

    /**
     * @brief Initialize database connection
     * @return true if successful, false otherwise
     */
    bool initializeDatabase();

    /**
     * @brief Create database schema
     * @return true if successful, false otherwise
     */
    bool createSchema();

    /**
     * @brief Cleanup database connection
     */
    void cleanupDatabase();
};

#endif // LOG_STORAGE_H

