#ifndef SINGLEFILEENCRYPTION_H
#define SINGLEFILEENCRYPTION_H

#include <QtCore/QString>

/**
 * @brief Result structure for single file encryption operation
 */
struct EncryptionResult {
    bool success;
    QString errorMessage;
    QString operationId;
    
    EncryptionResult() : success(false) {}
    EncryptionResult(bool success, const QString &error = QString()) 
        : success(success), errorMessage(error) {}
};

/**
 * @brief Encrypt a single file using existing encryption logic
 * 
 * This function provides the core encryption logic that can be reused
 * by both single-file and batch encryption operations.
 * 
 * @param filePath Path to the file to encrypt
 * @param algorithm Encryption algorithm identifier (e.g., "CRYSTALS-Kyber")
 * @param keyId UUID of the encryption key
 * @return EncryptionResult containing success status and operation details
 */
EncryptionResult encryptFile(const QString &filePath, 
                             const QString &algorithm, 
                             const QString &keyId);

#endif // SINGLEFILEENCRYPTION_H

