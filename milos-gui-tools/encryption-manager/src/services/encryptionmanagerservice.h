#ifndef ENCRYPTIONMANAGERSERVICE_H
#define ENCRYPTIONMANAGERSERVICE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusContext>

/**
 * @brief Encryption Manager D-Bus service implementation
 * 
 * This service provides encryption operations through D-Bus interface
 * org.milos.EncryptionManager. It handles both single-file and batch file encryption.
 */
class EncryptionManagerService : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.EncryptionManager")

public:
    explicit EncryptionManagerService(QObject *parent = nullptr);
    ~EncryptionManagerService();

    /**
     * @brief Maximum number of files allowed for batch operations
     */
    static constexpr int MAX_BATCH_FILES = 5;

public Q_SLOTS:
    /**
     * @brief Encrypt a single file
     * @param path File path to encrypt
     * @param algorithm Encryption algorithm identifier
     * @param key_id UUID of encryption key
     * @return Operation ID for tracking the encryption operation
     * 
     * @throws org.milos.EncryptionManager.Error.InvalidPath if file path is invalid
     * @throws org.milos.EncryptionManager.Error.InvalidAlgorithm if algorithm is not supported
     * @throws org.milos.EncryptionManager.Error.InvalidKey if key_id is not found
     */
    QString EncryptFile(const QString &path, const QString &algorithm, const QString &key_id);

    /**
     * @brief Encrypt multiple files (batch operation)
     * @param paths Array of file paths (up to 5 files)
     * @param algorithm Encryption algorithm identifier
     * @param key_id UUID of encryption key
     * @return Operation ID for tracking the batch encryption operation
     * 
     * @throws org.milos.EncryptionManager.Error.InvalidPath if any file path is invalid
     * @throws org.milos.EncryptionManager.Error.InvalidAlgorithm if algorithm is not supported
     * @throws org.milos.EncryptionManager.Error.InvalidKey if key_id is not found
     * @throws org.milos.EncryptionManager.Error.TooManyFiles if more than 5 files provided
     */
    QString EncryptFiles(const QStringList &paths, const QString &algorithm, const QString &key_id);

Q_SIGNALS:
    /**
     * @brief Emitted when encryption operation starts
     * @param operation_id Operation identifier
     */
    void EncryptionStarted(const QString &operation_id);

    /**
     * @brief Emitted during encryption operation progress
     * @param operation_id Operation identifier
     * @param percentage Progress percentage (0-100)
     */
    void EncryptionProgress(const QString &operation_id, int percentage);

    /**
     * @brief Emitted when encryption operation completes
     * @param operation_id Operation identifier
     * @param status Operation status (SUCCESS, FAILED, CANCELLED)
     */
    void EncryptionCompleted(const QString &operation_id, const QString &status);

    /**
     * @brief Emitted when encryption operation encounters an error
     * @param operation_id Operation identifier
     * @param error Error message
     */
    void EncryptionError(const QString &operation_id, const QString &error);

    /**
     * @brief Emitted when batch encryption operation starts
     * @param operation_id Operation identifier for batch operation
     */
    void BatchEncryptionStarted(const QString &operation_id);

    /**
     * @brief Emitted during batch encryption operation progress
     * @param operation_id Operation identifier for batch operation
     * @param file_index Index of current file being processed (0-based)
     * @param percentage Progress percentage for current file (0-100)
     */
    void BatchEncryptionProgress(const QString &operation_id, int file_index, int percentage);

    /**
     * @brief Emitted when batch encryption operation completes
     * @param operation_id Operation identifier for batch operation
     * @param status Operation status (SUCCESS, FAILED, CANCELLED)
     */
    void BatchEncryptionCompleted(const QString &operation_id, const QString &status);

private:
    /**
     * @brief Generate a UUID for operation tracking
     * @return UUID string
     */
    QString generateOperationId();

    /**
     * @brief Validate file path
     * @param path File path to validate
     * @return True if file exists and is readable
     */
    bool validateFilePath(const QString &path);

    /**
     * @brief Validate algorithm identifier
     * @param algorithm Algorithm identifier
     * @return True if algorithm is supported
     */
    bool validateAlgorithm(const QString &algorithm);

    /**
     * @brief Validate key identifier
     * @param key_id Key UUID
     * @return True if key exists
     */
    bool validateKeyId(const QString &key_id);

    /**
     * @brief Send D-Bus error reply
     * @param errorName Error name (e.g., "org.milos.EncryptionManager.Error.InvalidPath")
     * @param errorMessage Error message
     */
    void sendErrorReply(const QString &errorName, const QString &errorMessage);

public:
    /**
     * @brief Register this service on D-Bus
     * @param serviceName Service name (e.g., "org.milos.EncryptionManager")
     * @param objectPath Object path (e.g., "/org/milos/EncryptionManager")
     * @return True if registration successful
     */
    bool registerService(const QString &serviceName = "org.milos.EncryptionManager",
                        const QString &objectPath = "/org/milos/EncryptionManager");
};

#endif // ENCRYPTIONMANAGERSERVICE_H

