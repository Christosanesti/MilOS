#ifndef FILE_SHARING_H
#define FILE_SHARING_H

#include "messaging_core.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>

/**
 * @brief File Transfer Status
 */
enum class FileTransferStatus {
    Pending,
    Uploading,
    Downloading,
    Completed,
    Failed,
    Cancelled
};

/**
 * @brief File Transfer Info
 */
struct FileTransferInfo {
    QString transferId;
    QString messageId;
    QString fileName;
    QString filePath;
    qint64 fileSize;
    qint64 bytesTransferred;
    FileTransferStatus status;
    QString mimeType;
    QDateTime startedAt;
    QDateTime completedAt;
};

/**
 * @brief File Sharing
 * 
 * Handles file sharing with size limits and type restrictions.
 */
class FileSharing : public QObject {
    Q_OBJECT

public:
    explicit FileSharing(QObject* parent = nullptr);
    ~FileSharing();

    /**
     * @brief Initialize file sharing
     * @param maxFileSize Maximum file size in bytes
     * @param allowedTypes List of allowed MIME types
     * @return true if initialization successful, false otherwise
     */
    bool initialize(qint64 maxFileSize = 100 * 1024 * 1024, const QStringList& allowedTypes = QStringList());

    /**
     * @brief Send file
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param filePath File path
     * @return Transfer ID or empty string if failed
     */
    QString sendFile(const QString& conversationId, const QString& recipientId, const QString& filePath);

    /**
     * @brief Receive file
     * @param transferId Transfer ID
     * @param destinationPath Destination path
     * @return true if receive successful, false otherwise
     */
    bool receiveFile(const QString& transferId, const QString& destinationPath);

    /**
     * @brief Get transfer info
     * @param transferId Transfer ID
     * @return Transfer info or empty if not found
     */
    FileTransferInfo getTransferInfo(const QString& transferId) const;

    /**
     * @brief Cancel transfer
     * @param transferId Transfer ID
     * @return true if cancel successful, false otherwise
     */
    bool cancelTransfer(const QString& transferId);

    /**
     * @brief Check if file is allowed
     * @param filePath File path
     * @return true if file is allowed, false otherwise
     */
    bool isFileAllowed(const QString& filePath) const;

    /**
     * @brief Set messaging core
     */
    void setMessagingCore(MessagingCore* messagingCore);

    /**
     * @brief Set E2E encryption
     */
    void setE2EEncryption(class E2EEncryption* e2eEncryption);

Q_SIGNALS:
    /**
     * @brief Emitted when file transfer progress is updated
     */
    void transferProgressUpdated(const QString& transferId, qint64 bytesTransferred, qint64 totalBytes);

    /**
     * @brief Emitted when file transfer is completed
     */
    void transferCompleted(const QString& transferId);

    /**
     * @brief Emitted when file transfer fails
     */
    void transferFailed(const QString& transferId, const QString& error);

private:
    MessagingCore* m_messagingCore;
    class E2EEncryption* m_e2eEncryption;
    qint64 m_maxFileSize;
    QStringList m_allowedTypes;
    QMap<QString, FileTransferInfo> m_transfers;
    
    QString generateTransferId() const;
    QString detectMimeType(const QString& filePath) const;
    bool validateFile(const QString& filePath) const;
    QByteArray readFileData(const QString& filePath) const;
};

#endif // FILE_SHARING_H

