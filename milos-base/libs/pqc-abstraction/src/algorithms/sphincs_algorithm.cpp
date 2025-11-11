#include "sphincs_algorithm.h"
#include <stdexcept>
#include <cstring>
#include <iostream>

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

SPHINCSAlgorithm::SPHINCSAlgorithm(const std::string& variant)
    : m_variant(variant)
    , m_initialized(false)
#ifdef HAVE_LIBOQS
    , m_sig(nullptr)
#endif
{
    // Map variant to liboqs algorithm name
    // Use variant as-is, assuming it matches liboqs naming
    m_algorithmName = variant;

#ifdef HAVE_LIBOQS
    if (!initializeSig()) {
        std::cerr << "Failed to initialize SPHINCS+ algorithm: " << m_algorithmName << std::endl;
    }
#else
    std::cerr << "Warning: liboqs not available. SPHINCS+ algorithm will not function." << std::endl;
#endif
}

SPHINCSAlgorithm::~SPHINCSAlgorithm() {
#ifdef HAVE_LIBOQS
    cleanupSig();
#endif
}

#ifdef HAVE_LIBOQS
bool SPHINCSAlgorithm::initializeSig() {
    if (m_initialized) {
        return true;
    }

    m_sig = OQS_SIG_new(m_algorithmName.c_str());
    if (m_sig == nullptr) {
        std::cerr << "Error: OQS_SIG_new failed for " << m_algorithmName << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void SPHINCSAlgorithm::cleanupSig() {
    if (m_sig != nullptr) {
        OQS_SIG_free(m_sig);
        m_sig = nullptr;
    }
    m_initialized = false;
}
#endif

AlgorithmMetadata SPHINCSAlgorithm::getMetadata() const {
    AlgorithmMetadata metadata;
    metadata.algorithm_id = m_variant;
    metadata.name = "SPHINCS+";
    metadata.version = "1.0.0";
    metadata.description = "SPHINCS+ hash-based signature scheme - " + m_variant;
    metadata.capabilities.push_back(AlgorithmCapability::HASH_BASED_SIGNATURE);
    metadata.is_nist_approved = true;  // NIST standardized
    metadata.is_hybrid_supported = true;
    
#ifdef HAVE_LIBOQS
    if (m_sig != nullptr) {
        metadata.key_size = m_sig->length_public_key;
        metadata.ciphertext_overhead = m_sig->length_signature;
    } else {
        metadata.key_size = 0;
        metadata.ciphertext_overhead = 0;
    }
#else
    metadata.key_size = 0;
    metadata.ciphertext_overhead = 0;
#endif
    
    metadata.performance_score = 0.8;  // SPHINCS+ is slower than Dilithium
    return metadata;
}

bool SPHINCSAlgorithm::supportsCapability(AlgorithmCapability capability) const {
    return capability == AlgorithmCapability::HASH_BASED_SIGNATURE;
}

KeyGenerationResult SPHINCSAlgorithm::generateKeyPair() {
    KeyGenerationResult result;
    
#ifdef HAVE_LIBOQS
    if (!m_initialized || m_sig == nullptr) {
        result.success = false;
        result.error_message = "Algorithm not initialized";
        return result;
    }

    size_t publicKeyLen = m_sig->length_public_key;
    size_t privateKeyLen = m_sig->length_secret_key;

    result.public_key.resize(publicKeyLen);
    result.private_key.resize(privateKeyLen);

    uint8_t* publicKeyPtr = result.public_key.data();
    uint8_t* privateKeyPtr = result.private_key.data();

    OQS_STATUS status = OQS_SIG_keypair(m_sig, publicKeyPtr, privateKeyPtr);
    if (status != OQS_SUCCESS) {
        result.success = false;
        result.error_message = "Key pair generation failed";
        result.public_key.clear();
        result.private_key.clear();
        return result;
    }

    result.success = true;
    result.algorithm_used = m_variant;
    result.algorithm_version = "1.0.0";
#else
    result.success = false;
    result.error_message = "liboqs not available";
#endif

    return result;
}

SignatureResult SPHINCSAlgorithm::sign(
    const std::vector<uint8_t>& data,
    const std::vector<uint8_t>& privateKey
) {
    SignatureResult result;
    
#ifdef HAVE_LIBOQS
    if (!m_initialized || m_sig == nullptr) {
        result.success = false;
        result.error_message = "Algorithm not initialized";
        return result;
    }

    if (privateKey.size() != m_sig->length_secret_key) {
        result.success = false;
        result.error_message = "Invalid private key size";
        return result;
    }

    size_t signatureLen = m_sig->length_signature;
    result.signature.resize(signatureLen);

    size_t signatureLenActual = 0;
    OQS_STATUS status = OQS_SIG_sign(m_sig, result.signature.data(), &signatureLenActual,
                                     data.data(), data.size(), privateKey.data());
    if (status != OQS_SUCCESS) {
        result.success = false;
        result.error_message = "Signature generation failed";
        result.signature.clear();
        return result;
    }

    result.signature.resize(signatureLenActual);
    result.success = true;
    result.algorithm_used = m_variant;
    result.algorithm_version = "1.0.0";
#else
    result.success = false;
    result.error_message = "liboqs not available";
#endif

    return result;
}

VerificationResult SPHINCSAlgorithm::verify(
    const std::vector<uint8_t>& data,
    const std::vector<uint8_t>& signature,
    const std::vector<uint8_t>& publicKey
) {
    VerificationResult result;
    
#ifdef HAVE_LIBOQS
    if (!m_initialized || m_sig == nullptr) {
        result.success = false;
        result.error_message = "Algorithm not initialized";
        return result;
    }

    if (publicKey.size() != m_sig->length_public_key) {
        result.success = false;
        result.error_message = "Invalid public key size";
        return result;
    }

    OQS_STATUS status = OQS_SIG_verify(m_sig, data.data(), data.size(), signature.data(),
                                       signature.size(), publicKey.data());
    if (status != OQS_SUCCESS) {
        result.success = true;  // Verification attempted
        result.verified = false;
        result.error_message = "Signature verification failed";
        return result;
    }

    result.success = true;
    result.verified = true;
    result.algorithm_used = m_variant;
    result.algorithm_version = "1.0.0";
#else
    result.success = false;
    result.error_message = "liboqs not available";
#endif

    return result;
}

EncryptionResult SPHINCSAlgorithm::encapsulateKey(const std::vector<uint8_t>& publicKey) {
    EncryptionResult result;
    result.success = false;
    result.error_message = "SPHINCS+ does not support key encapsulation";
    return result;
}

DecryptionResult SPHINCSAlgorithm::decapsulateKey(
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& privateKey
) {
    DecryptionResult result;
    result.success = false;
    result.error_message = "SPHINCS+ does not support key decapsulation";
    return result;
}

EncryptionResult SPHINCSAlgorithm::encrypt(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& publicKey
) {
    EncryptionResult result;
    result.success = false;
    result.error_message = "SPHINCS+ does not support encryption";
    return result;
}

DecryptionResult SPHINCSAlgorithm::decrypt(
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& privateKey
) {
    DecryptionResult result;
    result.success = false;
    result.error_message = "SPHINCS+ does not support decryption";
    return result;
}

bool SPHINCSAlgorithm::isHealthy() const {
#ifdef HAVE_LIBOQS
    return m_initialized && m_sig != nullptr;
#else
    return false;
#endif
}

std::string SPHINCSAlgorithm::getVersion() const {
    return "1.0.0";
}

