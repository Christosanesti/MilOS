#ifndef BACKUP_KEY_ROTATION_H
#define BACKUP_KEY_ROTATION_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

// Forward declarations
class BackupEncryption;
class BackupMetadataManager;
class BackupDestinationManager;

/**
 * @brief Key Rotation Configuration
 */
struct KeyRotationConfig {
    QString configId;
    bool enabled;
    uint32_t rotationIntervalDays;  // 0 = manual only
    QDateTime lastRotation;
    bool reEncryptBackups;  // Re-encrypt existing backups with new key
};

/**
 * @brief Backup Key Rotation Manager
 * 
 * Manages encryption key rotation for security.
 */
class BackupKeyRotationManager : public QObject {
    Q_OBJECT

public:
    explicit BackupKeyRotationManager(QObject* parent = nullptr);
    ~BackupKeyRotationManager();

    /**
     * @brief Initialize key rotation manager
     * @param encryption Backup encryption instance
     * @param metadataManager Backup metadata manager instance
     * @param destinationManager Backup destination manager instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BackupEncryption* encryption,
                    BackupMetadataManager* metadataManager,
                    BackupDestinationManager* destinationManager);

    /**
     * @brief Start key rotation manager
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop key rotation manager
     */
    void stop();

    /**
     * @brief Rotate encryption keys
     * @param reEncryptBackups Whether to re-encrypt existing backups
     * @return true if rotation successful, false otherwise
     */
    bool rotateKeys(bool reEncryptBackups = false);

    /**
     * @brief Get key rotation configuration
     * @return Key rotation configuration
     */
    KeyRotationConfig getConfig() const { return m_config; }

    /**
     * @brief Set key rotation configuration
     * @param config Key rotation configuration
     */
    void setConfig(const KeyRotationConfig& config) { m_config = config; }

Q_SIGNALS:
    /**
     * @brief Emitted when keys are rotated
     */
    void keysRotated();

    /**
     * @brief Emitted when key rotation fails
     */
    void keyRotationFailed(const QString& error);

private slots:
    /**
     * @brief Check if key rotation is needed
     */
    void checkKeyRotation();

private:
    bool m_initialized;
    bool m_running;
    BackupEncryption* m_encryption;
    BackupMetadataManager* m_metadataManager;
    BackupDestinationManager* m_destinationManager;
    QTimer* m_rotationTimer;
    KeyRotationConfig m_config;

    /**
     * @brief Re-encrypt backup with new key
     */
    bool reEncryptBackup(const QString& backupId);
};

#endif // BACKUP_KEY_ROTATION_H

