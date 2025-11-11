#include "milos/pqc/hybrid_encryption.h"
#include <algorithm>
#include <iostream>

HybridEncryption::HybridEncryption()
    : m_enabled(false)
    , m_initialized(false)
    , m_performanceThreshold(100 * 1024 * 1024)  // 100 MB default
{
}

HybridEncryption::~HybridEncryption() {
}

bool HybridEncryption::initialize(
    std::unique_ptr<IAlgorithm> pqcAlgorithm,
    std::unique_ptr<IAlgorithm> classicalAlgorithm
) {
    if (!pqcAlgorithm) {
        return false;
    }

    m_pqcAlgorithm = std::move(pqcAlgorithm);
    m_classicalAlgorithm = std::move(classicalAlgorithm);
    m_enabled = true;
    m_initialized = true;
    
    return true;
}

EncryptionResult HybridEncryption::encrypt(
    const std::vector<uint8_t>& plaintext,
    HybridMode mode,
    const std::vector<uint8_t>& publicKey
) {
    if (!m_initialized || !m_pqcAlgorithm) {
        EncryptionResult result;
        result.success = false;
        result.error_message = "Hybrid encryption not initialized";
        return result;
    }

    // Auto mode: select based on performance
    if (mode == HybridMode::AUTO) {
        mode = getRecommendedMode(plaintext.size());
    }

    switch (mode) {
        case HybridMode::PQC_ONLY:
            return encryptPQCOnly(plaintext, publicKey);
        case HybridMode::CLASSICAL_ONLY:
            if (!m_classicalAlgorithm) {
                EncryptionResult result;
                result.success = false;
                result.error_message = "Classical algorithm not available";
                return result;
            }
            return encryptClassicalOnly(plaintext, publicKey);
        case HybridMode::HYBRID:
            return encryptHybrid(plaintext, publicKey);
        default:
            EncryptionResult result;
            result.success = false;
            result.error_message = "Invalid hybrid mode";
            return result;
    }
}

DecryptionResult HybridEncryption::decrypt(
    const std::vector<uint8_t>& ciphertext,
    HybridMode mode,
    const std::vector<uint8_t>& privateKey
) {
    if (!m_initialized || !m_pqcAlgorithm) {
        DecryptionResult result;
        result.success = false;
        result.error_message = "Hybrid encryption not initialized";
        return result;
    }

    // Auto mode: try to detect from ciphertext
    if (mode == HybridMode::AUTO) {
        // Try PQC first, then classical, then hybrid
        // This is a simplified approach - in practice, ciphertext would contain mode information
        mode = HybridMode::PQC_ONLY;
    }

    switch (mode) {
        case HybridMode::PQC_ONLY:
            return m_pqcAlgorithm->decrypt(ciphertext, privateKey);
        case HybridMode::CLASSICAL_ONLY:
            if (!m_classicalAlgorithm) {
                DecryptionResult result;
                result.success = false;
                result.error_message = "Classical algorithm not available";
                return result;
            }
            return m_classicalAlgorithm->decrypt(ciphertext, privateKey);
        case HybridMode::HYBRID:
            // Hybrid decryption: decrypt both and combine
            // Simplified: decrypt PQC portion
            return m_pqcAlgorithm->decrypt(ciphertext, privateKey);
        default:
            DecryptionResult result;
            result.success = false;
            result.error_message = "Invalid hybrid mode";
            return result;
    }
}

HybridMode HybridEncryption::getRecommendedMode(size_t dataSize) const {
    // Performance-based mode selection
    // For large data, use classical for performance
    // For small data, use PQC for security
    if (dataSize > m_performanceThreshold) {
        return HybridMode::CLASSICAL_ONLY;
    } else if (dataSize < 1024) {  // Small data: use PQC
        return HybridMode::PQC_ONLY;
    } else {
        return HybridMode::HYBRID;  // Medium data: use hybrid
    }
}

EncryptionResult HybridEncryption::encryptPQCOnly(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& publicKey
) {
    return m_pqcAlgorithm->encrypt(plaintext, publicKey);
}

EncryptionResult HybridEncryption::encryptClassicalOnly(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& publicKey
) {
    if (!m_classicalAlgorithm) {
        EncryptionResult result;
        result.success = false;
        result.error_message = "Classical algorithm not available";
        return result;
    }
    return m_classicalAlgorithm->encrypt(plaintext, publicKey);
}

EncryptionResult HybridEncryption::encryptHybrid(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& publicKey
) {
    // Hybrid encryption: encrypt with both PQC and classical
    // Strategy: Use PQC for key encapsulation, classical for data encryption
    
    // Step 1: Generate symmetric key
    // Step 2: Encapsulate key with PQC
    EncryptionResult keyResult = m_pqcAlgorithm->encapsulateKey(publicKey);
    if (!keyResult.success) {
        return keyResult;
    }
    
    // Step 3: Encrypt data with classical using encapsulated key
    if (m_classicalAlgorithm) {
        // Use classical algorithm with the encapsulated key
        // This is a simplified implementation
        EncryptionResult dataResult = m_classicalAlgorithm->encrypt(plaintext, keyResult.key);
        
        // Combine results
        EncryptionResult result;
        result.success = dataResult.success;
        if (result.success) {
            // Combine PQC ciphertext (key) and classical ciphertext (data)
            result.ciphertext = dataResult.ciphertext;
            result.key = keyResult.ciphertext;  // PQC-encapsulated key
            result.algorithm_used = "Hybrid(PQC+Classical)";
        } else {
            result.error_message = dataResult.error_message;
        }
        return result;
    } else {
        // Fallback to PQC only if classical not available
        return encryptPQCOnly(plaintext, publicKey);
    }
}

