#ifndef HYBRID_ENCRYPTION_H
#define HYBRID_ENCRYPTION_H

#include "algorithm_interface.h"
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Hybrid Encryption Mode
 */
enum class HybridMode {
    PQC_ONLY,        // Post-quantum only
    CLASSICAL_ONLY,  // Classical only
    HYBRID,          // Both PQC and classical
    AUTO             // Automatic selection based on performance
};

/**
 * @brief Hybrid Encryption Manager
 * 
 * Manages hybrid encryption modes combining post-quantum and classical encryption.
 */
class HybridEncryption {
public:
    HybridEncryption();
    ~HybridEncryption();

    /**
     * @brief Initialize hybrid encryption
     * @param pqcAlgorithm PQC algorithm instance
     * @param classicalAlgorithm Classical algorithm instance (optional)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        std::unique_ptr<IAlgorithm> pqcAlgorithm,
        std::unique_ptr<IAlgorithm> classicalAlgorithm = nullptr
    );

    /**
     * @brief Encrypt data using hybrid mode
     * @param plaintext Data to encrypt
     * @param mode Hybrid encryption mode
     * @param publicKey Public key (for asymmetric encryption)
     * @return Encryption result
     */
    EncryptionResult encrypt(
        const std::vector<uint8_t>& plaintext,
        HybridMode mode = HybridMode::HYBRID,
        const std::vector<uint8_t>& publicKey = {}
    );

    /**
     * @brief Decrypt data using hybrid mode
     * @param ciphertext Encrypted data
     * @param mode Hybrid encryption mode used for encryption
     * @param privateKey Private key (for asymmetric decryption)
     * @return Decryption result
     */
    DecryptionResult decrypt(
        const std::vector<uint8_t>& ciphertext,
        HybridMode mode = HybridMode::HYBRID,
        const std::vector<uint8_t>& privateKey = {}
    );

    /**
     * @brief Get recommended mode based on performance
     * @param dataSize Data size in bytes
     * @return Recommended hybrid mode
     */
    HybridMode getRecommendedMode(size_t dataSize) const;

    /**
     * @brief Check if hybrid mode is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return m_enabled; }

private:
    bool m_enabled;
    bool m_initialized;
    std::unique_ptr<IAlgorithm> m_pqcAlgorithm;
    std::unique_ptr<IAlgorithm> m_classicalAlgorithm;
    
    size_t m_performanceThreshold;  // Threshold for mode selection
    
    /**
     * @brief Encrypt with PQC only
     */
    EncryptionResult encryptPQCOnly(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& publicKey);
    
    /**
     * @brief Encrypt with classical only
     */
    EncryptionResult encryptClassicalOnly(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& publicKey);
    
    /**
     * @brief Encrypt with hybrid mode
     */
    EncryptionResult encryptHybrid(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& publicKey);
};

#endif // HYBRID_ENCRYPTION_H

