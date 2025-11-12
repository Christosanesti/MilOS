#ifndef BACKUP_SERVICE_H
#define BACKUP_SERVICE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class BackupEncryption;
class BackupDestinationManager;
class BackupMetadataManager;
class BackupScheduler;
class BackupIntegrityVerifier;
class BackupRetentionManager;
class BackupCompression;
class BackupKeyRotationManager;

/**
 * @brief Backup Service
 * 
 * Main service class for zero-knowledge backup operations.
 */
class BackupService : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentBackupStatus READ currentBackupStatus NOTIFY backupStatusChanged)
    Q_PROPERTY(double backupProgress READ backupProgress NOTIFY backupProgressChanged)

public:
    explicit BackupService(QObject* parent = nullptr);
    ~BackupService();

    /**
     * @brief Initialize backup service
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start backup service
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop backup service
     */
    void stop();

    // Property getters
    QString currentBackupStatus() const { return m_currentStatus; }
    double backupProgress() const { return m_backupProgress; }

    /**
     * @brief Create backup
     * @param sourcePath Source path to backup
     * @param destinationId Destination ID
     * @param backupName Backup name
     * @return Backup ID if successful, empty string otherwise
     */
    Q_INVOKABLE QString createBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName);

    /**
     * @brief Restore backup
     * @param backupId Backup ID to restore
     * @param restorePath Restore destination path
     * @return true if restore successful, false otherwise
     */
    Q_INVOKABLE bool restoreBackup(const QString& backupId, const QString& restorePath);

    /**
     * @brief List backups
     * @return JSON string with backup list
     */
    Q_INVOKABLE QString listBackups() const;

    /**
     * @brief Delete backup
     * @param backupId Backup ID to delete
     * @return true if delete successful, false otherwise
     */
    Q_INVOKABLE bool deleteBackup(const QString& backupId);

    /**
     * @brief Verify backup integrity
     * @param backupId Backup ID to verify
     * @return JSON string with integrity report
     */
    Q_INVOKABLE QString verifyBackupIntegrity(const QString& backupId);

    /**
     * @brief Get backup status
     * @param backupId Backup ID
     * @return JSON string with backup status
     */
    Q_INVOKABLE QString getBackupStatus(const QString& backupId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when backup status changes
     */
    void backupStatusChanged();

    /**
     * @brief Emitted when backup progress changes
     */
    void backupProgressChanged();

    /**
     * @brief Emitted when backup completes
     */
    void backupCompleted(const QString& backupId);

    /**
     * @brief Emitted when backup fails
     */
    void backupFailed(const QString& error);

private:
    bool m_initialized;
    bool m_running;
    QString m_currentStatus;
    double m_backupProgress;

    std::unique_ptr<BackupEncryption> m_encryption;
    std::unique_ptr<BackupDestinationManager> m_destinationManager;
    std::unique_ptr<BackupMetadataManager> m_metadataManager;
    std::unique_ptr<BackupScheduler> m_scheduler;
    std::unique_ptr<BackupIntegrityVerifier> m_integrityVerifier;
    std::unique_ptr<BackupRetentionManager> m_retentionManager;
    std::unique_ptr<BackupCompression> m_compression;
    std::unique_ptr<BackupKeyRotationManager> m_keyRotation;

    /**
     * @brief Perform backup operation
     */
    QString performBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName);
};

#endif // BACKUP_SERVICE_H

