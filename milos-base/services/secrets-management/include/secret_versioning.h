#ifndef SECRET_VERSIONING_H
#define SECRET_VERSIONING_H

#include <string>
#include <vector>
#include <memory>

class SecretStorage;

/**
 * @brief Secret Version
 */
struct SecretVersion {
    std::string version_id;
    std::string secret_id;
    std::vector<uint8_t> encrypted_data;
    std::string created_at;
    std::string description;
};

/**
 * @brief Secret Versioning
 * 
 * Manages secret version history and rollback.
 */
class SecretVersioning {
public:
    SecretVersioning();
    ~SecretVersioning();

    /**
     * @brief Initialize versioning
     * @param storage Secret storage instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(SecretStorage* storage);

    /**
     * @brief Create new version
     * @param secretId Secret ID
     * @param secretData Secret data
     * @return Version ID, or empty if failed
     */
    std::string createVersion(const std::string& secretId, const std::vector<uint8_t>& secretData);

    /**
     * @brief Get version
     * @param secretId Secret ID
     * @param versionId Version ID (empty for latest)
     * @return Secret version, or empty if not found
     */
    SecretVersion getVersion(const std::string& secretId, const std::string& versionId = "") const;

    /**
     * @brief List versions for secret
     * @param secretId Secret ID
     * @return List of version IDs
     */
    std::vector<std::string> listVersions(const std::string& secretId) const;

    /**
     * @brief Rollback to version
     * @param secretId Secret ID
     * @param versionId Version ID to rollback to
     * @return true if rollback successful, false otherwise
     */
    bool rollbackToVersion(const std::string& secretId, const std::string& versionId);

    /**
     * @brief Delete old versions
     * @param secretId Secret ID
     * @param keepCount Number of versions to keep
     * @return true if deletion successful, false otherwise
     */
    bool deleteOldVersions(const std::string& secretId, int keepCount);

private:
    SecretStorage* m_storage;
    void* m_database;  // sqlite3* pointer

    /**
     * @brief Initialize versioning database
     * @return true if successful, false otherwise
     */
    bool initializeDatabase();

    /**
     * @brief Generate version ID
     * @return Version ID
     */
    std::string generateVersionId() const;
};

#endif // SECRET_VERSIONING_H

