#ifndef CLIPBOARD_HISTORY_H
#define CLIPBOARD_HISTORY_H

#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>

/**
 * @brief Clipboard History Entry
 */
struct ClipboardEntry {
    std::string id;
    std::vector<uint8_t> encrypted_data;
    std::string data_type;  // "text", "image", "file"
    uint64_t timestamp;
    uint64_t timeout;
    bool is_active;
};

/**
 * @brief Clipboard History Manager
 * 
 * Manages encrypted clipboard history storage using SQLite.
 */
class ClipboardHistoryManager {
public:
    ClipboardHistoryManager();
    ~ClipboardHistoryManager();

    /**
     * @brief Initialize history manager
     * @param dbPath Path to SQLite database
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& dbPath);

    /**
     * @brief Add clipboard entry
     * @param entry Clipboard entry to add
     * @return true if add successful, false otherwise
     */
    bool addEntry(const ClipboardEntry& entry);

    /**
     * @brief Get clipboard entries
     * @param limit Maximum number of entries to retrieve
     * @return Vector of clipboard entries
     */
    std::vector<ClipboardEntry> getEntries(uint32_t limit = 50) const;

    /**
     * @brief Get clipboard entry by ID
     * @param entryId Entry ID
     * @return Clipboard entry if found, empty entry otherwise
     */
    ClipboardEntry getEntry(const std::string& entryId) const;

    /**
     * @brief Delete clipboard entry
     * @param entryId Entry ID to delete
     * @return true if delete successful, false otherwise
     */
    bool deleteEntry(const std::string& entryId);

    /**
     * @brief Clear all clipboard entries
     * @return true if clear successful, false otherwise
     */
    bool clearAll();

    /**
     * @brief Cleanup expired entries
     * @return Number of entries removed
     */
    uint32_t cleanupExpired();

private:
    sqlite3* m_db;
    bool m_initialized;

    /**
     * @brief Create database schema
     */
    bool createSchema();
};

#endif // CLIPBOARD_HISTORY_H

