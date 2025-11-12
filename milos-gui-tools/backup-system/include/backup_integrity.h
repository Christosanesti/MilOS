#ifndef BACKUP_INTEGRITY_H
#define BACKUP_INTEGRITY_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Backup Integrity Verification Result
 */
struct IntegrityResult {
    bool isValid;
    std::string checksum;
    std::string algorithm;  // "SHA-256", "SHA-512", etc.
    std::string errorMessage;
    uint64_t verificationTimestamp;
};

/**
 * @brief Backup Integrity Verifier
 * 
 * Verifies backup integrity using checksums and signatures.
 */
class BackupIntegrityVerifier {
public:
    BackupIntegrityVerifier();
    ~BackupIntegrityVerifier();

    /**
     * @brief Initialize integrity verifier
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Verify backup integrity
     * @param backupData Backup data to verify
     * @param expectedChecksum Expected checksum
     * @param algorithm Checksum algorithm (SHA-256, SHA-512)
     * @return Integrity verification result
     */
    IntegrityResult verifyIntegrity(const std::vector<uint8_t>& backupData,
                                     const std::string& expectedChecksum,
                                     const std::string& algorithm = "SHA-256");

    /**
     * @brief Calculate backup checksum
     * @param backupData Backup data
     * @param algorithm Checksum algorithm (SHA-256, SHA-512)
     * @return Calculated checksum
     */
    std::string calculateChecksum(const std::vector<uint8_t>& backupData,
                                   const std::string& algorithm = "SHA-256");

    /**
     * @brief Generate integrity report
     * @param backupId Backup ID
     * @param result Integrity verification result
     * @return JSON string with integrity report
     */
    std::string generateIntegrityReport(const std::string& backupId,
                                        const IntegrityResult& result);

private:
    bool m_initialized;

    /**
     * @brief Calculate SHA-256 checksum
     */
    std::string calculateSHA256(const std::vector<uint8_t>& data);

    /**
     * @brief Calculate SHA-512 checksum
     */
    std::string calculateSHA512(const std::vector<uint8_t>& data);
};

#endif // BACKUP_INTEGRITY_H

