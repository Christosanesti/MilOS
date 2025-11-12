#ifndef BACKUP_ENCRYPTION_H
#define BACKUP_ENCRYPTION_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

// Forward declaration
namespace milos {
namespace pqc {
class IAlgorithm;
}
}

/**
 * @brief Backup Encryption
 * 
 * Handles zero-knowledge encryption of backup data using PQC algorithms.
 * All encryption is done client-side before upload.
 */
class BackupEncryption {
public:
    BackupEncryption();
    ~BackupEncryption();

    /**
     * @brief Initialize encryption
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Encrypt backup data (client-side)
     * @param data Plaintext backup data
     * @return Encrypted data (server cannot decrypt)
     */
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);

    /**
     * @brief Decrypt backup data (client-side)
     * @param encryptedData Encrypted backup data
     * @return Plaintext data
     */
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encryptedData);

    /**
     * @brief Generate encryption key (never sent to server)
     * @return Encryption key
     */
    std::vector<uint8_t> generateKey();

    /**
     * @brief Verify zero-knowledge architecture
     * @return true if zero-knowledge verified, false otherwise
     */
    bool verifyZeroKnowledge() const;

    /**
     * @brief Check if encryption is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized;
    std::unique_ptr<milos::pqc::IAlgorithm> m_algorithm;
    std::vector<uint8_t> m_encryptionKey;

    /**
     * @brief Generate or load encryption key (client-side only)
     */
    bool initializeKey();
};

#endif // BACKUP_ENCRYPTION_H

