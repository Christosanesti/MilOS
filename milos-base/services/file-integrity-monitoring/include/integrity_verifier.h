#ifndef INTEGRITY_VERIFIER_H
#define INTEGRITY_VERIFIER_H

#include <string>
#include <vector>

class BaselineManager;

/**
 * @brief Integrity Verification Result
 */
struct IntegrityVerificationResult {
    bool success;
    bool is_valid;
    std::string file_path;
    std::string baseline_id;
    std::string error_message;
    std::string verified_at;
};

/**
 * @brief Integrity Verifier
 * 
 * Verifies file integrity against baselines.
 */
class IntegrityVerifier {
public:
    IntegrityVerifier();
    ~IntegrityVerifier();

    /**
     * @brief Initialize integrity verifier
     * @param baselineManager Baseline manager instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BaselineManager* baselineManager);

    /**
     * @brief Verify file integrity
     * @param filePath File path to verify
     * @return Verification result
     */
    IntegrityVerificationResult verifyIntegrity(const std::string& filePath);

    /**
     * @brief Verify multiple files
     * @param filePaths List of file paths
     * @return List of verification results
     */
    std::vector<IntegrityVerificationResult> verifyIntegrityBatch(const std::vector<std::string>& filePaths);

private:
    bool m_initialized;
    BaselineManager* m_baselineManager;
};

#endif // INTEGRITY_VERIFIER_H

