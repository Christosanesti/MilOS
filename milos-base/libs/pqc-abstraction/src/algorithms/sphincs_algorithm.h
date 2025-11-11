#ifndef SPHINCS_ALGORITHM_H
#define SPHINCS_ALGORITHM_H

#include "milos/pqc/algorithm_interface.h"
#include <string>
#include <vector>
#include <memory>

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

/**
 * @brief SPHINCS+ Algorithm Implementation
 * 
 * Implements SPHINCS+ hash-based signature scheme
 * using liboqs library. Supports multiple SPHINCS+ variants:
 * - SPHINCS+-SHA256-128f-simple
 * - SPHINCS+-SHA256-128s-simple
 * - SPHINCS+-SHA256-192f-simple
 * - SPHINCS+-SHA256-256f-simple
 */
class SPHINCSAlgorithm : public IAlgorithm {
public:
    /**
     * @brief Constructor
     * @param variant SPHINCS+ variant (e.g., "SPHINCS+-SHA256-128f-simple")
     */
    explicit SPHINCSAlgorithm(const std::string& variant = "SPHINCS+-SHA256-128f-simple");

    /**
     * @brief Destructor
     */
    ~SPHINCSAlgorithm() override;

    // IAlgorithm interface implementation
    AlgorithmMetadata getMetadata() const override;
    bool supportsCapability(AlgorithmCapability capability) const override;
    KeyGenerationResult generateKeyPair() override;
    EncryptionResult encapsulateKey(const std::vector<uint8_t>& publicKey) override;
    DecryptionResult decapsulateKey(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& privateKey
    ) override;
    EncryptionResult encrypt(
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& publicKey = {}
    ) override;
    DecryptionResult decrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& privateKey = {}
    ) override;
    SignatureResult sign(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& privateKey
    ) override;
    VerificationResult verify(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& signature,
        const std::vector<uint8_t>& publicKey
    ) override;
    bool isHealthy() const override;
    std::string getVersion() const override;

private:
    std::string m_variant;
    std::string m_algorithmName;  // liboqs algorithm name
    bool m_initialized;
    
#ifdef HAVE_LIBOQS
    // liboqs-specific members
    OQS_SIG* m_sig;
#endif

    /**
     * @brief Initialize liboqs signature scheme
     * @return true if initialization successful, false otherwise
     */
    bool initializeSig();

    /**
     * @brief Cleanup liboqs signature scheme
     */
    void cleanupSig();
};

#endif // SPHINCS_ALGORITHM_H

