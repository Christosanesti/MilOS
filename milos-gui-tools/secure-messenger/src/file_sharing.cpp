#include "file_sharing.h"
#include "messaging_core.h"
#include <QUuid>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>

FileSharing::FileSharing(QObject* parent)
    : QObject(parent)
    , m_messagingCore(nullptr)
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

    m_transfers[transfer.transferId] = transfer;

    // In production, would create message and start transfer via mesh network
    // For now, just emit signal
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

