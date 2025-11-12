#include "backup_key_rotation.h"
#include "backup_encryption.h"
#include "backup_metadata.h"
#include "backup_destinations.h"
#include <QTimer>
#include <QDateTime>
#include <iostream>

BackupKeyRotationManager::BackupKeyRotationManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
    , m_encryption(nullptr)
    , m_metadataManager(nullptr)
    , m_destinationManager(nullptr)
    , m_rotationTimer(nullptr)
{
    m_config.enabled = false;
    m_config.rotationIntervalDays = 90;  // Default 90 days
}

BackupKeyRotationManager::~BackupKeyRotationManager() {
    stop();
}

bool BackupKeyRotationManager::initialize(BackupEncryption* encryption,
                                          BackupMetadataManager* metadataManager,
                                          BackupDestinationManager* destinationManager) {
    if (m_initialized) {
        return true;
    }

    m_encryption = encryption;
    m_metadataManager = metadataManager;
    m_destinationManager = destinationManager;

    if (!m_encryption || !m_metadataManager || !m_destinationManager) {
        std::cerr << "Required components are null" << std::endl;
        return false;
    }

    m_rotationTimer = new QTimer(this);
    connect(m_rotationTimer, &QTimer::timeout, this, &BackupKeyRotationManager::checkKeyRotation);
    m_rotationTimer->setInterval(86400000);  // Check daily

    m_initialized = true;
    return true;
}

bool BackupKeyRotationManager::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running) {
        return true;
    }

    if (m_rotationTimer && m_config.enabled) {
        m_rotationTimer->start();
    }

    m_running = true;
    return true;
}

void BackupKeyRotationManager::stop() {
    if (!m_running) {
        return;
    }

    if (m_rotationTimer) {
        m_rotationTimer->stop();
    }

    m_running = false;
}

bool BackupKeyRotationManager::rotateKeys(bool reEncryptBackups) {
    if (!m_encryption) {
        emit keyRotationFailed("Encryption not initialized");
        return false;
    }

    // Generate new encryption key
    // In production, this would call m_encryption->generateKey() and update key storage
    // For now, this is a placeholder

    m_config.lastRotation = QDateTime::currentDateTime();

    if (reEncryptBackups) {
        // Re-encrypt all existing backups
        if (!m_metadataManager) {
            emit keyRotationFailed("Metadata manager not initialized");
            return false;
        }

        auto backups = m_metadataManager->listBackups();
        for (const auto& backup : backups) {
            if (!reEncryptBackup(QString::fromStdString(backup.id))) {
                emit keyRotationFailed("Failed to re-encrypt backup: " + QString::fromStdString(backup.id));
                return false;
            }
        }
    }

    emit keysRotated();
    return true;
}

bool BackupKeyRotationManager::reEncryptBackup(const QString& backupId) {
    if (!m_metadataManager || !m_destinationManager || !m_encryption) {
        return false;
    }

    // Get backup metadata
    BackupMetadata metadata = m_metadataManager->getMetadata(backupId.toStdString());
    if (metadata.id.empty()) {
        return false;
    }

    // Download backup
    IBackupDestination* destination = m_destinationManager->getDestination(metadata.destination_id);
    if (!destination) {
        return false;
    }

    std::vector<uint8_t> encryptedData = destination->downloadBackup(backupId.toStdString());
    if (encryptedData.empty()) {
        return false;
    }

    // Decrypt with old key
    std::vector<uint8_t> decryptedData = m_encryption->decrypt(encryptedData);

    // Re-encrypt with new key
    std::vector<uint8_t> reEncryptedData = m_encryption->encrypt(decryptedData);

    // Upload re-encrypted backup
    if (!destination->uploadBackup(backupId.toStdString(), reEncryptedData)) {
        return false;
    }

    return true;
}

void BackupKeyRotationManager::checkKeyRotation() {
    if (!m_config.enabled || m_config.rotationIntervalDays == 0) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QDateTime nextRotation = m_config.lastRotation.addDays(m_config.rotationIntervalDays);

    if (now >= nextRotation || m_config.lastRotation.isNull()) {
        rotateKeys(m_config.reEncryptBackups);
    }
}

