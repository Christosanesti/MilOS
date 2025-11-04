#ifndef BATCHCOMMAND_H
#define BATCHCOMMAND_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QDBusPendingReply>

QT_BEGIN_NAMESPACE
class QDBusConnection;
QT_END_NAMESPACE

class OrgMilosEncryptionManagerInterface;

/**
 * @brief Batch encryption command handler
 * 
 * This class handles the CLI batch encryption command, providing
 * 100% functional equivalence to GUI batch operations (NFR18).
 */
class BatchCommand : public QObject
{
    Q_OBJECT

public:
    explicit BatchCommand(QObject *parent = nullptr);
    ~BatchCommand() = default;

    /**
     * @brief Execute batch encryption command
     * @param filePaths List of file paths to encrypt (1-5 files)
     * @param algorithm Encryption algorithm identifier
     * @param keyId Encryption key ID (UUID)
     * @return Exit code (0 = success, non-zero = failure)
     */
    int execute(const QStringList &filePaths, const QString &algorithm, const QString &keyId);

    /**
     * @brief Check if D-Bus service is available
     * @return True if service is available
     */
    bool isServiceAvailable() const;

signals:
    /**
     * @brief Emitted when batch operation completes (internal use)
     * @param operationId Operation ID
     * @param status Operation status
     */
    void batchOperationCompleted(const QString &operationId, const QString &status);

private slots:
    /**
     * @brief Handle batch encryption started signal
     * @param operationId Operation ID
     */
    void onBatchEncryptionStarted(const QString &operationId);

    /**
     * @brief Handle batch encryption progress signal
     * @param operationId Operation ID
     * @param fileIndex File index (0-based)
     * @param percentage Progress percentage (0-100)
     */
    void onBatchEncryptionProgress(const QString &operationId, int fileIndex, int percentage);

    /**
     * @brief Handle batch encryption completed signal
     * @param operationId Operation ID
     * @param status Operation status
     */
    void onBatchEncryptionCompleted(const QString &operationId, const QString &status);

    /**
     * @brief Handle encryption error signal
     * @param operationId Operation ID
     * @param errorMessage Error message
     */
    void onEncryptionError(const QString &operationId, const QString &errorMessage);

private:
    /**
     * @brief Connect to D-Bus signals
     * @return True if connection successful
     */
    bool connectToDBusSignals();

    /**
     * @brief Format and output progress message
     * @param fileIndex File index (1-based for display)
     * @param totalFiles Total number of files
     * @param filePath File path
     * @param status Status message
     */
    void outputProgress(int fileIndex, int totalFiles, const QString &filePath, const QString &status);

    /**
     * @brief Output final summary
     * @param successCount Number of successful files
     * @param failureCount Number of failed files
     */
    void outputSummary(int successCount, int failureCount);

    OrgMilosEncryptionManagerInterface *m_dbusInterface;
    QString m_operationId;
    QStringList m_filePaths;
    QList<bool> m_fileResults;  // true = success, false = failure
    QList<QString> m_fileErrors;  // Error messages for failed files
    int m_currentFileIndex;
    bool m_operationCompleted;
    int m_exitCode;
};

#endif // BATCHCOMMAND_H

