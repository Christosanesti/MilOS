#ifndef SECURE_BOOT_MANAGER_H
#define SECURE_BOOT_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

class TPMDevice;
class TPMAttestation;

/**
 * @brief Boot Measurement
 */
struct BootMeasurement {
    std::vector<uint32_t> pcr_indices;
    std::map<uint32_t, std::vector<uint8_t>> pcr_values;
    std::string algorithm;
    std::string measured_at;
    bool is_valid;
};

/**
 * @brief Secure Boot Policy
 */
struct SecureBootPolicy {
    std::string policy_id;
    std::vector<uint32_t> required_pcrs;
    std::map<uint32_t, std::vector<uint8_t>> expected_pcr_values;  // Expected PCR values
    bool enforce_policy;
    bool allow_pcr_changes;
    std::string description;
};

/**
 * @brief Boot Integrity Verification Result
 */
struct BootIntegrityResult {
    bool is_valid;
    bool policy_compliant;
    std::vector<uint32_t> mismatched_pcrs;
    std::string verification_time;
    std::string error_message;
};

/**
 * @brief Secure Boot Manager
 * 
 * Manages TPM-based secure boot measurement, attestation, and integrity verification.
 */
class SecureBootManager {
public:
    SecureBootManager();
    ~SecureBootManager();

    /**
     * @brief Initialize secure boot manager
     * @param tpmDevice TPM device instance
     * @param attestation TPM attestation instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(TPMDevice* tpmDevice, TPMAttestation* attestation);

    /**
     * @brief Collect boot measurement (PCR registers)
     * @param pcrIndices PCR indices to collect (empty for default boot PCRs 0-7)
     * @return Boot measurement
     */
    BootMeasurement collectBootMeasurement(const std::vector<uint32_t>& pcrIndices = {});

    /**
     * @brief Generate boot attestation
     * @param nonce Nonce for attestation (optional)
     * @return Attestation quote
     */
    std::string generateBootAttestation(const std::string& nonce = "");

    /**
     * @brief Verify boot integrity
     * @param policy Secure boot policy to verify against
     * @return Boot integrity verification result
     */
    BootIntegrityResult verifyBootIntegrity(const SecureBootPolicy& policy);

    /**
     * @brief Verify boot integrity against default policy
     * @return Boot integrity verification result
     */
    BootIntegrityResult verifyBootIntegrity();

    /**
     * @brief Set secure boot policy
     * @param policy Secure boot policy
     */
    void setSecureBootPolicy(const SecureBootPolicy& policy);

    /**
     * @brief Get secure boot policy
     * @param policyId Policy ID (empty for default)
     * @return Secure boot policy
     */
    SecureBootPolicy getSecureBootPolicy(const std::string& policyId = "") const;

    /**
     * @brief Check if secure boot is enabled
     * @return true if secure boot enabled, false otherwise
     */
    bool isSecureBootEnabled() const;

    /**
     * @brief Get current boot measurement
     * @return Current boot measurement
     */
    BootMeasurement getCurrentBootMeasurement() const;

    /**
     * @brief Store boot measurement baseline
     * @param measurement Boot measurement to store as baseline
     * @return true if storage successful, false otherwise
     */
    bool storeBootBaseline(const BootMeasurement& measurement);

    /**
     * @brief Get stored boot baseline
     * @return Stored boot baseline, or empty if not found
     */
    BootMeasurement getBootBaseline() const;

private:
    bool m_initialized;
    TPMDevice* m_tpmDevice;
    TPMAttestation* m_attestation;
    SecureBootPolicy m_defaultPolicy;
    std::map<std::string, SecureBootPolicy> m_policies;
    BootMeasurement m_currentMeasurement;
    BootMeasurement m_baselineMeasurement;

    /**
     * @brief Initialize default secure boot policy
     */
    void initializeDefaultPolicy();

    /**
     * @brief Check UEFI secure boot status
     * @return true if UEFI secure boot enabled, false otherwise
     */
    bool checkUEFISecureBoot() const;

    /**
     * @brief Compare PCR values
     * @param expected Expected PCR values
     * @param actual Actual PCR values
     * @return List of mismatched PCR indices
     */
    std::vector<uint32_t> comparePCRValues(const std::map<uint32_t, std::vector<uint8_t>>& expected,
                                           const std::map<uint32_t, std::vector<uint8_t>>& actual) const;
};

#endif // SECURE_BOOT_MANAGER_H

