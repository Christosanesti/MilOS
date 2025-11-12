#include "backup_metadata.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>

BackupMetadataManager::BackupMetadataManager()
    : m_db(nullptr)
    , m_initialized(false)
{
}

BackupMetadataManager::~BackupMetadataManager() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool BackupMetadataManager::initialize(const std::string& dbPath) {
    if (m_initialized) {
        return true;
    }

    int rc = sqlite3_open(dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open backup metadata database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }

    if (!createSchema()) {
        std::cerr << "Failed to create backup metadata schema" << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }

    m_initialized = true;
    return true;
}

bool BackupMetadataManager::createSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS backup_metadata (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            destination_id TEXT NOT NULL,
            source_path TEXT NOT NULL,
            timestamp INTEGER NOT NULL,
            size INTEGER NOT NULL,
            encryption_algorithm TEXT NOT NULL,
            is_encrypted INTEGER NOT NULL DEFAULT 1,
            additional_info TEXT
        );
        CREATE INDEX IF NOT EXISTS idx_timestamp ON backup_metadata(timestamp);
        CREATE INDEX IF NOT EXISTS idx_destination ON backup_metadata(destination_id);
    )";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool BackupMetadataManager::addMetadata(const BackupMetadata& metadata) {
    if (!m_initialized || !m_db) {
        return false;
    }

    const char* sql = R"(
        INSERT INTO backup_metadata (id, name, destination_id, source_path, timestamp, size, encryption_algorithm, is_encrypted, additional_info)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, metadata.id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, metadata.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, metadata.destination_id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, metadata.source_path.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, metadata.timestamp);
    sqlite3_bind_int64(stmt, 6, metadata.size);
    sqlite3_bind_text(stmt, 7, metadata.encryption_algorithm.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, metadata.is_encrypted ? 1 : 0);
    
    // Serialize additional info as JSON (simplified)
    std::string additionalInfo = "";  // Would serialize metadata.additionalInfo
    sqlite3_bind_text(stmt, 9, additionalInfo.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert metadata: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    return true;
}

BackupMetadata BackupMetadataManager::getMetadata(const std::string& backupId) const {
    BackupMetadata metadata;

    if (!m_initialized || !m_db) {
        return metadata;
    }

    const char* sql = R"(
        SELECT id, name, destination_id, source_path, timestamp, size, encryption_algorithm, is_encrypted, additional_info
        FROM backup_metadata
        WHERE id = ?
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return metadata;
    }

    sqlite3_bind_text(stmt, 1, backupId.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        metadata.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        metadata.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        metadata.destination_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        metadata.source_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        metadata.timestamp = sqlite3_column_int64(stmt, 4);
        metadata.size = sqlite3_column_int64(stmt, 5);
        metadata.encryption_algorithm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        metadata.is_encrypted = sqlite3_column_int(stmt, 7) != 0;
    }

    sqlite3_finalize(stmt);
    return metadata;
}

std::vector<BackupMetadata> BackupMetadataManager::listBackups() const {
    std::vector<BackupMetadata> backups;

    if (!m_initialized || !m_db) {
        return backups;
    }

    const char* sql = R"(
        SELECT id, name, destination_id, source_path, timestamp, size, encryption_algorithm, is_encrypted, additional_info
        FROM backup_metadata
        ORDER BY timestamp DESC
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return backups;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        BackupMetadata metadata;
        metadata.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        metadata.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        metadata.destination_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        metadata.source_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        metadata.timestamp = sqlite3_column_int64(stmt, 4);
        metadata.size = sqlite3_column_int64(stmt, 5);
        metadata.encryption_algorithm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        metadata.is_encrypted = sqlite3_column_int(stmt, 7) != 0;
        backups.push_back(metadata);
    }

    sqlite3_finalize(stmt);
    return backups;
}

bool BackupMetadataManager::deleteMetadata(const std::string& backupId) {
    if (!m_initialized || !m_db) {
        return false;
    }

    const char* sql = "DELETE FROM backup_metadata WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, backupId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

