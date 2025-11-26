#ifndef UPDATE_APPLIER_H
#define UPDATE_APPLIER_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>

class ConfigParser;
class PackageManager;
class SignatureVerifier;
class RollbackManager;
class AuditLogger;

/**
 * @brief Update Status
 */
enum class UpdateStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED,
    FAILED,
    ROLLED_BACK
};

/**
 * @brief Update Information
 */
struct UpdateInfo {
    std::string update_id;
    std::vector<std::string> package_list;
    UpdateStatus status;
    std::string error_message;
    std::string rollback_info;  // JSON string
    bool signature_verified;
};

/**
 * @brief Update Applier
 * 
 * Handles update application with backup, signature verification,
 * and progress tracking.
 */
class UpdateApplier {
public:
    UpdateApplier();
    ~UpdateApplier();

    /**
     * @brief Initialize update applier
     * @param configParser Configuration parser instance
     * @param packageManager Package manager instance
     * @param signatureVerifier Signature verifier instance
     * @param rollbackManager Rollback manager instance
     * @param auditLogger Audit logger instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        PackageManager* packageManager,
        SignatureVerifier* signatureVerifier,
        RollbackManager* rollbackManager,
        AuditLogger* auditLogger
    );

    /**
     * @brief Apply updates
     * @param packageList List of packages to update
     * @param progressCallback Progress callback (package_name, progress_percent)
     * @return Update information
     */
    UpdateInfo applyUpdates(
        const std::vector<std::string>& packageList,
        std::function<void(const std::string&, int)> progressCallback = nullptr
    );

    /**
     * @brief Get current update status
     * @param updateId Update ID
     * @return Update information, nullptr if not found
     */
    std::unique_ptr<UpdateInfo> getUpdateStatus(const std::string& updateId);

    /**
     * @brief Get all update history
     * @return Vector of update information
     */
    std::vector<UpdateInfo> getUpdateHistory() const;

    /**
     * @brief Get current update (if any)
     * @return Update information, nullptr if no update in progress
     */
    std::unique_ptr<UpdateInfo> getCurrentUpdate() const;

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    PackageManager* m_packageManager;
    SignatureVerifier* m_signatureVerifier;
    RollbackManager* m_rollbackManager;
    AuditLogger* m_auditLogger;
    
    // Update history storage
    mutable std::vector<UpdateInfo> m_updateHistory;
    mutable std::mutex m_historyMutex;
    std::unique_ptr<UpdateInfo> m_currentUpdate;
    mutable std::mutex m_currentUpdateMutex;

    /**
     * @brief Create backup before update
     * @param packageList List of packages to backup
     * @return true if backup successful, false otherwise
     */
    bool createBackup(const std::vector<std::string>& packageList);

    /**
     * @brief Verify package signatures before installation
     * @param packageList List of packages to verify
     * @return true if all signatures verified, false otherwise
     */
    bool verifySignatures(const std::vector<std::string>& packageList);
};

#endif // UPDATE_APPLIER_H

