#include "backup_integrity.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <iostream>

BackupIntegrityVerifier::BackupIntegrityVerifier()
    : m_initialized(false)
{
}

BackupIntegrityVerifier::~BackupIntegrityVerifier() {
}

bool BackupIntegrityVerifier::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

std::string BackupIntegrityVerifier::calculateSHA256(const std::vector<uint8_t>& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string BackupIntegrityVerifier::calculateSHA512(const std::vector<uint8_t>& data) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, data.data(), data.size());
    SHA512_Final(hash, &sha512);

    std::stringstream ss;
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string BackupIntegrityVerifier::calculateChecksum(const std::vector<uint8_t>& backupData,
                                                        const std::string& algorithm) {
    if (!m_initialized) {
        return "";
    }

    if (algorithm == "SHA-256") {
        return calculateSHA256(backupData);
    } else if (algorithm == "SHA-512") {
        return calculateSHA512(backupData);
    }

    return "";
}

IntegrityResult BackupIntegrityVerifier::verifyIntegrity(const std::vector<uint8_t>& backupData,
                                                          const std::string& expectedChecksum,
                                                          const std::string& algorithm) {
    IntegrityResult result;
    result.isValid = false;
    result.algorithm = algorithm;
    result.verificationTimestamp = 0;  // Would use actual timestamp

    if (!m_initialized) {
        result.errorMessage = "Integrity verifier not initialized";
        return result;
    }

    std::string calculatedChecksum = calculateChecksum(backupData, algorithm);
    if (calculatedChecksum.empty()) {
        result.errorMessage = "Failed to calculate checksum";
        return result;
    }

    result.checksum = calculatedChecksum;
    result.isValid = (calculatedChecksum == expectedChecksum);

    if (!result.isValid) {
        result.errorMessage = "Checksum mismatch";
    }

    return result;
}

std::string BackupIntegrityVerifier::generateIntegrityReport(const std::string& backupId,
                                                              const IntegrityResult& result) {
    std::stringstream ss;
    ss << "{"
       << "\"backup_id\":\"" << backupId << "\","
       << "\"is_valid\":" << (result.isValid ? "true" : "false") << ","
       << "\"checksum\":\"" << result.checksum << "\","
       << "\"algorithm\":\"" << result.algorithm << "\","
       << "\"error_message\":\"" << result.errorMessage << "\","
       << "\"verification_timestamp\":" << result.verificationTimestamp
       << "}";
    return ss.str();
}

