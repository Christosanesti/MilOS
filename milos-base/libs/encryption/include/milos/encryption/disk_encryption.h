#ifndef DISK_ENCRYPTION_H
#define DISK_ENCRYPTION_H

#include <string>
#include <vector>
#include <memory>

/**
 * @brief Disk Encryption Result
 */
struct DiskEncryptionResult {
    bool success;
    std::string error_message;
    std::string device_path;
    std::string algorithm_used;
};

/**
 * @brief Disk Encryption Library
 * 
 * Provides disk encryption operations using LUKS2
 * with PQC algorithms for key derivation.
 */
class DiskEncryption {
public:
    DiskEncryption();
    ~DiskEncryption();

    /**
     * @brief Initialize disk encryption
     * @param pqcAlgorithmId PQC algorithm ID for key derivation (optional)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& pqcAlgorithmId = "");

    /**
     * @brief Format device with LUKS2 encryption
     * @param devicePath Path to device (e.g., /dev/sda1)
     * @param password Password for encryption
     * @param keySize Key size in bits (default: 512)
     * @return Encryption result
     */
    DiskEncryptionResult formatDevice(
        const std::string& devicePath,
        const std::string& password,
        size_t keySize = 512
    );

    /**
     * @brief Open encrypted device
     * @param devicePath Path to encrypted device
     * @param password Password
     * @param mapperName Mapper device name (e.g., "cryptroot")
     * @return true if open successful, false otherwise
     */
    bool openDevice(
        const std::string& devicePath,
        const std::string& password,
        const std::string& mapperName
    );

    /**
     * @brief Close encrypted device
     * @param mapperName Mapper device name
     * @return true if close successful, false otherwise
     */
    bool closeDevice(const std::string& mapperName);

    /**
     * @brief Check if device is encrypted
     * @param devicePath Path to device
     * @return true if encrypted, false otherwise
     */
    bool isDeviceEncrypted(const std::string& devicePath) const;

    /**
     * @brief Check if disk encryption is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized;
    std::string m_pqcAlgorithmId;

    /**
     * @brief Derive key from password using PQC
     */
    std::vector<uint8_t> deriveKeyFromPassword(
        const std::string& password,
        const std::vector<uint8_t>& salt,
        size_t keySize
    );
};

#endif // DISK_ENCRYPTION_H

