#ifndef ALGORITHM_INTERFACE_H
#define ALGORITHM_INTERFACE_H

#include <string>
#include <vector>
#include <memory>

/**
 * @brief Algorithm Capabilities
 */
enum class AlgorithmCapability {
    KEY_ENCAPSULATION,    // KEM (Key Encapsulation Mechanism)
    DIGITAL_SIGNATURE,    // Digital signatures
    HASH_BASED_SIGNATURE, // Hash-based signatures
    SYMMETRIC_ENCRYPTION  // Symmetric encryption
};

/**
 * @brief Algorithm Metadata
 */
struct AlgorithmMetadata {
    std::string algorithm_id;
    std::string name;
    std::string version;
    std::string description;
    std::vector<AlgorithmCapability> capabilities;
    bool is_nist_approved;
    bool is_hybrid_supported;
    size_t key_size;
    size_t ciphertext_overhead;
    double performance_score;  // Relative performance metric
};

/**
 * @brief Encryption Result
 */
struct EncryptionResult {
    bool success;
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> key;  // For KEM: encapsulated key
    std::string error_message;
    std::string algorithm_used;
    std::string algorithm_version;
};

/**
 * @brief Decryption Result
 */
struct DecryptionResult {
    bool success;
    std::vector<uint8_t> plaintext;
    std::vector<uint8_t> key;  // For KEM: decapsulated key
    std::string error_message;
    std::string algorithm_used;
    std::string algorithm_version;
};

/**
 * @brief Signature Result
 */
struct SignatureResult {
    bool success;
    std::vector<uint8_t> signature;
    std::string error_message;
    std::string algorithm_used;
    std::string algorithm_version;
};

/**
 * @brief Verification Result
 */
struct VerificationResult {
    bool success;
    bool verified;
    std::string error_message;
    std::string algorithm_used;
    std::string algorithm_version;
};

/**
 * @brief Key Generation Result
 */
struct KeyGenerationResult {
    bool success;
    std::vector<uint8_t> public_key;
    std::vector<uint8_t> private_key;
    std::string error_message;
    std::string algorithm_used;
    std::string algorithm_version;
};

/**
 * @brief Abstract Algorithm Interface
 * 
 * This interface defines the contract for all PQC algorithms.
 * Algorithms implement this interface to provide encryption, decryption,
 * key generation, signing, and verification operations.
 */
class IAlgorithm {
public:
    virtual ~IAlgorithm() = default;

    /**
     * @brief Get algorithm metadata
     * @return Algorithm metadata
     */
    virtual AlgorithmMetadata getMetadata() const = 0;

    /**
     * @brief Check if algorithm supports a capability
     * @param capability Algorithm capability
     * @return true if supported, false otherwise
     */
    virtual bool supportsCapability(AlgorithmCapability capability) const = 0;

    /**
     * @brief Generate key pair
     * @return Key generation result
     */
    virtual KeyGenerationResult generateKeyPair() = 0;

    /**
     * @brief Encapsulate key (for KEM algorithms)
     * @param publicKey Public key
     * @return Encryption result with encapsulated key
     */
    virtual EncryptionResult encapsulateKey(const std::vector<uint8_t>& publicKey) = 0;

    /**
     * @brief Decapsulate key (for KEM algorithms)
     * @param ciphertext Encapsulated key
     * @param privateKey Private key
     * @return Decryption result with decapsulated key
     */
    virtual DecryptionResult decapsulateKey(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& privateKey
    ) = 0;

    /**
     * @brief Encrypt data
     * @param plaintext Data to encrypt
     * @param publicKey Public key (for asymmetric encryption)
     * @return Encryption result
     */
    virtual EncryptionResult encrypt(
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& publicKey = {}
    ) = 0;

    /**
     * @brief Decrypt data
     * @param ciphertext Encrypted data
     * @param privateKey Private key (for asymmetric decryption)
     * @return Decryption result
     */
    virtual DecryptionResult decrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& privateKey = {}
    ) = 0;

    /**
     * @brief Sign data
     * @param data Data to sign
     * @param privateKey Private key
     * @return Signature result
     */
    virtual SignatureResult sign(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& privateKey
    ) = 0;

    /**
     * @brief Verify signature
     * @param data Original data
     * @param signature Signature to verify
     * @param publicKey Public key
     * @return Verification result
     */
    virtual VerificationResult verify(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& signature,
        const std::vector<uint8_t>& publicKey
    ) = 0;

    /**
     * @brief Check if algorithm is healthy
     * @return true if healthy, false otherwise
     */
    virtual bool isHealthy() const = 0;

    /**
     * @brief Get algorithm version
     * @return Algorithm version string
     */
    virtual std::string getVersion() const = 0;
};

#endif // ALGORITHM_INTERFACE_H

