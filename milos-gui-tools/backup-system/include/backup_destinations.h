#ifndef BACKUP_DESTINATIONS_H
#define BACKUP_DESTINATIONS_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstdint>

/**
 * @brief Backup Destination Type
 */
enum class BackupDestinationType {
    Local,
    SMB,
    NFS,
    Cloud
};

/**
 * @brief Backup Destination Configuration
 */
struct BackupDestinationConfig {
    BackupDestinationType type;
    std::string name;
    std::string path;
    std::string host;  // For network destinations
    std::string username;
    std::string password;
    std::map<std::string, std::string> additionalParams;
};

/**
 * @brief Backup Destination Interface
 */
class IBackupDestination {
public:
    virtual ~IBackupDestination() = default;

    /**
     * @brief Initialize destination
     * @param config Destination configuration
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize(const BackupDestinationConfig& config) = 0;

    /**
     * @brief Upload backup data
     * @param backupId Backup ID
     * @param data Encrypted backup data
     * @return true if upload successful, false otherwise
     */
    virtual bool uploadBackup(const std::string& backupId, const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Download backup data
     * @param backupId Backup ID
     * @return Encrypted backup data
     */
    virtual std::vector<uint8_t> downloadBackup(const std::string& backupId) = 0;

    /**
     * @brief List available backups
     * @return Vector of backup IDs
     */
    virtual std::vector<std::string> listBackups() = 0;

    /**
     * @brief Delete backup
     * @param backupId Backup ID to delete
     * @return true if delete successful, false otherwise
     */
    virtual bool deleteBackup(const std::string& backupId) = 0;

    /**
     * @brief Check if destination is available
     * @return true if available, false otherwise
     */
    virtual bool isAvailable() const = 0;
};

/**
 * @brief Local Filesystem Backup Destination
 */
class LocalBackupDestination : public IBackupDestination {
public:
    bool initialize(const BackupDestinationConfig& config) override;
    bool uploadBackup(const std::string& backupId, const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> downloadBackup(const std::string& backupId) override;
    std::vector<std::string> listBackups() override;
    bool deleteBackup(const std::string& backupId) override;
    bool isAvailable() const override { return m_available; }

private:
    bool m_available;
    std::string m_basePath;
};

/**
 * @brief Backup Destination Manager
 * 
 * Manages multiple backup destinations (local, network, cloud).
 */
class BackupDestinationManager {
public:
    BackupDestinationManager();
    ~BackupDestinationManager();

    /**
     * @brief Initialize destination manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add backup destination
     * @param config Destination configuration
     * @return Destination ID
     */
    std::string addDestination(const BackupDestinationConfig& config);

    /**
     * @brief Get backup destination
     * @param destinationId Destination ID
     * @return Backup destination interface, nullptr if not found
     */
    IBackupDestination* getDestination(const std::string& destinationId);

    /**
     * @brief List all destinations
     * @return Vector of destination IDs
     */
    std::vector<std::string> listDestinations() const;

private:
    std::map<std::string, std::unique_ptr<IBackupDestination>> m_destinations;
};

#endif // BACKUP_DESTINATIONS_H

