#ifndef DILITHIUM_ALGORITHM_H
#define DILITHIUM_ALGORITHM_H

#include "milos/pqc/algorithm_interface.h"
#include <string>
#include <vector>
#include <memory>

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

/**
 * @brief CRYSTALS-Dilithium Algorithm Implementation
 * 
 * Implements CRYSTALS-Dilithium digital signature scheme
 * using liboqs library. Supports multiple Dilithium variants:
 * - Dilithium2 (security level 2)
 * - Dilithium3 (security level 3)
 * - Dilithium5 (security level 5)
 */
class DilithiumAlgorithm : public IAlgorithm {
public:
    /**
     * @brief Constructor
     * @param variant Dilithium variant ("Dilithium2", "Dilithium3", "Dilithium5")
     */
    explicit DilithiumAlgorithm(const std::string& variant = "Dilithium2");

    /**
     * @brief Destructor
     */
    ~DilithiumAlgorithm() override;

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

#endif // DILITHIUM_ALGORITHM_H

