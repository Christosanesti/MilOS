#include "file_sharing.h"
#include "messaging_core.h"
#include "e2e_encryption.h"
#include <QUuid>
#include <QFileInfo>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>

FileSharing::FileSharing(QObject* parent)
    : QObject(parent)
    , m_messagingCore(nullptr)
    , m_e2eEncryption(nullptr)
    , m_maxFileSize(100 * 1024 * 1024)  // 100 MB default
{
}

FileSharing::~FileSharing() {
}

bool FileSharing::initialize(qint64 maxFileSize, const QStringList& allowedTypes) {
    m_maxFileSize = maxFileSize;
    m_allowedTypes = allowedTypes.isEmpty() ? QStringList() << "*/*" : allowedTypes;
    return true;
}

QString FileSharing::sendFile(const QString& conversationId, const QString& recipientId, const QString& filePath) {
    if (!validateFile(filePath)) {
        return QString();
    }

    if (!m_messagingCore) {
        qWarning() << "MessagingCore not set for FileSharing";
        return QString();
    }

    QFileInfo fileInfo(filePath);
    FileTransferInfo transfer;
    transfer.transferId = generateTransferId();
    transfer.fileName = fileInfo.fileName();
    transfer.filePath = filePath;
    transfer.fileSize = fileInfo.size();
    transfer.bytesTransferred = 0;
    transfer.status = FileTransferStatus::Pending;
    transfer.mimeType = detectMimeType(filePath);
    transfer.startedAt = QDateTime::currentDateTime();

    // Read file data
    QByteArray fileData = readFileData(filePath);
    if (fileData.isEmpty()) {
        qWarning() << "Failed to read file data:" << filePath;
        return QString();
    }

    // Encrypt file data if E2E encryption is available
    QByteArray encryptedData = fileData;
    if (m_e2eEncryption) {
        encryptedData = m_e2eEncryption->encryptFile(fileData, recipientId);
        if (encryptedData.isEmpty()) {
            qWarning() << "Failed to encrypt file data";
            return QString();
        }
    }

    // Create message with file metadata
    Message message;
    message.conversationId = conversationId;
    message.recipientId = recipientId;
    message.type = MessageType::File;
    message.content = fileInfo.fileName();  // Store filename in content
    message.data = encryptedData;  // Store encrypted file data
    message.metadata["file_name"] = fileInfo.fileName();
    message.metadata["file_size"] = fileInfo.size();
    message.metadata["mime_type"] = transfer.mimeType;
    message.metadata["transfer_id"] = transfer.transferId;
    message.status = MessageStatus::Pending;

    // Send message through messaging core
    QString messageId = m_messagingCore->sendMessage(message);
    if (messageId.isEmpty()) {
        qWarning() << "Failed to send file message";
        return QString();
    }

    transfer.messageId = messageId;
    transfer.status = FileTransferStatus::Uploading;
    m_transfers[transfer.transferId] = transfer;

    emit transferProgressUpdated(transfer.transferId, 0, transfer.fileSize);

    return transfer.transferId;
}

bool FileSharing::receiveFile(const QString& transferId, const QString& destinationPath) {
    if (!m_transfers.contains(transferId)) {
        return false;
    }

    FileTransferInfo& transfer = m_transfers[transferId];
    transfer.status = FileTransferStatus::Downloading;
    transfer.filePath = destinationPath;

    // In production, would receive file via mesh network
    // For now, just update status
    transfer.status = FileTransferStatus::Completed;
    transfer.completedAt = QDateTime::currentDateTime();

    emit transferCompleted(transferId);

    return true;
}

FileTransferInfo FileSharing::getTransferInfo(const QString& transferId) const {
    return m_transfers.value(transferId, FileTransferInfo());
}

bool FileSharing::cancelTransfer(const QString& transferId) {
    if (!m_transfers.contains(transferId)) {
        return false;
    }

    FileTransferInfo& transfer = m_transfers[transferId];
    transfer.status = FileTransferStatus::Cancelled;

    return true;
}

bool FileSharing::isFileAllowed(const QString& filePath) const {
    return validateFile(filePath);
}

QString FileSharing::generateTransferId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString FileSharing::detectMimeType(const QString& filePath) const {
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(filePath);
    return mimeType.name();
}

bool FileSharing::validateFile(const QString& filePath) const {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false;
    }

    if (fileInfo.size() > m_maxFileSize) {
        return false;
    }

    QString mimeType = detectMimeType(filePath);
    if (m_allowedTypes.contains("*/*")) {
        return true;
    }

    for (const QString& allowedType : m_allowedTypes) {
        if (mimeType.startsWith(allowedType.split('/')[0] + '/')) {
            return true;
        }
    }

    return false;
}

void FileSharing::setMessagingCore(MessagingCore* messagingCore) {
    m_messagingCore = messagingCore;
}

void FileSharing::setE2EEncryption(E2EEncryption* e2eEncryption) {
    m_e2eEncryption = e2eEncryption;
}

QByteArray FileSharing::readFileData(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return QByteArray();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    return data;
}

