#include "rollback_manager.h"
#include "config_parser.h"
#include "package_manager.h"
#include <sqlite3.h>
#include <QUuid>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

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
    QDir dir;
    if (!dir.exists(QString::fromStdString(m_snapshotPath))) {
        if (!dir.mkpath(QString::fromStdString(m_snapshotPath))) {
            std::cerr << "Failed to create snapshot directory: " << m_snapshotPath << std::endl;
            return false;
        }
    }

    // Initialize database
    std::string dbPath = m_snapshotPath + "/snapshots.db";
    if (!initializeDatabase(dbPath)) {
        std::cerr << "Failed to initialize snapshot database" << std::endl;
        return false;
    }

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
    
    // Create directory using QDir
    QDir dir;
    if (!dir.mkpath(QString::fromStdString(snapshot.snapshot_path))) {
        std::cerr << "Failed to create snapshot directory: " << snapshot.snapshot_path << std::endl;
        return "";
    }
    
    // Save package files to snapshot directory (if available in package cache)
    // For pacman, packages are typically in /var/cache/pacman/pkg/
    // For APT, packages are in /var/cache/apt/archives/
    // This is a best-effort attempt to save package files
    std::string packageCachePath = "/var/cache/pacman/pkg/";
    QDir cacheDir(QString::fromStdString(packageCachePath));
    if (!cacheDir.exists()) {
        packageCachePath = "/var/cache/apt/archives/";
        cacheDir = QDir(QString::fromStdString(packageCachePath));
    }
    
    if (cacheDir.exists()) {
        for (const auto& pair : snapshot.package_versions) {
            // Try to find package file in cache
            QString packageName = QString::fromStdString(pair.first);
            QStringList filters;
            filters << packageName + "*";
            QStringList files = cacheDir.entryList(filters, QDir::Files);
            if (!files.isEmpty()) {
                // Copy first matching file to snapshot directory
                QString sourceFile = cacheDir.absoluteFilePath(files.first());
                QString destFile = QString::fromStdString(snapshot.snapshot_path) + "/" + files.first();
                QFile::copy(sourceFile, destFile);
            }
        }
    }

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

    // Implement actual rollback using package manager
    // For pacman: pacman -U package-version.pkg.tar.xz
    // For APT: apt install package=version
    
    bool success = true;
    QProcess process;
    
    // Check which package manager is available
    process.start("which", QStringList() << "pacman");
    process.waitForFinished(1000);
    bool isPacman = (process.exitCode() == 0);
    
    if (isPacman) {
        // Use pacman to rollback packages
        QStringList packageFiles;
        QDir snapshotDir(QString::fromStdString(snapshot->snapshot_path));
        QStringList filters;
        filters << "*.pkg.tar.*";
        QStringList files = snapshotDir.entryList(filters, QDir::Files);
        
        if (!files.isEmpty()) {
            for (const QString& file : files) {
                packageFiles << snapshotDir.absoluteFilePath(file);
            }
            
            process.start("pacman", QStringList() << "-U" << "--noconfirm" << packageFiles);
            process.waitForFinished(300000); // 5 minute timeout
            
            if (process.exitCode() != 0) {
                std::cerr << "Failed to rollback packages: " << process.readAllStandardError().toStdString() << std::endl;
                success = false;
            }
        } else {
            // No package files in snapshot, try to downgrade using package versions
            for (const auto& pair : snapshot->package_versions) {
                QString packageName = QString::fromStdString(pair.first);
                QString version = QString::fromStdString(pair.second);
                
                // Try to find package in cache or download specific version
                process.start("pacman", QStringList() << "-U" << "--noconfirm" << 
                             QString("%1-%2").arg(packageName).arg(version));
                process.waitForFinished(300000);
                
                if (process.exitCode() != 0) {
                    std::cerr << "Failed to rollback package " << pair.first << std::endl;
                    success = false;
                }
            }
        }
    } else {
        // Use APT to rollback packages
        for (const auto& pair : snapshot->package_versions) {
            QString packageName = QString::fromStdString(pair.first);
            QString version = QString::fromStdString(pair.second);
            
            process.start("apt", QStringList() << "install" << "--yes" << 
                         QString("%1=%2").arg(packageName).arg(version));
            process.waitForFinished(300000);
            
            if (process.exitCode() != 0) {
                std::cerr << "Failed to rollback package " << pair.first << std::endl;
                success = false;
            }
        }
    }

    return success;
}

