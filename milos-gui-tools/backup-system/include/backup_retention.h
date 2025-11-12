#ifndef BACKUP_RETENTION_H
#define BACKUP_RETENTION_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

// Forward declarations
class BackupMetadataManager;

/**
 * @brief Backup Retention Policy
 */
struct RetentionPolicy {
    QString policyId;
    QString name;
    uint32_t retentionDays;  // 0 = keep forever
    uint32_t maxBackups;  // 0 = unlimited
    bool enabled;
};

/**
 * @brief Backup Retention Manager
 * 
 * Manages backup retention policies and automatic cleanup.
 */
class BackupRetentionManager : public QObject {
    Q_OBJECT

public:
    explicit BackupRetentionManager(QObject* parent = nullptr);
    ~BackupRetentionManager();

    /**
     * @brief Initialize retention manager
     * @param metadataManager Backup metadata manager instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BackupMetadataManager* metadataManager);

    /**
     * @brief Start retention manager
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop retention manager
     */
    void stop();

    /**
     * @brief Add retention policy
     * @param policy Retention policy
     * @return true if add successful, false otherwise
     */
    bool addPolicy(const RetentionPolicy& policy);

    /**
     * @brief Remove retention policy
     * @param policyId Policy ID
     * @return true if remove successful, false otherwise
     */
    bool removePolicy(const QString& policyId);

    /**
     * @brief Get all policies
     * @return Vector of retention policies
     */
    std::vector<RetentionPolicy> getPolicies() const;

    /**
     * @brief Enforce retention policies
     * @return Number of backups cleaned up
     */
    uint32_t enforcePolicies();

Q_SIGNALS:
    /**
     * @brief Emitted when backups are cleaned up
     */
    void backupsCleanedUp(uint32_t count);

private slots:
    /**
     * @brief Periodic cleanup check
     */
    void performCleanup();

private:
    bool m_initialized;
    bool m_running;
    BackupMetadataManager* m_metadataManager;
    QTimer* m_cleanupTimer;
    std::vector<RetentionPolicy> m_policies;

    /**
     * @brief Check if backup should be retained
     */
    bool shouldRetainBackup(const QString& backupId, uint64_t timestamp) const;
};

#endif // BACKUP_RETENTION_H


