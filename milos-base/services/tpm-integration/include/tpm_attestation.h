#ifndef TPM_ATTESTATION_H
#define TPM_ATTESTATION_H

#include <string>
#include <vector>
#include <memory>

class TPMDevice;

/**
 * @brief Attestation Quote
 */
struct AttestationQuote {
    std::string quote_data;
    std::string signature;
    std::string nonce;
    std::vector<uint32_t> pcr_indices;
    std::string algorithm;
    std::string created_at;
};

/**
 * @brief Attestation Verification Result
 */
struct AttestationVerificationResult {
    bool success;
    bool is_valid;
    std::string error_message;
    std::string verified_at;
};

/**
 * @brief TPM Attestation
 * 
 * Provides TPM attestation capabilities (quote generation and verification).
 */
class TPMAttestation {
public:
    TPMAttestation();
    ~TPMAttestation();

    /**
     * @brief Initialize attestation
     * @param tpmDevice TPM device instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(TPMDevice* tpmDevice);

    /**
     * @brief Generate attestation quote
     * @param nonce Nonce for quote (optional, will be generated if empty)
     * @param pcrIndices PCR indices to include in quote
     * @return Attestation quote
     */
    AttestationQuote generateAttestation(const std::string& nonce = "",
                                        const std::vector<uint32_t>& pcrIndices = {});

    /**
     * @brief Verify attestation quote
     * @param quote Attestation quote to verify
     * @return Verification result
     */
    AttestationVerificationResult verifyAttestation(const AttestationQuote& quote);

private:
    bool m_initialized;
    TPMDevice* m_tpmDevice;

    /**
     * @brief Generate random nonce
     * @param length Nonce length in bytes
     * @return Nonce as hex string
     */
    std::string generateNonce(size_t length = 32) const;
};

#endif // TPM_ATTESTATION_H

