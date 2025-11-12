#ifndef BACKUP_METADATA_H
#define BACKUP_METADATA_H

#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>

/**
 * @brief Backup Metadata Entry
 */
struct BackupMetadata {
    std::string id;
    std::string name;
    std::string destination_id;
    std::string source_path;
    uint64_t timestamp;
    uint64_t size;
    std::string encryption_algorithm;
    bool is_encrypted;
    std::map<std::string, std::string> additionalInfo;
};

/**
 * @brief Backup Metadata Manager
 * 
 * Manages backup metadata storage using SQLite.
 */
class BackupMetadataManager {
public:
    BackupMetadataManager();
    ~BackupMetadataManager();

    /**
     * @brief Initialize metadata manager
     * @param dbPath Path to SQLite database
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& dbPath);

    /**
     * @brief Add backup metadata
     * @param metadata Backup metadata
     * @return true if add successful, false otherwise
     */
    bool addMetadata(const BackupMetadata& metadata);

    /**
     * @brief Get backup metadata
     * @param backupId Backup ID
     * @return Backup metadata if found, empty metadata otherwise
     */
    BackupMetadata getMetadata(const std::string& backupId) const;

    /**
     * @brief List all backups
     * @return Vector of backup metadata
     */
    std::vector<BackupMetadata> listBackups() const;

    /**
     * @brief Delete backup metadata
     * @param backupId Backup ID to delete
     * @return true if delete successful, false otherwise
     */
    bool deleteMetadata(const std::string& backupId);

private:
    sqlite3* m_db;
    bool m_initialized;

    /**
     * @brief Create database schema
     */
    bool createSchema();
};

#endif // BACKUP_METADATA_H

