#include "secret_storage.h"
#include <sqlite3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

SecretStorage::SecretStorage()
    : m_initialized(false)
    , m_databasePath("/var/lib/milos/secrets/secrets.db")
    , m_database(nullptr)
{
}

SecretStorage::~SecretStorage() {
    if (m_database) {
        sqlite3_close(static_cast<sqlite3*>(m_database));
        m_database = nullptr;
    }
}

bool SecretStorage::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create storage directory
    std::filesystem::create_directories(std::filesystem::path(m_databasePath).parent_path());

    // Initialize database
    if (!initializeDatabase()) {
        return false;
    }

    // Load secrets from database
    loadSecrets();

    m_initialized = true;
    return true;
}

bool SecretStorage::initializeDatabase() {
    sqlite3* db = nullptr;
    int rc = sqlite3_open(m_databasePath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    m_database = db;

    // Create tables
    const char* createSecretsTable = R"(
        CREATE TABLE IF NOT EXISTS secrets (
            secret_id TEXT PRIMARY KEY,
            encrypted_data BLOB NOT NULL,
            encryption_algorithm TEXT NOT NULL,
            version TEXT NOT NULL,
            created_at TEXT NOT NULL
        )
    )";

    const char* createMetadataTable = R"(
        CREATE TABLE IF NOT EXISTS secret_metadata (
            secret_id TEXT PRIMARY KEY,
            secret_type TEXT NOT NULL,
            description TEXT,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL,
            expires_at TEXT,
            owner TEXT NOT NULL,
            tags TEXT,
            is_encrypted INTEGER NOT NULL,
            FOREIGN KEY(secret_id) REFERENCES secrets(secret_id)
        )
    )";

    rc = sqlite3_exec(static_cast<sqlite3*>(m_database), createSecretsTable, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to create secrets table: " << sqlite3_errmsg(static_cast<sqlite3*>(m_database)) << std::endl;
        return false;
    }

    rc = sqlite3_exec(static_cast<sqlite3*>(m_database), createMetadataTable, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to create metadata table: " << sqlite3_errmsg(static_cast<sqlite3*>(m_database)) << std::endl;
        return false;
    }

    return true;
}

bool SecretStorage::storeSecret(const std::string& secretId,
                                const std::vector<uint8_t>& secretData,
                                const SecretMetadata& metadata) {
    if (!m_initialized || !m_database) {
        return false;
    }

    // Encrypt secret data
    std::vector<uint8_t> encryptedData = encryptSecret(secretData, secretId, metadata);
    if (encryptedData.empty()) {
        std::cerr << "Failed to encrypt secret" << std::endl;
        return false;
    }

    // Format timestamps
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    std::string createdAt = timeStr;

    sqlite3* db = static_cast<sqlite3*>(m_database);

    // Insert secret data
    sqlite3_stmt* stmt;
    const char* insertSecret = "INSERT OR REPLACE INTO secrets (secret_id, encrypted_data, encryption_algorithm, version, created_at) VALUES (?, ?, ?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, insertSecret, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, encryptedData.data(), encryptedData.size(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "aes-256-gcm", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, "1.0.0", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, createdAt.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        return false;
    }

    // Insert metadata
    const char* insertMetadata = "INSERT OR REPLACE INTO secret_metadata (secret_id, secret_type, description, created_at, updated_at, expires_at, owner, tags, is_encrypted) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    rc = sqlite3_prepare_v2(db, insertMetadata, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    std::string tagsStr;
    for (size_t i = 0; i < metadata.tags.size(); i++) {
        if (i > 0) tagsStr += ",";
        tagsStr += metadata.tags[i];
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, metadata.secret_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, metadata.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, metadata.created_at.empty() ? createdAt.c_str() : metadata.created_at.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, createdAt.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, metadata.expires_at.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, metadata.owner.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, tagsStr.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 9, metadata.is_encrypted ? 1 : 0);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        return false;
    }

    // Update cache
    SecretData secretData;
    secretData.secret_id = secretId;
    secretData.encrypted_data = encryptedData;
    secretData.encryption_algorithm = "aes-256-gcm";
    secretData.version = "1.0.0";
    secretData.created_at = createdAt;

    m_secretCache[secretId] = secretData;
    m_metadataCache[secretId] = metadata;

    return true;
}

std::vector<uint8_t> SecretStorage::getSecret(const std::string& secretId) const {
    if (!m_initialized || !m_database) {
        return std::vector<uint8_t>();
    }

    // Check cache first
    auto it = m_secretCache.find(secretId);
    if (it != m_secretCache.end()) {
        return decryptSecret(it->second.encrypted_data, secretId);
    }

    // Load from database
    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* selectSecret = "SELECT encrypted_data FROM secrets WHERE secret_id = ?";
    
    int rc = sqlite3_prepare_v2(db, selectSecret, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return std::vector<uint8_t>();
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
    
    std::vector<uint8_t> encryptedData;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const void* blob = sqlite3_column_blob(stmt, 0);
        int blobSize = sqlite3_column_bytes(stmt, 0);
        encryptedData.assign(static_cast<const uint8_t*>(blob), 
                            static_cast<const uint8_t*>(blob) + blobSize);
    }
    
    sqlite3_finalize(stmt);

    if (encryptedData.empty()) {
        return std::vector<uint8_t>();
    }

    return decryptSecret(encryptedData, secretId);
}

