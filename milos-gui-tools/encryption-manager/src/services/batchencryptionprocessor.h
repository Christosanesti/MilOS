#ifndef BATCHENCRYPTIONPROCESSOR_H
#define BATCHENCRYPTIONPROCESSOR_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "singlefileencryption.h"

/**
 * @brief File result structure for batch encryption operations
 */
struct FileResult {
    QString filePath;
    QString status;  // "SUCCESS" or "FAILED"
    QString errorMessage;
    
    FileResult() : status("FAILED") {}
    FileResult(const QString &path, const QString &status, const QString &error = QString())
        : filePath(path), status(status), errorMessage(error) {}
};

/**
 * @brief Batch encryption result structure
 */
struct BatchEncryptionResult {
    QString batchOperationId;
    int totalFiles;
    int processedFiles;
    int successfulFiles;
    int failedFiles;
    QList<FileResult> fileResults;
    
    BatchEncryptionResult() : totalFiles(0), processedFiles(0), successfulFiles(0), failedFiles(0) {}
};

/**
 * @brief Batch encryption processor for sequential file encryption
 * 
 * This class processes multiple files sequentially using the existing
 * single-file encryption logic. It handles partial failures and tracks
 * progress per file.
 */
class BatchEncryptionProcessor : public QObject
{
    Q_OBJECT

public:
    explicit BatchEncryptionProcessor(QObject *parent = nullptr);
    ~BatchEncryptionProcessor() = default;

    /**
     * @brief Process batch encryption for multiple files
     * @param filePaths List of file paths to encrypt (up to 5 files)
     * @param algorithm Encryption algorithm identifier
     * @param keyId UUID of encryption key
     * @return BatchEncryptionResult containing results for all files
     */
    BatchEncryptionResult processBatchEncryption(const QStringList &filePaths,
                                                  const QString &algorithm,
                                                  const QString &keyId);

signals:
    /**
     * @brief Emitted when a file encryption starts
     * @param batchOperationId Batch operation identifier
     * @param fileIndex Current file index (0-based)
     * @param filePath Path of file being encrypted
     */
    void fileEncryptionStarted(const QString &batchOperationId, int fileIndex, const QString &filePath);

    /**
     * @brief Emitted during file encryption progress
     * @param batchOperationId Batch operation identifier
     * @param fileIndex Current file index (0-based)
     * @param percentage Progress percentage for current file (0-100)
     */
    void fileEncryptionProgress(const QString &batchOperationId, int fileIndex, int percentage);

    /**
     * @brief Emitted when a file encryption completes
     * @param batchOperationId Batch operation identifier
     * @param fileIndex Current file index (0-based)
     * @param success Whether encryption succeeded
     * @param errorMessage Error message if failed
     */
    void fileEncryptionCompleted(const QString &batchOperationId, int fileIndex, bool success, const QString &errorMessage);

private:
    /**
     * @brief Log encryption operation to audit service
     * @param filePath Path of encrypted file
     * @param algorithm Encryption algorithm used
     * @param keyId Encryption key ID
     * @param status Operation status ("SUCCESS" or "FAILED")
     * @param errorMessage Error message if failed
     */
    void logAuditEvent(const QString &filePath,
                      const QString &algorithm,
                      const QString &keyId,
                      const QString &status,
                      const QString &errorMessage = QString());
};

#endif // BATCHENCRYPTIONPROCESSOR_H

