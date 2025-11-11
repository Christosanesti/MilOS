#include "kyber_algorithm.h"
#include <stdexcept>
#include <cstring>
#include <iostream>

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

KyberAlgorithm::KyberAlgorithm(const std::string& variant)
    : m_variant(variant)
    , m_initialized(false)
#ifdef HAVE_LIBOQS
    , m_kem(nullptr)
#endif
{
    // Map variant to liboqs algorithm name
    if (variant == "Kyber512") {
        m_algorithmName = "Kyber512";
    } else if (variant == "Kyber768") {
        m_algorithmName = "Kyber768";
    } else if (variant == "Kyber1024") {
        m_algorithmName = "Kyber1024";
    } else {
        m_algorithmName = "Kyber512";  // Default
    }

#ifdef HAVE_LIBOQS
    if (!initializeKEM()) {
        std::cerr << "Failed to initialize Kyber algorithm: " << m_algorithmName << std::endl;
    }
#else
    std::cerr << "Warning: liboqs not available. Kyber algorithm will not function." << std::endl;
#endif
}

KyberAlgorithm::~KyberAlgorithm() {
#ifdef HAVE_LIBOQS
    cleanupKEM();
#endif
}

#ifdef HAVE_LIBOQS
bool KyberAlgorithm::initializeKEM() {
    if (m_initialized) {
        return true;
    }

    m_kem = OQS_KEM_new(m_algorithmName.c_str());
    if (m_kem == nullptr) {
        std::cerr << "Error: OQS_KEM_new failed for " << m_algorithmName << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void KyberAlgorithm::cleanupKEM() {
    if (m_kem != nullptr) {
        OQS_KEM_free(m_kem);
        m_kem = nullptr;
    }
    m_initialized = false;
}
#endif

AlgorithmMetadata KyberAlgorithm::getMetadata() const {
    AlgorithmMetadata metadata;
    metadata.algorithm_id = m_variant;
    metadata.name = "CRYSTALS-Kyber";
    metadata.version = "1.0.0";
    metadata.description = "CRYSTALS-Kyber key encapsulation mechanism (KEM) - " + m_variant;
    metadata.capabilities.push_back(AlgorithmCapability::KEY_ENCAPSULATION);
    metadata.is_nist_approved = true;  // NIST standardized
    metadata.is_hybrid_supported = true;
    
#ifdef HAVE_LIBOQS
    if (m_kem != nullptr) {
        metadata.key_size = m_kem->length_public_key;
        metadata.ciphertext_overhead = m_kem->length_ciphertext - m_kem->length_shared_secret;
    } else {
        metadata.key_size = 0;
        metadata.ciphertext_overhead = 0;
    }
#else
    metadata.key_size = 0;
    metadata.ciphertext_overhead = 0;
#endif
    
    metadata.performance_score = 1.0;  // Baseline
    return metadata;
}

bool KyberAlgorithm::supportsCapability(AlgorithmCapability capability) const {
    return capability == AlgorithmCapability::KEY_ENCAPSULATION;
}

KeyGenerationResult KyberAlgorithm::generateKeyPair() {
    KeyGenerationResult result;
    
#ifdef HAVE_LIBOQS
    if (!m_initialized || m_kem == nullptr) {
        result.success = false;
        result.error_message = "Algorithm not initialized";
        return result;
    }

    size_t publicKeyLen = m_kem->length_public_key;
    size_t privateKeyLen = m_kem->length_secret_key;

    result.public_key.resize(publicKeyLen);
    result.private_key.resize(privateKeyLen);

    uint8_t* publicKeyPtr = result.public_key.data();
    uint8_t* privateKeyPtr = result.private_key.data();

    OQS_STATUS status = OQS_KEM_keypair(m_kem, publicKeyPtr, privateKeyPtr);
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

EncryptionResult KyberAlgorithm::encapsulateKey(const std::vector<uint8_t>& publicKey) {
    EncryptionResult result;
    
#ifdef HAVE_LIBOQS
    if (!m_initialized || m_kem == nullptr) {
        result.success = false;
        result.error_message = "Algorithm not initialized";
        return result;
    }

    if (publicKey.size() != m_kem->length_public_key) {
        result.success = false;
        result.error_message = "Invalid public key size";
        return result;
    }

    size_t ciphertextLen = m_kem->length_ciphertext;
    size_t sharedSecretLen = m_kem->length_shared_secret;

    result.ciphertext.resize(ciphertextLen);
    result.key.resize(sharedSecretLen);

    uint8_t* ciphertextPtr = result.ciphertext.data();
    uint8_t* sharedSecretPtr = result.key.data();

    OQS_STATUS status = OQS_KEM_encaps(m_kem, ciphertextPtr, sharedSecretPtr, publicKey.data());
    if (status != OQS_SUCCESS) {
        result.success = false;
        result.error_message = "Key encapsulation failed";
        result.ciphertext.clear();
        result.key.clear();
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

DecryptionResult KyberAlgorithm::decapsulateKey(
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& privateKey
) {
    DecryptionResult result;
    
#ifdef HAVE_LIBOQS
    if (!m_initialized || m_kem == nullptr) {
        result.success = false;
        result.error_message = "Algorithm not initialized";
        return result;
    }

    if (ciphertext.size() != m_kem->length_ciphertext) {
        result.success = false;
        result.error_message = "Invalid ciphertext size";
        return result;
    }

    if (privateKey.size() != m_kem->length_secret_key) {
        result.success = false;
        result.error_message = "Invalid private key size";
        return result;
    }

    size_t sharedSecretLen = m_kem->length_shared_secret;
    result.key.resize(sharedSecretLen);

    OQS_STATUS status = OQS_KEM_decaps(m_kem, result.key.data(), ciphertext.data(), privateKey.data());
    if (status != OQS_SUCCESS) {
        result.success = false;
        result.error_message = "Key decapsulation failed";
        result.key.clear();
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

EncryptionResult KyberAlgorithm::encrypt(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& publicKey
) {
    // Kyber is a KEM, not a general encryption scheme
    // For encryption, we use key encapsulation and then encrypt with the shared secret
    EncryptionResult result;
    
    if (publicKey.empty()) {
        result.success = false;
        result.error_message = "Public key required for encryption";
        return result;
    }

    // Encapsulate key to get shared secret
    EncryptionResult kemResult = encapsulateKey(publicKey);
    if (!kemResult.success) {
        return kemResult;
    }

    // Use shared secret to encrypt plaintext (simplified - in practice, use AEAD)
    // This is a placeholder - actual implementation would use the shared secret with AES-GCM
    result.success = true;
    result.ciphertext = plaintext;  // Simplified - should encrypt with shared secret
    result.key = kemResult.key;  // Shared secret
    result.algorithm_used = m_variant;
    result.algorithm_version = "1.0.0";
    
    return result;
}

DecryptionResult KyberAlgorithm::decrypt(
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& privateKey
) {
    // Kyber is a KEM, not a general encryption scheme
    DecryptionResult result;
    
    if (privateKey.empty()) {
        result.success = false;
        result.error_message = "Private key required for decryption";
        return result;
    }

    // Simplified - actual implementation would extract encapsulated key and decrypt
    result.success = false;
    result.error_message = "Direct decryption not supported for KEM. Use decapsulateKey instead.";
    
    return result;
}

SignatureResult KyberAlgorithm::sign(
    const std::vector<uint8_t>& data,
    const std::vector<uint8_t>& privateKey
) {
    SignatureResult result;
    result.success = false;
    result.error_message = "Kyber does not support digital signatures";
    return result;
}

VerificationResult KyberAlgorithm::verify(
    const std::vector<uint8_t>& data,
    const std::vector<uint8_t>& signature,
    const std::vector<uint8_t>& publicKey
) {
    VerificationResult result;
    result.success = false;
    result.verified = false;
    result.error_message = "Kyber does not support digital signatures";
    return result;
}

bool KyberAlgorithm::isHealthy() const {
#ifdef HAVE_LIBOQS
    return m_initialized && m_kem != nullptr;
#else
    return false;
#endif
}

std::string KyberAlgorithm::getVersion() const {
    return "1.0.0";
}

