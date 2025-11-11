#include "milos/encryption/disk_encryption.h"
#include "milos/encryption/key_derivation.h"
#include <iostream>
#include <fstream>
#include <cstring>

#ifdef HAVE_CRYPTSETUP
#include <libcryptsetup.h>
#endif

DiskEncryption::DiskEncryption()
    : m_initialized(false)
    , m_pqcAlgorithmId("Kyber512")
{
}

DiskEncryption::~DiskEncryption() {
}

bool DiskEncryption::initialize(const std::string& pqcAlgorithmId) {
    if (!pqcAlgorithmId.empty()) {
        m_pqcAlgorithmId = pqcAlgorithmId;
    }
    m_initialized = true;
    return true;
}

DiskEncryptionResult DiskEncryption::formatDevice(
    const std::string& devicePath,
    const std::string& password,
    size_t keySize
) {
    DiskEncryptionResult result;
    result.success = false;
    result.device_path = devicePath;

#ifdef HAVE_CRYPTSETUP
    struct crypt_device* cd = nullptr;
    int r;

    // Initialize crypt device
    r = crypt_init(&cd, devicePath.c_str());
    if (r < 0) {
        result.error_message = "Failed to initialize crypt device";
        return result;
    }

    // Set LUKS2 parameters
    struct crypt_params_luks2 params = {
        .hash = "sha256",  // Hash algorithm
        .data_alignment = 0,  // Auto-align
        .data_device = nullptr
    };

    // Format device with LUKS2
    r = crypt_format(cd, CRYPT_LUKS2, "aes", "xts-plain64",
                     nullptr, nullptr, keySize / 8, &params);
    if (r < 0) {
        result.error_message = "Failed to format device with LUKS2";
        crypt_free(cd);
        return result;
    }

    // Derive key from password using PQC
    KeyDerivation keyDerivation;
    keyDerivation.initialize(KeyDerivationAlgorithm::PBKDF2_PQC, m_pqcAlgorithmId);
    
    // Get salt from LUKS2 header (simplified - in production, use actual salt from header)
    std::vector<uint8_t> salt = KeyDerivation::generateSalt(32);
    
    // Derive key
    KeyDerivationResult keyResult = keyDerivation.deriveFromPassword(
        password, salt, keySize / 8, 100000
    );
    
    if (!keyResult.success) {
        result.error_message = "Key derivation failed: " + keyResult.error_message;
        crypt_free(cd);
        return result;
    }

    // Add key slot
    r = crypt_keyslot_add_by_volume_key(cd, CRYPT_ANY_SLOT,
                                         keyResult.derived_key.data(),
                                         keyResult.derived_key.size(),
                                         password.c_str(), password.length());
    if (r < 0) {
        result.error_message = "Failed to add key slot";
        crypt_free(cd);
        return result;
    }

    crypt_free(cd);
    result.success = true;
    result.algorithm_used = "LUKS2-PQC";
#else
    result.error_message = "cryptsetup not available. LUKS2 support not compiled.";
#endif

    return result;
}

bool DiskEncryption::openDevice(
    const std::string& devicePath,
    const std::string& password,
    const std::string& mapperName
) {
#ifdef HAVE_CRYPTSETUP
    struct crypt_device* cd = nullptr;
    int r;

    r = crypt_init(&cd, devicePath.c_str());
    if (r < 0) {
        return false;
    }

    r = crypt_load(cd, CRYPT_LUKS2, nullptr);
    if (r < 0) {
        crypt_free(cd);
        return false;
    }

    r = crypt_activate_by_passphrase(cd, mapperName.c_str(),
                                      CRYPT_ANY_SLOT,
                                      password.c_str(), password.length(),
                                      CRYPT_ACTIVATE_READONLY);
    if (r < 0) {
        crypt_free(cd);
        return false;
    }

    crypt_free(cd);
    return true;
#else
    return false;
#endif
}

bool DiskEncryption::closeDevice(const std::string& mapperName) {
#ifdef HAVE_CRYPTSETUP
    struct crypt_device* cd = nullptr;
    int r;

    r = crypt_init_by_name(&cd, mapperName.c_str());
    if (r < 0) {
        return false;
    }

    r = crypt_deactivate(cd, mapperName.c_str());
    crypt_free(cd);
    return r == 0;
#else
    return false;
#endif
}

bool DiskEncryption::isDeviceEncrypted(const std::string& devicePath) const {
#ifdef HAVE_CRYPTSETUP
    struct crypt_device* cd = nullptr;
    int r;

    r = crypt_init(&cd, devicePath.c_str());
    if (r < 0) {
        return false;
    }

    r = crypt_load(cd, CRYPT_LUKS2, nullptr);
    crypt_free(cd);
    return r == 0;
#else
    return false;
#endif
}

std::vector<uint8_t> DiskEncryption::deriveKeyFromPassword(
    const std::string& password,
    const std::vector<uint8_t>& salt,
    size_t keySize
) {
    KeyDerivation keyDerivation;
    keyDerivation.initialize(KeyDerivationAlgorithm::PBKDF2_PQC, m_pqcAlgorithmId);
    
    KeyDerivationResult result = keyDerivation.deriveFromPassword(
        password, salt, keySize, 100000
    );
    
    if (result.success) {
        return result.derived_key;
    }
    
    return {};
}

