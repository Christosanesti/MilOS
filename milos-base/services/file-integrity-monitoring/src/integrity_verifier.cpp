#include "integrity_verifier.h"
#include "baseline_manager.h"
#include <ctime>
#include <filesystem>

IntegrityVerifier::IntegrityVerifier()
    : m_initialized(false)
    , m_baselineManager(nullptr)
{
}

IntegrityVerifier::~IntegrityVerifier() {
}

bool IntegrityVerifier::initialize(BaselineManager* baselineManager) {
    if (m_initialized) {
        return true;
    }

    if (!baselineManager) {
        return false;
    }

    m_baselineManager = baselineManager;
    m_initialized = true;
    return true;
}

IntegrityVerificationResult IntegrityVerifier::verifyIntegrity(const std::string& filePath) {
    IntegrityVerificationResult result;
    result.file_path = filePath;
    result.success = false;
    result.is_valid = false;

    if (!m_baselineManager) {
        result.error_message = "Baseline manager not initialized";
        return result;
    }

    // Get baseline for file
    auto baseline = m_baselineManager->getBaselineForFile(filePath);
    if (baseline.baseline_id.empty()) {
        result.error_message = "No baseline found for file";
        result.success = true;  // Successfully checked, but no baseline
        return result;
    }

    result.baseline_id = baseline.baseline_id;

    // Verify baseline
    result.is_valid = m_baselineManager->verifyBaseline(baseline.baseline_id);
    result.success = true;

    // Format verification time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    result.verified_at = timeStr;

    if (!result.is_valid) {
        result.error_message = "File integrity verification failed - file has been modified";
    }

    return result;
}

std::vector<IntegrityVerificationResult> IntegrityVerifier::verifyIntegrityBatch(const std::vector<std::string>& filePaths) {
    std::vector<IntegrityVerificationResult> results;

    for (const auto& filePath : filePaths) {
        results.push_back(verifyIntegrity(filePath));
    }

    return results;
}

