#include "secret_versioning.h"
#include "secret_storage.h"
#include <sqlite3.h>
#include <openssl/rand.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

SecretVersioning::SecretVersioning()
    : m_storage(nullptr)
    , m_database(nullptr)
{
}

SecretVersioning::~SecretVersioning() {
    if (m_database) {
        sqlite3_close(static_cast<sqlite3*>(m_database));
        m_database = nullptr;
    }
}

bool SecretVersioning::initialize(SecretStorage* storage) {
    if (!storage) {
        return false;
    }

    m_storage = storage;

    // Use same database as secret storage
    // In production, could use separate database
    if (!initializeDatabase()) {
        return false;
    }

    return true;
}

bool SecretVersioning::initializeDatabase() {
    // Open or create database
    std::string dbPath = "/var/lib/milos/secrets/versions.db";
    sqlite3* db = nullptr;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open versioning database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    m_database = db;

    // Create versions table
    const char* createVersionsTable = R"(
        CREATE TABLE IF NOT EXISTS secret_versions (
            version_id TEXT PRIMARY KEY,
            secret_id TEXT NOT NULL,
            encrypted_data BLOB NOT NULL,
            created_at TEXT NOT NULL,
            description TEXT,
            FOREIGN KEY(secret_id) REFERENCES secrets(secret_id)
        )
    )";

    rc = sqlite3_exec(static_cast<sqlite3*>(m_database), createVersionsTable, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to create versions table: " << sqlite3_errmsg(static_cast<sqlite3*>(m_database)) << std::endl;
        return false;
    }

    // Create index
    const char* createIndex = "CREATE INDEX IF NOT EXISTS idx_secret_id ON secret_versions(secret_id)";
    sqlite3_exec(static_cast<sqlite3*>(m_database), createIndex, nullptr, nullptr, nullptr);

    return true;
}

std::string SecretVersioning::createVersion(const std::string& secretId, const std::vector<uint8_t>& secretData) {
    if (!m_database || !m_storage) {
        return "";
    }

    // Encrypt secret data (reuse storage encryption)
    std::vector<uint8_t> encryptedData = m_storage->encryptSecret(secretData);
    if (encryptedData.empty()) {
        return "";
    }

    std::string versionId = generateVersionId();
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    std::string createdAt = timeStr;

    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* insertVersion = "INSERT INTO secret_versions (version_id, secret_id, encrypted_data, created_at, description) VALUES (?, ?, ?, ?, ?)";
    
    int rc = sqlite3_prepare_v2(db, insertVersion, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return "";
    }

    sqlite3_bind_text(stmt, 1, versionId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, secretId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, encryptedData.data(), encryptedData.size(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, createdAt.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, "", -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        return "";
    }

    return versionId;
}

SecretVersion SecretVersioning::getVersion(const std::string& secretId, const std::string& versionId) const {
    SecretVersion version;

    if (!m_database) {
        return version;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* selectVersion;

    if (versionId.empty()) {
        // Get latest version
        selectVersion = "SELECT version_id, secret_id, encrypted_data, created_at, description FROM secret_versions WHERE secret_id = ? ORDER BY created_at DESC LIMIT 1";
    } else {
        // Get specific version
        selectVersion = "SELECT version_id, secret_id, encrypted_data, created_at, description FROM secret_versions WHERE secret_id = ? AND version_id = ?";
    }

    int rc = sqlite3_prepare_v2(db, selectVersion, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return version;
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
    if (!versionId.empty()) {
        sqlite3_bind_text(stmt, 2, versionId.c_str(), -1, SQLITE_STATIC);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        version.version_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        version.secret_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        
        const void* blob = sqlite3_column_blob(stmt, 2);
        int blobSize = sqlite3_column_bytes(stmt, 2);
        version.encrypted_data.assign(static_cast<const uint8_t*>(blob),
                                     static_cast<const uint8_t*>(blob) + blobSize);
        
        version.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        version.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    }

    sqlite3_finalize(stmt);
    return version;
}

std::vector<std::string> SecretVersioning::listVersions(const std::string& secretId) const {
    std::vector<std::string> versionIds;

    if (!m_database) {
        return versionIds;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* selectVersions = "SELECT version_id FROM secret_versions WHERE secret_id = ? ORDER BY created_at DESC";
    
    int rc = sqlite3_prepare_v2(db, selectVersions, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return versionIds;
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        versionIds.push_back(id);
    }

    sqlite3_finalize(stmt);
    return versionIds;
}

bool SecretVersioning::rollbackToVersion(const std::string& secretId, const std::string& versionId) {
    if (!m_database || !m_storage) {
        return false;
    }

    // Get version data
    SecretVersion version = getVersion(secretId, versionId);
    if (version.version_id.empty()) {
        return false;
    }

    // Decrypt version data
    std::vector<uint8_t> secretData = m_storage->decryptSecret(version.encrypted_data);
    if (secretData.empty()) {
        return false;
    }

    // Get current metadata
    SecretMetadata metadata = m_storage->getSecretMetadata(secretId);
    if (metadata.secret_id.empty()) {
        return false;
    }

    // Store rolled back secret
    return m_storage->storeSecret(secretId, secretData, metadata);
}

bool SecretVersioning::deleteOldVersions(const std::string& secretId, int keepCount) {
    if (!m_database) {
        return false;
    }

    // Get all versions
    std::vector<std::string> versionIds = listVersions(secretId);
    if (versionIds.size() <= keepCount) {
        return true;
    }

    // Delete old versions (keep most recent)
    sqlite3* db = static_cast<sqlite3*>(m_database);
    for (size_t i = keepCount; i < versionIds.size(); i++) {
        sqlite3_stmt* stmt;
        const char* deleteVersion = "DELETE FROM secret_versions WHERE secret_id = ? AND version_id = ?";
        
        int rc = sqlite3_prepare_v2(db, deleteVersion, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            continue;
        }

        sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, versionIds[i].c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    return true;
}

std::string SecretVersioning::generateVersionId() const {
    // Generate random version ID
    unsigned char randomBytes[16];
    RAND_bytes(randomBytes, sizeof(randomBytes));

    std::ostringstream oss;
    oss << std::hex;
    for (int i = 0; i < 16; i++) {
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(randomBytes[i]);
    }

    return oss.str();
}

