#include "backup_service.h"
#include "backup_encryption.h"
#include "backup_destinations.h"
#include "backup_metadata.h"
#include "backup_scheduler.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QUuid>
#include <fstream>
#include <filesystem>
#include <iostream>

BackupService::BackupService(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
    , m_backupProgress(0.0)
{
}

BackupService::~BackupService() {
    stop();
}

bool BackupService::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize encryption
    m_encryption = std::make_unique<BackupEncryption>();
    if (!m_encryption->initialize()) {
        std::cerr << "Failed to initialize backup encryption" << std::endl;
        return false;
    }

    // Initialize destination manager
    m_destinationManager = std::make_unique<BackupDestinationManager>();
    if (!m_destinationManager->initialize()) {
        std::cerr << "Failed to initialize backup destination manager" << std::endl;
        return false;
    }

    // Initialize metadata manager
    m_metadataManager = std::make_unique<BackupMetadataManager>();
    std::string dbPath = "/var/lib/milos/backup-system/metadata.db";
    if (!m_metadataManager->initialize(dbPath)) {
        std::cerr << "Failed to initialize backup metadata manager" << std::endl;
        return false;
    }

    // Initialize scheduler
    m_scheduler = std::make_unique<BackupScheduler>();
    if (!m_scheduler->initialize(this)) {
        std::cerr << "Failed to initialize backup scheduler" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool BackupService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start scheduler
    if (!m_scheduler->start()) {
        std::cerr << "Failed to start backup scheduler" << std::endl;
        return false;
    }

    m_running = true;
    m_currentStatus = "Ready";
    emit backupStatusChanged();
    return true;
}

void BackupService::stop() {
    if (!m_running) {
        return;
    }

    if (m_scheduler) {
        m_scheduler->stop();
    }

    m_running = false;
    m_currentStatus = "Stopped";
    emit backupStatusChanged();
}

QString BackupService::createBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName) {
    m_currentStatus = "Creating backup...";
    m_backupProgress = 0.0;
    emit backupStatusChanged();
    emit backupProgressChanged();

    QString backupId = performBackup(sourcePath, destinationId, backupName);

    if (!backupId.isEmpty()) {
        m_currentStatus = "Backup completed";
        m_backupProgress = 100.0;
        emit backupCompleted(backupId);
    } else {
        m_currentStatus = "Backup failed";
        emit backupFailed("Failed to create backup");
    }

    emit backupStatusChanged();
    emit backupProgressChanged();
    return backupId;
}

QString BackupService::performBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName) {
    if (!m_encryption || !m_destinationManager || !m_metadataManager) {
        return QString();
    }

    // Generate backup ID
    QString backupId = QUuid::createUuid().toString();

    // Read source files
    std::vector<uint8_t> sourceData;
    try {
        std::ifstream file(sourcePath.toStdString(), std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open source file: " << sourcePath.toStdString() << std::endl;
            return QString();
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        sourceData.resize(size);
        file.read(reinterpret_cast<char*>(sourceData.data()), size);
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error reading source file: " << e.what() << std::endl;
        return QString();
    }

    // Encrypt backup data (client-side, zero-knowledge)
    std::vector<uint8_t> encryptedData = m_encryption->encrypt(sourceData);

    // Upload to destination
    IBackupDestination* destination = m_destinationManager->getDestination(destinationId.toStdString());
    if (!destination) {
        std::cerr << "Backup destination not found: " << destinationId.toStdString() << std::endl;
        return QString();
    }

    if (!destination->uploadBackup(backupId.toStdString(), encryptedData)) {
        std::cerr << "Failed to upload backup" << std::endl;
        return QString();
    }

    // Store metadata
    BackupMetadata metadata;
    metadata.id = backupId.toStdString();
    metadata.name = backupName.toStdString();
    metadata.destination_id = destinationId.toStdString();
    metadata.source_path = sourcePath.toStdString();
    metadata.timestamp = QDateTime::currentSecsSinceEpoch();
    metadata.size = encryptedData.size();
    metadata.encryption_algorithm = "CRYSTALS-Kyber";
    metadata.is_encrypted = true;

    if (!m_metadataManager->addMetadata(metadata)) {
        std::cerr << "Failed to store backup metadata" << std::endl;
        return QString();
    }

    return backupId;
}

bool BackupService::restoreBackup(const QString& backupId, const QString& restorePath) {
    m_currentStatus = "Restoring backup...";
    m_backupProgress = 0.0;
    emit backupStatusChanged();
    emit backupProgressChanged();

    if (!m_encryption || !m_destinationManager || !m_metadataManager) {
        emit backupFailed("Service not initialized");
        return false;
    }

    // Get metadata
    BackupMetadata metadata = m_metadataManager->getMetadata(backupId.toStdString());
    if (metadata.id.empty()) {
        emit backupFailed("Backup not found");
        return false;
    }

    // Download from destination
    IBackupDestination* destination = m_destinationManager->getDestination(metadata.destination_id);
    if (!destination) {
        emit backupFailed("Backup destination not found");
        return false;
    }

    std::vector<uint8_t> encryptedData = destination->downloadBackup(backupId.toStdString());
    if (encryptedData.empty()) {
        emit backupFailed("Failed to download backup");
        return false;
    }

    // Decrypt backup data (client-side, zero-knowledge)
    std::vector<uint8_t> decryptedData = m_encryption->decrypt(encryptedData);

    // Write to restore path
    try {
        std::ofstream file(restorePath.toStdString(), std::ios::binary);
        if (!file.is_open()) {
            emit backupFailed("Failed to open restore path");
            return false;
        }

        file.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
        file.close();
    } catch (const std::exception& e) {
        emit backupFailed(QString::fromStdString(e.what()));
        return false;
    }

    m_currentStatus = "Restore completed";
    m_backupProgress = 100.0;
    emit backupStatusChanged();
    emit backupProgressChanged();
    return true;
}

QString BackupService::listBackups() const {
    if (!m_metadataManager) {
        return QString("[]");
    }

    auto backups = m_metadataManager->listBackups();
    QJsonArray backupArray;

    for (const auto& backup : backups) {
        QJsonObject backupObj;
        backupObj["id"] = QString::fromStdString(backup.id);
        backupObj["name"] = QString::fromStdString(backup.name);
        backupObj["destination_id"] = QString::fromStdString(backup.destination_id);
        backupObj["source_path"] = QString::fromStdString(backup.source_path);
        backupObj["timestamp"] = static_cast<qint64>(backup.timestamp);
        backupObj["size"] = static_cast<qint64>(backup.size);
        backupObj["encryption_algorithm"] = QString::fromStdString(backup.encryption_algorithm);
        backupObj["is_encrypted"] = backup.is_encrypted;
        backupArray.append(backupObj);
    }

    QJsonDocument doc(backupArray);
    return QString::fromUtf8(doc.toJson());
}

bool BackupService::deleteBackup(const QString& backupId) {
    if (!m_metadataManager || !m_destinationManager) {
        return false;
    }

    // Get metadata
    BackupMetadata metadata = m_metadataManager->getMetadata(backupId.toStdString());
    if (metadata.id.empty()) {
        return false;
    }

    // Delete from destination
    IBackupDestination* destination = m_destinationManager->getDestination(metadata.destination_id);
    if (destination) {
        destination->deleteBackup(backupId.toStdString());
    }

    // Delete metadata
    return m_metadataManager->deleteMetadata(backupId.toStdString());
}

