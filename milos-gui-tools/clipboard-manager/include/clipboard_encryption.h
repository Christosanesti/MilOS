#ifndef CLIPBOARD_ENCRYPTION_H
#define CLIPBOARD_ENCRYPTION_H

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
 * @brief Clipboard Encryption
 * 
 * Handles encryption and decryption of clipboard data using PQC algorithms.
 */
class ClipboardEncryption {
public:
    ClipboardEncryption();
    ~ClipboardEncryption();

    /**
     * @brief Initialize encryption
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Encrypt clipboard data
     * @param data Plaintext data
     * @return Encrypted data
     */
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);

    /**
     * @brief Decrypt clipboard data
     * @param encryptedData Encrypted data
     * @return Plaintext data
     */
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encryptedData);

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
     * @brief Generate or load encryption key
     */
    bool initializeKey();
};

#endif // CLIPBOARD_ENCRYPTION_H

