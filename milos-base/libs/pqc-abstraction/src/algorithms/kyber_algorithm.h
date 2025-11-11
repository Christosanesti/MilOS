#ifndef KYBER_ALGORITHM_H
#define KYBER_ALGORITHM_H

#include "milos/pqc/algorithm_interface.h"
#include <string>
#include <vector>
#include <memory>

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

/**
 * @brief CRYSTALS-Kyber Algorithm Implementation
 * 
 * Implements CRYSTALS-Kyber key encapsulation mechanism (KEM)
 * using liboqs library. Supports multiple Kyber variants:
 * - Kyber512 (security level 1)
 * - Kyber768 (security level 3)
 * - Kyber1024 (security level 5)
 */
class KyberAlgorithm : public IAlgorithm {
public:
    /**
     * @brief Constructor
     * @param variant Kyber variant ("Kyber512", "Kyber768", "Kyber1024")
     */
    explicit KyberAlgorithm(const std::string& variant = "Kyber512");

    /**
     * @brief Destructor
     */
    ~KyberAlgorithm() override;

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
    OQS_KEM* m_kem;
#endif

    /**
     * @brief Initialize liboqs KEM
     * @return true if initialization successful, false otherwise
     */
    bool initializeKEM();

    /**
     * @brief Cleanup liboqs KEM
     */
    void cleanupKEM();
};

#endif // KYBER_ALGORITHM_H

