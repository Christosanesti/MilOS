#include "backup_destinations.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

// Local Filesystem Implementation
bool LocalBackupDestination::initialize(const BackupDestinationConfig& config) {
    m_basePath = config.path;
    
    // Create base directory if it doesn't exist
    try {
        std::filesystem::create_directories(m_basePath);
        m_available = true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to create backup directory: " << e.what() << std::endl;
        m_available = false;
    }
    
    return m_available;
}

bool LocalBackupDestination::uploadBackup(const std::string& backupId, const std::vector<uint8_t>& data) {
    if (!m_available) {
        return false;
    }

    std::string filePath = m_basePath + "/" + backupId + ".backup";
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open backup file for writing: " << filePath << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
    
    return true;
}

std::vector<uint8_t> LocalBackupDestination::downloadBackup(const std::string& backupId) {
    std::vector<uint8_t> data;
    
    if (!m_available) {
        return data;
    }

    std::string filePath = m_basePath + "/" + backupId + ".backup";
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open backup file for reading: " << filePath << std::endl;
        return data;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    file.close();
    
    return data;
}

std::vector<std::string> LocalBackupDestination::listBackups() {
    std::vector<std::string> backups;
    
    if (!m_available) {
        return backups;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_basePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".backup") {
                std::string filename = entry.path().filename().string();
                std::string backupId = filename.substr(0, filename.length() - 7);  // Remove ".backup"
                backups.push_back(backupId);
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to list backups: " << e.what() << std::endl;
    }
    
    return backups;
}

bool LocalBackupDestination::deleteBackup(const std::string& backupId) {
    if (!m_available) {
        return false;
    }

    std::string filePath = m_basePath + "/" + backupId + ".backup";
    
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to delete backup: " << e.what() << std::endl;
        return false;
    }
}

// Backup Destination Manager Implementation
BackupDestinationManager::BackupDestinationManager() {
}

BackupDestinationManager::~BackupDestinationManager() {
}

bool BackupDestinationManager::initialize() {
    return true;
}

std::string BackupDestinationManager::addDestination(const BackupDestinationConfig& config) {
    std::unique_ptr<IBackupDestination> destination;
    
    switch (config.type) {
        case BackupDestinationType::Local:
            destination = std::make_unique<LocalBackupDestination>();
            break;
        case BackupDestinationType::SMB:
        case BackupDestinationType::NFS:
        case BackupDestinationType::Cloud:
            // Network and cloud destinations would be implemented here
            // For now, return empty string
            std::cerr << "Network/cloud destinations not yet implemented" << std::endl;
            return "";
    }
    
    if (!destination) {
        return "";
    }
    
    if (!destination->initialize(config)) {
        std::cerr << "Failed to initialize backup destination" << std::endl;
        return "";
    }
    
    std::string destinationId = config.name;
    m_destinations[destinationId] = std::move(destination);
    
    return destinationId;
}

IBackupDestination* BackupDestinationManager::getDestination(const std::string& destinationId) {
    auto it = m_destinations.find(destinationId);
    if (it != m_destinations.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> BackupDestinationManager::listDestinations() const {
    std::vector<std::string> destinationIds;
    for (const auto& pair : m_destinations) {
        destinationIds.push_back(pair.first);
    }
    return destinationIds;
}

