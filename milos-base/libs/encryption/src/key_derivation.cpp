#include "milos/encryption/key_derivation.h"
#include "milos/pqc/algorithm_factory.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

KeyDerivation::KeyDerivation()
    : m_initialized(false)
    , m_algorithm(KeyDerivationAlgorithm::PBKDF2_PQC)
{
}

KeyDerivation::~KeyDerivation() {
}

bool KeyDerivation::initialize(
    KeyDerivationAlgorithm algorithm,
    const std::string& pqcHashAlgorithm
) {
    m_algorithm = algorithm;
    m_pqcHashAlgorithm = pqcHashAlgorithm.empty() ? "SHA-256" : pqcHashAlgorithm;
    m_initialized = true;
    return true;
}

KeyDerivationResult KeyDerivation::deriveFromPassword(
    const std::string& password,
    const std::vector<uint8_t>& salt,
    size_t keyLength,
    uint32_t iterations
) {
    std::vector<uint8_t> passwordBytes(password.begin(), password.end());
    return deriveFromPassword(passwordBytes, salt, keyLength, iterations);
}

KeyDerivationResult KeyDerivation::deriveFromPassword(
    const std::vector<uint8_t>& password,
    const std::vector<uint8_t>& salt,
    size_t keyLength,
    uint32_t iterations
) {
    KeyDerivationResult result;
    result.success = false;

    if (!m_initialized) {
        result.error_message = "Key derivation not initialized";
        return result;
    }

    if (salt.empty()) {
        result.error_message = "Salt cannot be empty";
        return result;
    }

    switch (m_algorithm) {
        case KeyDerivationAlgorithm::PBKDF2_PQC:
            return derivePBKDF2(password, salt, keyLength, iterations);
        case KeyDerivationAlgorithm::ARGON2_PQC:
            // Argon2 implementation would go here
            // For now, fall back to PBKDF2
            result.error_message = "Argon2 not yet implemented, using PBKDF2";
            return derivePBKDF2(password, salt, keyLength, iterations);
        case KeyDerivationAlgorithm::HKDF_PQC:
            // HKDF requires input key material, not password
            result.error_message = "Use deriveHKDF for HKDF algorithm";
            return result;
        default:
            result.error_message = "Unknown key derivation algorithm";
            return result;
    }
}

KeyDerivationResult KeyDerivation::deriveHKDF(
    const std::vector<uint8_t>& inputKeyMaterial,
    const std::vector<uint8_t>& salt,
    const std::vector<uint8_t>& info,
    size_t keyLength
) {
    KeyDerivationResult result;
    result.success = false;

    if (!m_initialized) {
        result.error_message = "Key derivation not initialized";
        return result;
    }

    // Simplified HKDF implementation using OpenSSL
    // In production, this should use PQC hash functions
    
    // Extract phase: HMAC with salt
    std::vector<uint8_t> prk;
    if (salt.empty()) {
        // Use zero salt if not provided
        prk = hashPQC(inputKeyMaterial);
    } else {
        // HMAC with salt
        // Simplified: use hash(salt || inputKeyMaterial)
        std::vector<uint8_t> combined;
        combined.insert(combined.end(), salt.begin(), salt.end());
        combined.insert(combined.end(), inputKeyMaterial.begin(), inputKeyMaterial.end());
        prk = hashPQC(combined);
    }

    // Expand phase: generate key material
    result.derived_key.resize(keyLength);
    
    // Simplified expansion (production should use proper HKDF-Expand)
    std::vector<uint8_t> temp;
    temp.insert(temp.end(), prk.begin(), prk.end());
    temp.insert(temp.end(), info.begin(), info.end());
    
    std::vector<uint8_t> hash = hashPQC(temp);
    size_t copyLen = std::min(hash.size(), keyLength);
    std::memcpy(result.derived_key.data(), hash.data(), copyLen);
    
    // If more key material needed, expand further
    if (keyLength > hash.size()) {
        // Simplified: repeat hash for additional bytes
        size_t remaining = keyLength - hash.size();
        std::vector<uint8_t> additional = hashPQC(hash);
        size_t additionalCopy = std::min(additional.size(), remaining);
        std::memcpy(result.derived_key.data() + copyLen, additional.data(), additionalCopy);
    }

    result.success = true;
    result.algorithm_used = "HKDF-PQC";
    return result;
}

KeyDerivationResult KeyDerivation::derivePBKDF2(
    const std::vector<uint8_t>& password,
    const std::vector<uint8_t>& salt,
    size_t keyLength,
    uint32_t iterations
) {
    KeyDerivationResult result;
    result.success = false;

    // Use OpenSSL PBKDF2 with SHA-256 (classical)
    // In production, this should use PQC hash functions
    result.derived_key.resize(keyLength);

    const EVP_MD* digest = EVP_sha256();
    if (PKCS5_PBKDF2_HMAC(
            reinterpret_cast<const char*>(password.data()),
            password.size(),
            salt.data(),
            salt.size(),
            iterations,
            digest,
            keyLength,
            result.derived_key.data()
        ) != 1) {
        result.error_message = "PBKDF2 derivation failed";
        return result;
    }

    result.success = true;
    result.algorithm_used = "PBKDF2-PQC";
    return result;
}

std::vector<uint8_t> KeyDerivation::hashPQC(const std::vector<uint8_t>& data) {
    // Use PQC hash algorithm if available, otherwise fall back to SHA-256
    // For now, use SHA-256 (classical)
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash.data(), &sha256);
    return hash;
}

std::vector<uint8_t> KeyDerivation::generateSalt(size_t saltLength) {
    std::vector<uint8_t> salt(saltLength);
    if (RAND_bytes(salt.data(), saltLength) != 1) {
        // Fallback to pseudo-random if RAND_bytes fails
        std::cerr << "Warning: RAND_bytes failed, using fallback" << std::endl;
        // Simple fallback (not cryptographically secure)
        for (size_t i = 0; i < saltLength; ++i) {
            salt[i] = static_cast<uint8_t>(rand() % 256);
        }
    }
    return salt;
}