std::unique_ptr<RollbackSnapshot> RollbackManager::getSnapshot(const std::string& snapshotId) {
    if (!m_initialized) {
        return nullptr;
    }
    
    std::string dbPath = m_snapshotPath + "/snapshots.db";
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return nullptr;
    }
    
    std::unique_ptr<RollbackSnapshot> snapshot = std::make_unique<RollbackSnapshot>();
    
    std::string sql = "SELECT snapshot_id, update_id, timestamp, snapshot_path, package_versions FROM snapshots WHERE snapshot_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, snapshotId.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            snapshot->snapshot_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            snapshot->update_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            snapshot->timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            snapshot->snapshot_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            
            // Parse package versions JSON
            QString versionsJson = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(versionsJson.toUtf8(), &error);
            if (error.error == QJsonParseError::NoError && doc.isObject()) {
                QJsonObject obj = doc.object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    snapshot->package_versions[it.key().toStdString()] = it.value().toString().toStdString();
                }
            }
        } else {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return nullptr;
        }
        
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    return snapshot;
}

std::vector<RollbackSnapshot> RollbackManager::getAllSnapshots() {
    std::vector<RollbackSnapshot> snapshots;
    
    if (!m_initialized) {
        return snapshots;
    }
    
    std::string dbPath = m_snapshotPath + "/snapshots.db";
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return snapshots;
    }
    
    std::string sql = "SELECT snapshot_id, update_id, timestamp, snapshot_path, package_versions FROM snapshots ORDER BY timestamp DESC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RollbackSnapshot snapshot;
            snapshot.snapshot_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            snapshot.update_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            snapshot.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            snapshot.snapshot_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            
            // Parse package versions JSON
            QString versionsJson = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(versionsJson.toUtf8(), &error);
            if (error.error == QJsonParseError::NoError && doc.isObject()) {
                QJsonObject obj = doc.object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    snapshot.package_versions[it.key().toStdString()] = it.value().toString().toStdString();
                }
            }
            
            snapshots.push_back(snapshot);
        }
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
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
    // Snapshots are loaded on-demand via getAllSnapshots()
    // This method is kept for compatibility but doesn't need to do anything
    // since we use lazy loading from the database
}

void RollbackManager::saveSnapshot(const RollbackSnapshot& snapshot) {
    if (!m_initialized) {
        return;
    }
    
    std::string dbPath = m_snapshotPath + "/snapshots.db";
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }
    
    // Convert package versions to JSON
    QJsonObject versionsObj;
    for (const auto& pair : snapshot.package_versions) {
        versionsObj[QString::fromStdString(pair.first)] = QString::fromStdString(pair.second);
    }
    QJsonDocument versionsDoc(versionsObj);
    QString versionsJson = QString::fromUtf8(versionsDoc.toJson());
    
    std::string sql = "INSERT OR REPLACE INTO snapshots (snapshot_id, update_id, timestamp, snapshot_path, package_versions) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, snapshot.snapshot_id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, snapshot.update_id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, snapshot.timestamp.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, snapshot.snapshot_path.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, versionsJson.toUtf8().constData(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to save snapshot: " << sqlite3_errmsg(db) << std::endl;
        }
        
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    std::cout << "Snapshot saved: " << snapshot.snapshot_id << std::endl;
}

void RollbackManager::deleteSnapshot(const std::string& snapshotId) {
    if (!m_initialized) {
        return;
    }
    
    // Get snapshot info to delete directory
    auto snapshot = getSnapshot(snapshotId);
    if (snapshot) {
        // Delete snapshot directory
        QDir snapshotDir(QString::fromStdString(snapshot->snapshot_path));
        if (snapshotDir.exists()) {
            snapshotDir.removeRecursively();
        }
    }
    
    // Delete from database
    std::string dbPath = m_snapshotPath + "/snapshots.db";
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }
    
    std::string sql = "DELETE FROM snapshots WHERE snapshot_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, snapshotId.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    std::cout << "Snapshot deleted: " << snapshotId << std::endl;
}

bool RollbackManager::initializeDatabase(const std::string& dbPath) {
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS snapshots (
            snapshot_id TEXT PRIMARY KEY,
            update_id TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            snapshot_path TEXT NOT NULL,
            package_versions TEXT NOT NULL
        )
    )";
    
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create snapshots table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_close(db);
    return true;
}

