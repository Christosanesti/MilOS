#include "rollback_manager.h"
#include "config_parser.h"
#include "package_manager.h"
#include <sqlite3.h>
#include <QUuid>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

RollbackManager::RollbackManager()
    : m_initialized(false)
    , m_configParser(nullptr)
    , m_packageManager(nullptr)
{
}

RollbackManager::~RollbackManager() {
}

bool RollbackManager::initialize(ConfigParser* configParser, PackageManager* packageManager) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_packageManager = packageManager;

    // Get snapshot path from configuration
    m_snapshotPath = configParser->getString("rollback.snapshot_path");
    if (m_snapshotPath.empty()) {
        m_snapshotPath = "/var/lib/milos/update-service/snapshots";
    }

    // Create snapshot directory if it doesn't exist
    // TODO: Use filesystem library to create directory

    // Load existing snapshots
    loadSnapshots();

    m_initialized = true;
    return true;
}

std::string RollbackManager::createSnapshot(const std::string& updateId, const std::vector<std::string>& packageList) {
    if (!m_initialized || !m_packageManager) {
        return "";
    }

    RollbackSnapshot snapshot;
    snapshot.snapshot_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    snapshot.update_id = updateId;
    snapshot.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();

    // Get current package versions
    for (const auto& packageName : packageList) {
        auto packageInfo = m_packageManager->getPackageInfo(packageName);
        if (packageInfo) {
            snapshot.package_versions[packageName] = packageInfo->installed_version;
        }
    }

    // Create snapshot directory
    std::ostringstream snapshotDir;
    snapshotDir << m_snapshotPath << "/" << snapshot.snapshot_id;
    snapshot.snapshot_path = snapshotDir.str();
    
    // TODO: Create directory using filesystem library
    // TODO: Save package files to snapshot directory

    // Save snapshot metadata
    saveSnapshot(snapshot);

    return snapshot.snapshot_id;
}

bool RollbackManager::rollbackToSnapshot(const std::string& snapshotId) {
    if (!m_initialized || !m_packageManager) {
        return false;
    }

    auto snapshot = getSnapshot(snapshotId);
    if (!snapshot) {
        return false;
    }

    // Restore package versions
    std::vector<std::string> packagesToRestore;
    for (const auto& pair : snapshot->package_versions) {
        packagesToRestore.push_back(pair.first);
    }

    // TODO: Implement actual rollback using package manager
    // This would involve installing specific package versions
    // For pacman: pacman -U package-version.pkg.tar.xz
    // For APT: apt install package=version

    return true;
}

std::unique_ptr<RollbackSnapshot> RollbackManager::getSnapshot(const std::string& snapshotId) {
    // TODO: Load from database or file storage
    // For now, return nullptr
    return nullptr;
}

std::vector<RollbackSnapshot> RollbackManager::getAllSnapshots() {
    std::vector<RollbackSnapshot> snapshots;
    // TODO: Load all snapshots from storage
    return snapshots;
}

int RollbackManager::cleanupOldSnapshots(int maxSnapshots) {
    if (!m_initialized) {
        return 0;
    }

    std::vector<RollbackSnapshot> allSnapshots = getAllSnapshots();
    
    if (allSnapshots.size() <= maxSnapshots) {
        return 0;
    }

    // Sort by timestamp (oldest first)
    std::sort(allSnapshots.begin(), allSnapshots.end(),
        [](const RollbackSnapshot& a, const RollbackSnapshot& b) {
            return a.timestamp < b.timestamp;
        });

    // Delete oldest snapshots
    int deleted = 0;
    for (size_t i = 0; i < allSnapshots.size() - maxSnapshots; ++i) {
        deleteSnapshot(allSnapshots[i].snapshot_id);
        deleted++;
    }

    return deleted;
}

void RollbackManager::loadSnapshots() {
    // TODO: Load snapshots from database or file storage
}

void RollbackManager::saveSnapshot(const RollbackSnapshot& snapshot) {
    // TODO: Save snapshot to database or file storage
    // For now, just log
    std::cout << "Snapshot created: " << snapshot.snapshot_id << std::endl;
}

void RollbackManager::deleteSnapshot(const std::string& snapshotId) {
    // TODO: Delete snapshot from storage and filesystem
    std::cout << "Snapshot deleted: " << snapshotId << std::endl;
}

