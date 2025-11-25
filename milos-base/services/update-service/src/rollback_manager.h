#ifndef ROLLBACK_MANAGER_H
#define ROLLBACK_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <map>

class ConfigParser;
class PackageManager;

/**
 * @brief Rollback Snapshot
 */
struct RollbackSnapshot {
    std::string snapshot_id;
    std::string update_id;
    std::string timestamp;
    std::map<std::string, std::string> package_versions;  // package_name -> version
    std::string snapshot_path;
};

/**
 * @brief Rollback Manager
 * 
 * Manages rollback snapshots and rollback operations.
 */
class RollbackManager {
public:
    RollbackManager();
    ~RollbackManager();

    /**
     * @brief Initialize rollback manager
     * @param configParser Configuration parser instance
     * @param packageManager Package manager instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser, PackageManager* packageManager);

    /**
     * @brief Create rollback snapshot
     * @param updateId Update ID
     * @param packageList List of packages to snapshot
     * @return Snapshot ID if successful, empty string otherwise
     */
    std::string createSnapshot(const std::string& updateId, const std::vector<std::string>& packageList);

    /**
     * @brief Rollback to snapshot
     * @param snapshotId Snapshot ID
     * @return true if rollback successful, false otherwise
     */
    bool rollbackToSnapshot(const std::string& snapshotId);

    /**
     * @brief Get snapshot information
     * @param snapshotId Snapshot ID
     * @return Snapshot information, nullptr if not found
     */
    std::unique_ptr<RollbackSnapshot> getSnapshot(const std::string& snapshotId);

    /**
     * @brief Get all snapshots
     * @return Vector of snapshot information
     */
    std::vector<RollbackSnapshot> getAllSnapshots();

    /**
     * @brief Cleanup old snapshots
     * @param maxSnapshots Maximum number of snapshots to keep
     * @return Number of snapshots removed
     */
    int cleanupOldSnapshots(int maxSnapshots);

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    PackageManager* m_packageManager;
    std::string m_snapshotPath;

    /**
     * @brief Load snapshots from storage
     */
    void loadSnapshots();

    /**
     * @brief Save snapshot to storage
     * @param snapshot Snapshot to save
     */
    void saveSnapshot(const RollbackSnapshot& snapshot);

    /**
     * @brief Delete snapshot from storage
     * @param snapshotId Snapshot ID to delete
     */
    void deleteSnapshot(const std::string& snapshotId);

    /**
     * @brief Initialize SQLite database for snapshot storage
     * @param dbPath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initializeDatabase(const std::string& dbPath);
};

#endif // ROLLBACK_MANAGER_H

