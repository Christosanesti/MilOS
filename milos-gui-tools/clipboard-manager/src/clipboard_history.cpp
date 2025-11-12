#include "clipboard_history.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <ctime>

ClipboardHistoryManager::ClipboardHistoryManager()
    : m_db(nullptr)
    , m_initialized(false)
{
}

ClipboardHistoryManager::~ClipboardHistoryManager() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool ClipboardHistoryManager::initialize(const std::string& dbPath) {
    if (m_initialized) {
        return true;
    }

    int rc = sqlite3_open(dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open clipboard history database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }

    if (!createSchema()) {
        std::cerr << "Failed to create clipboard history schema" << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }

    m_initialized = true;
    return true;
}

bool ClipboardHistoryManager::createSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS clipboard_entries (
            id TEXT PRIMARY KEY,
            encrypted_data BLOB NOT NULL,
            data_type TEXT NOT NULL,
            timestamp INTEGER NOT NULL,
            timeout INTEGER NOT NULL,
            is_active INTEGER NOT NULL DEFAULT 1
        );
        CREATE INDEX IF NOT EXISTS idx_timestamp ON clipboard_entries(timestamp);
        CREATE INDEX IF NOT EXISTS idx_is_active ON clipboard_entries(is_active);
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

bool ClipboardHistoryManager::addEntry(const ClipboardEntry& entry) {
    if (!m_initialized || !m_db) {
        return false;
    }

    const char* sql = R"(
        INSERT INTO clipboard_entries (id, encrypted_data, data_type, timestamp, timeout, is_active)
        VALUES (?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, entry.id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, entry.encrypted_data.data(), entry.encrypted_data.size(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, entry.data_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 4, entry.timestamp);
    sqlite3_bind_int64(stmt, 5, entry.timeout);
    sqlite3_bind_int(stmt, 6, entry.is_active ? 1 : 0);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert entry: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    return true;
}

std::vector<ClipboardEntry> ClipboardHistoryManager::getEntries(uint32_t limit) const {
    std::vector<ClipboardEntry> entries;

    if (!m_initialized || !m_db) {
        return entries;
    }

    const char* sql = R"(
        SELECT id, encrypted_data, data_type, timestamp, timeout, is_active
        FROM clipboard_entries
        WHERE is_active = 1
        ORDER BY timestamp DESC
        LIMIT ?
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return entries;
    }

    sqlite3_bind_int(stmt, 1, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ClipboardEntry entry;
        entry.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        entry.data_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        entry.timestamp = sqlite3_column_int64(stmt, 3);
        entry.timeout = sqlite3_column_int64(stmt, 4);
        entry.is_active = sqlite3_column_int(stmt, 5) != 0;

        const void* blob = sqlite3_column_blob(stmt, 1);
        int blobSize = sqlite3_column_bytes(stmt, 1);
        if (blob && blobSize > 0) {
            entry.encrypted_data.assign(
                static_cast<const uint8_t*>(blob),
                static_cast<const uint8_t*>(blob) + blobSize
            );
        }

        entries.push_back(entry);
    }

    sqlite3_finalize(stmt);
    return entries;
}

ClipboardEntry ClipboardHistoryManager::getEntry(const std::string& entryId) const {
    ClipboardEntry entry;

    if (!m_initialized || !m_db) {
        return entry;
    }

    const char* sql = R"(
        SELECT id, encrypted_data, data_type, timestamp, timeout, is_active
        FROM clipboard_entries
        WHERE id = ?
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return entry;
    }

    sqlite3_bind_text(stmt, 1, entryId.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        entry.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        entry.data_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        entry.timestamp = sqlite3_column_int64(stmt, 3);
        entry.timeout = sqlite3_column_int64(stmt, 4);
        entry.is_active = sqlite3_column_int(stmt, 5) != 0;

        const void* blob = sqlite3_column_blob(stmt, 1);
        int blobSize = sqlite3_column_bytes(stmt, 1);
        if (blob && blobSize > 0) {
            entry.encrypted_data.assign(
                static_cast<const uint8_t*>(blob),
                static_cast<const uint8_t*>(blob) + blobSize
            );
        }
    }

    sqlite3_finalize(stmt);
    return entry;
}

bool ClipboardHistoryManager::deleteEntry(const std::string& entryId) {
    if (!m_initialized || !m_db) {
        return false;
    }

    const char* sql = "DELETE FROM clipboard_entries WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, entryId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool ClipboardHistoryManager::clearAll() {
    if (!m_initialized || !m_db) {
        return false;
    }

    const char* sql = "DELETE FROM clipboard_entries";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

uint32_t ClipboardHistoryManager::cleanupExpired() {
    if (!m_initialized || !m_db) {
        return 0;
    }

    uint64_t currentTime = std::time(nullptr);

    const char* sql = R"(
        DELETE FROM clipboard_entries
        WHERE timestamp + timeout < ?
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int64(stmt, 1, currentTime);

    rc = sqlite3_step(stmt);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);

    return changes;
}

