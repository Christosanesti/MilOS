#ifndef SECRET_STORAGE_H
#define SECRET_STORAGE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * @brief Secret Metadata
 */
struct SecretMetadata {
    std::string secret_id;
    std::string secret_type;  // password, api_key, certificate, etc.
    std::string description;
    std::string created_at;
    std::string updated_at;
    std::string expires_at;
    std::string owner;
    std::vector<std::string> tags;
    bool is_encrypted;
};

/**
 * @brief Secret Data
 */
struct SecretData {
    std::string secret_id;
    std::vector<uint8_t> encrypted_data;
    std::string encryption_algorithm;
    std::string version;
    std::string created_at;
};

/**
 * @brief Secret Storage
 * 
 * Manages encrypted secret storage and retrieval.
 */
class SecretStorage {
public:
    SecretStorage();
    ~SecretStorage();

    /**
     * @brief Initialize secret storage
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Store secret
     * @param secretId Secret ID
     * @param secretData Secret data (will be encrypted)
     * @param metadata Secret metadata
     * @return true if storage successful, false otherwise
     */
    bool storeSecret(const std::string& secretId,
                    const std::vector<uint8_t>& secretData,
                    const SecretMetadata& metadata);

    /**
     * @brief Get secret
     * @param secretId Secret ID
     * @return Secret data (decrypted), or empty if not found
     */
    std::vector<uint8_t> getSecret(const std::string& secretId) const;

    /**
     * @brief Get secret metadata
     * @param secretId Secret ID
     * @return Secret metadata, or empty if not found
     */
    SecretMetadata getSecretMetadata(const std::string& secretId) const;

    /**
     * @brief List all secrets
     * @return List of secret IDs
     */
    std::vector<std::string> listSecrets() const;

    /**
     * @brief Delete secret
     * @param secretId Secret ID
     * @return true if deletion successful, false otherwise
     */
    bool deleteSecret(const std::string& secretId);

    /**
     * @brief Check if secret exists
     * @param secretId Secret ID
     * @return true if exists, false otherwise
     */
    bool secretExists(const std::string& secretId) const;

    /**
     * @brief Encrypt secret data (public for versioning)
     * @param data Plaintext data
     * @return Encrypted data
     */
    std::vector<uint8_t> encryptSecret(const std::vector<uint8_t>& data) const;

    /**
     * @brief Decrypt secret data (public for versioning)
     * @param encryptedData Encrypted data
     * @return Plaintext data
     */
    std::vector<uint8_t> decryptSecret(const std::vector<uint8_t>& encryptedData) const;

private:
    bool m_initialized;
    std::string m_databasePath;
    void* m_database;  // sqlite3* pointer
    
    // In-memory cache
    std::map<std::string, SecretMetadata> m_metadataCache;
    std::map<std::string, SecretData> m_secretCache;

    /**
     * @brief Initialize database
     * @return true if successful, false otherwise
     */
    bool initializeDatabase();

    /**
     * @brief Load secrets from database
     */
    void loadSecrets();
};

#endif // SECRET_STORAGE_H

