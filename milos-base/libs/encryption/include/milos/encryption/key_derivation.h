#ifndef KEY_DERIVATION_H
#define KEY_DERIVATION_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Key Derivation Algorithm
 */
enum class KeyDerivationAlgorithm {
    PBKDF2_PQC,    // PBKDF2 with PQC hash
    ARGON2_PQC,    // Argon2 with PQC hash
    HKDF_PQC       // HKDF with PQC hash
};

/**
 * @brief Key Derivation Result
 */
struct KeyDerivationResult {
    bool success;
    std::vector<uint8_t> derived_key;
    std::string error_message;
    std::string algorithm_used;
};

/**
 * @brief Key Derivation Library
 * 
 * Provides quantum-resistant key derivation using PQC algorithms.
 * Supports PBKDF2, Argon2, and HKDF with PQC hash functions.
 */
class KeyDerivation {
public:
    KeyDerivation();
    ~KeyDerivation();

    /**
     * @brief Initialize key derivation
     * @param algorithm Key derivation algorithm
     * @param pqcHashAlgorithm PQC hash algorithm ID (optional)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        KeyDerivationAlgorithm algorithm = KeyDerivationAlgorithm::PBKDF2_PQC,
        const std::string& pqcHashAlgorithm = ""
    );

    /**
     * @brief Derive key from password
     * @param password Password
     * @param salt Salt (should be random)
     * @param keyLength Desired key length in bytes
     * @param iterations Number of iterations (for PBKDF2)
     * @return Key derivation result
     */
    KeyDerivationResult deriveFromPassword(
        const std::string& password,
        const std::vector<uint8_t>& salt,
        size_t keyLength,
        uint32_t iterations = 100000
    );

    /**
     * @brief Derive key from password (vector)
     * @param password Password as byte vector
     * @param salt Salt (should be random)
     * @param keyLength Desired key length in bytes
     * @param iterations Number of iterations (for PBKDF2)
     * @return Key derivation result
     */
    KeyDerivationResult deriveFromPassword(
        const std::vector<uint8_t>& password,
        const std::vector<uint8_t>& salt,
        size_t keyLength,
        uint32_t iterations = 100000
    );

    /**
     * @brief Derive key using HKDF
     * @param inputKeyMaterial Input key material
     * @param salt Salt
     * @param info Application-specific information
     * @param keyLength Desired key length in bytes
     * @return Key derivation result
     */
    KeyDerivationResult deriveHKDF(
        const std::vector<uint8_t>& inputKeyMaterial,
        const std::vector<uint8_t>& salt,
        const std::vector<uint8_t>& info,
        size_t keyLength
    );

    /**
     * @brief Generate random salt
     * @param saltLength Salt length in bytes
     * @return Random salt
     */
    static std::vector<uint8_t> generateSalt(size_t saltLength = 32);

    /**
     * @brief Check if key derivation is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized;
    KeyDerivationAlgorithm m_algorithm;
    std::string m_pqcHashAlgorithm;

    /**
     * @brief Derive key using PBKDF2 with PQC hash
     */
    KeyDerivationResult derivePBKDF2(
        const std::vector<uint8_t>& password,
        const std::vector<uint8_t>& salt,
        size_t keyLength,
        uint32_t iterations
    );

    /**
     * @brief Hash data using PQC hash algorithm
     */
    std::vector<uint8_t> hashPQC(const std::vector<uint8_t>& data);
};

#endif // KEY_DERIVATION_H

