#ifndef FILE_ENCRYPTION_H
#define FILE_ENCRYPTION_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

// Forward declarations
namespace milos {
namespace pqc {
    class IAlgorithm;
    class HybridEncryption;
}
}

/**
 * @brief File Encryption Progress Callback
 * 
 * @param bytesProcessed Number of bytes processed
 * @param totalBytes Total number of bytes to process
 * @param percentage Progress percentage (0-100)
 */
using FileEncryptionProgressCallback = std::function<void(size_t bytesProcessed, size_t totalBytes, double percentage)>;

/**
 * @brief File Encryption Result
 */
struct FileEncryptionResult {
    bool success;
    std::string error_message;
    std::string encrypted_file_path;
    size_t encrypted_size;
    std::string algorithm_used;
};

/**
 * @brief File Decryption Result
 */
struct FileDecryptionResult {
    bool success;
    std::string error_message;
    std::string decrypted_file_path;
    size_t decrypted_size;
    std::string algorithm_used;
};

/**
 * @brief File Encryption Library
 * 
 * Provides high-level file encryption/decryption operations
 * using the PQC abstraction layer. Supports streaming encryption
 * for large files and hybrid encryption modes.
 */
class FileEncryption {
public:
    FileEncryption();
    ~FileEncryption();

    /**
     * @brief Initialize file encryption
     * @param algorithmId Algorithm ID (optional, uses default if empty)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& algorithmId = "");

    /**
     * @brief Encrypt file
     * @param inputPath Path to input file
     * @param outputPath Path to output encrypted file
     * @param publicKey Public key for asymmetric encryption (optional)
     * @param progressCallback Progress callback (optional)
     * @return Encryption result
     */
    FileEncryptionResult encryptFile(
        const std::string& inputPath,
        const std::string& outputPath,
        const std::vector<uint8_t>& publicKey = {},
        FileEncryptionProgressCallback progressCallback = nullptr
    );

    /**
     * @brief Decrypt file
     * @param inputPath Path to input encrypted file
     * @param outputPath Path to output decrypted file
     * @param privateKey Private key for asymmetric decryption (optional)
     * @param progressCallback Progress callback (optional)
     * @return Decryption result
     */
    FileDecryptionResult decryptFile(
        const std::string& inputPath,
        const std::string& outputPath,
        const std::vector<uint8_t>& privateKey = {},
        FileEncryptionProgressCallback progressCallback = nullptr
    );

    /**
     * @brief Encrypt file data in memory
     * @param data Data to encrypt
     * @param publicKey Public key for asymmetric encryption (optional)
     * @return Encrypted data
     */
    std::vector<uint8_t> encryptData(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& publicKey = {}
    );

    /**
     * @brief Decrypt file data in memory
     * @param encryptedData Encrypted data
     * @param privateKey Private key for asymmetric decryption (optional)
     * @return Decrypted data
     */
    std::vector<uint8_t> decryptData(
        const std::vector<uint8_t>& encryptedData,
        const std::vector<uint8_t>& privateKey = {}
    );

    /**
     * @brief Set hybrid encryption mode
     * @param enabled Enable hybrid encryption
     */
    void setHybridEncryption(bool enabled) { m_hybridEnabled = enabled; }

    /**
     * @brief Check if hybrid encryption is enabled
     * @return true if enabled, false otherwise
     */
    bool isHybridEncryptionEnabled() const { return m_hybridEnabled; }

    /**
     * @brief Check if file encryption is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized;
    bool m_hybridEnabled;
    std::string m_algorithmId;
    std::unique_ptr<milos::pqc::IAlgorithm> m_algorithm;
    std::unique_ptr<milos::pqc::HybridEncryption> m_hybridEncryption;

    /**
     * @brief Initialize algorithm from factory
     * @return true if initialization successful, false otherwise
     */
    bool initializeAlgorithm();

    /**
     * @brief Encrypt data chunk
     * @param data Data chunk to encrypt
     * @param publicKey Public key (optional)
     * @return Encrypted data chunk
     */
    std::vector<uint8_t> encryptChunk(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& publicKey = {}
    );

    /**
     * @brief Decrypt data chunk
     * @param encryptedData Encrypted data chunk
     * @param privateKey Private key (optional)
     * @return Decrypted data chunk
     */
    std::vector<uint8_t> decryptChunk(
        const std::vector<uint8_t>& encryptedData,
        const std::vector<uint8_t>& privateKey = {}
    );
};

#endif // FILE_ENCRYPTION_H

