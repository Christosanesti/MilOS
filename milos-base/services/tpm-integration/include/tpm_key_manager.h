#ifndef TPM_KEY_MANAGER_H
#define TPM_KEY_MANAGER_H

#include <string>
#include <vector>
#include <memory>

class TPMDevice;

/**
 * @brief TPM Key Information
 */
struct TPMKeyInfo {
    std::string key_id;
    std::string key_type;  // RSA, ECC, etc.
    uint32_t persistent_handle;
    std::string public_key;
    std::string created_at;
    bool is_persistent;
};

/**
 * @brief Key Generation Parameters
 */
struct KeyGenerationParams {
    std::string key_type;  // "RSA2048", "ECC256", etc.
    std::string key_usage;  // "signing", "encryption", "storage"
    bool persistent;       // Store key persistently in TPM
    std::string key_auth;  // Authorization value (optional)
};

/**
 * @brief TPM Key Manager
 * 
 * Manages key generation, storage, and retrieval in TPM.
 */
class TPMKeyManager {
public:
    TPMKeyManager();
    ~TPMKeyManager();

    /**
     * @brief Initialize key manager
     * @param tpmDevice TPM device instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(TPMDevice* tpmDevice);

    /**
     * @brief Generate key in TPM
     * @param params Key generation parameters
     * @return Key information, or empty if failed
     */
    TPMKeyInfo generateKey(const KeyGenerationParams& params);

    /**
     * @brief Get key information
     * @param keyId Key ID
     * @return Key information, or empty if not found
     */
    TPMKeyInfo getKey(const std::string& keyId) const;

    /**
     * @brief List all keys
     * @return List of key information
     */
    std::vector<TPMKeyInfo> listKeys() const;

    /**
     * @brief Delete key from TPM
     * @param keyId Key ID
     * @return true if deletion successful, false otherwise
     */
    bool deleteKey(const std::string& keyId);

private:
    bool m_initialized;
    TPMDevice* m_tpmDevice;
    std::vector<TPMKeyInfo> m_keys;

    /**
     * @brief Generate persistent handle
     * @return Persistent handle value
     */
    uint32_t generatePersistentHandle() const;
};

#endif // TPM_KEY_MANAGER_H