SecretMetadata SecretStorage::getSecretMetadata(const std::string& secretId) const {
    // Check cache first
    auto it = m_metadataCache.find(secretId);
    if (it != m_metadataCache.end()) {
        return it->second;
    }

    // Load from database
    SecretMetadata metadata;
    if (!m_database) {
        return metadata;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* selectMetadata = "SELECT secret_id, secret_type, description, created_at, updated_at, expires_at, owner, tags, is_encrypted FROM secret_metadata WHERE secret_id = ?";
    
    int rc = sqlite3_prepare_v2(db, selectMetadata, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return metadata;
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        metadata.secret_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        metadata.secret_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        metadata.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        metadata.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        metadata.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        metadata.expires_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        metadata.owner = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        
        // Parse tags
        const char* tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        if (tagsStr) {
            std::istringstream iss(tagsStr);
            std::string tag;
            while (std::getline(iss, tag, ',')) {
                metadata.tags.push_back(tag);
            }
        }
        
        metadata.is_encrypted = sqlite3_column_int(stmt, 8) != 0;
    }
    
    sqlite3_finalize(stmt);

    return metadata;
}

std::vector<std::string> SecretStorage::listSecrets() const {
    std::vector<std::string> secretIds;

    if (!m_database) {
        return secretIds;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* selectIds = "SELECT secret_id FROM secrets";
    
    int rc = sqlite3_prepare_v2(db, selectIds, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return secretIds;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        secretIds.push_back(id);
    }
    
    sqlite3_finalize(stmt);

    return secretIds;
}

bool SecretStorage::deleteSecret(const std::string& secretId) {
    if (!m_database) {
        return false;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    
    // Delete from metadata table first (foreign key constraint)
    sqlite3_stmt* stmt;
    const char* deleteMetadata = "DELETE FROM secret_metadata WHERE secret_id = ?";
    int rc = sqlite3_prepare_v2(db, deleteMetadata, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Delete from secrets table
    const char* deleteSecret = "DELETE FROM secrets WHERE secret_id = ?";
    rc = sqlite3_prepare_v2(db, deleteSecret, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, secretId.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Remove from cache
    m_secretCache.erase(secretId);
    m_metadataCache.erase(secretId);

    return rc == SQLITE_DONE;
}

bool SecretStorage::secretExists(const std::string& secretId) const {
    return m_metadataCache.find(secretId) != m_metadataCache.end() ||
           !getSecretMetadata(secretId).secret_id.empty();
}

std::vector<uint8_t> SecretStorage::encryptSecret(const std::vector<uint8_t>& data,
                                                  const std::string& secretId,
                                                  const SecretMetadata& metadata) const {
    // Simplified encryption using AES-256-GCM
    // In production, use proper key derivation and key management
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return std::vector<uint8_t>();
    }

    // Derive key from secret_id and metadata (simplified - in production use proper key management)
    unsigned char key[32];
    std::string keyMaterial = secretId + metadata.secret_type + metadata.created_at;
    EVP_Digest(keyMaterial.c_str(), keyMaterial.length(), key, nullptr, EVP_sha256(), nullptr);
    
    // Generate random IV
    unsigned char iv[12];
    RAND_bytes(iv, sizeof(iv));

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> ciphertext(data.size() + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
    int len;
    int ciphertextLen = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }
    ciphertextLen = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }
    ciphertextLen += len;

    unsigned char tag[16];
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }

    EVP_CIPHER_CTX_free(ctx);

    // Prepend IV and tag to ciphertext
    std::vector<uint8_t> result;
    result.insert(result.end(), iv, iv + sizeof(iv));
    result.insert(result.end(), tag, tag + sizeof(tag));
    result.insert(result.end(), ciphertext.begin(), ciphertext.begin() + ciphertextLen);

    return result;
}

std::vector<uint8_t> SecretStorage::decryptSecret(const std::vector<uint8_t>& encryptedData,
                                                  const std::string& secretId) const {
    if (encryptedData.size() < 12 + 16) {
        return std::vector<uint8_t>();
    }

    // Extract IV, tag, and ciphertext
    unsigned char iv[12];
    unsigned char tag[16];
    std::copy(encryptedData.begin(), encryptedData.begin() + 12, iv);
    std::copy(encryptedData.begin() + 12, encryptedData.begin() + 28, tag);
    std::vector<uint8_t> ciphertext(encryptedData.begin() + 28, encryptedData.end());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return std::vector<uint8_t>();
    }

    // Get metadata to derive key
    SecretMetadata metadata = getSecretMetadata(secretId);
    if (metadata.secret_id.empty()) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }

    // Derive key from secret_id and metadata (simplified - in production use proper key management)
    // Note: In a full implementation, this would integrate with a secure key management service
    // (e.g., TPM, Hardware Security Module, or dedicated key management service)
    unsigned char key[32];
    std::string keyMaterial = secretId + metadata.secret_type + metadata.created_at;
    EVP_Digest(keyMaterial.c_str(), keyMaterial.length(), key, nullptr, EVP_sha256(), nullptr);

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> plaintext(ciphertext.size());
    int len;
    int plaintextLen = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }
    plaintextLen = len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<uint8_t>();
    }
    plaintextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(plaintextLen);
    return plaintext;
}

void SecretStorage::loadSecrets() {
    // Load metadata into cache
    if (!m_database) {
        return;
    }

    sqlite3* db = static_cast<sqlite3*>(m_database);
    sqlite3_stmt* stmt;
    const char* selectAll = "SELECT secret_id FROM secret_metadata";
    
    int rc = sqlite3_prepare_v2(db, selectAll, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        m_metadataCache[id] = getSecretMetadata(id);
    }
    
    sqlite3_finalize(stmt);
}

