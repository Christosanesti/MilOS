#include "messaging_core.h"
#include "message_storage.h"
#include "mesh_network.h"
#include <QUuid>
#include <QDebug>
#include <QTimer>

MessagingCore::MessagingCore(QObject* parent)
    : QObject(parent)
    , m_messageStorage(nullptr)
    , m_meshNetwork(nullptr)
{
}

MessagingCore::~MessagingCore() {
}

bool MessagingCore::initialize() {
    // Start message queue processing timer
    QTimer* queueTimer = new QTimer(this);
    connect(queueTimer, &QTimer::timeout, this, &MessagingCore::processMessageQueue);
    queueTimer->start(1000);  // Process queue every second
    
    return true;
}

QString MessagingCore::sendMessage(const Message& message) {
    Message msg = message;
    if (msg.messageId.isEmpty()) {
        msg.messageId = generateMessageId();
    }
    if (!msg.timestamp.isValid()) {
        msg.timestamp = QDateTime::currentDateTime();
    }
    msg.status = MessageStatus::Pending;
    
    m_messages[msg.messageId] = msg;
    
    // Automatically store message to persistent storage
    if (m_messageStorage) {
        m_messageStorage->storeMessage(msg);
    }
    
    queueMessage(msg);
    
    emit messageSent(msg.messageId);
    
    return msg.messageId;
}

bool MessagingCore::receiveMessage(const QString& messageId, const QByteArray& data) {
    // In production, would parse message data and create Message object
    // For now, create placeholder message
    Message msg;
    msg.messageId = messageId;
    msg.data = data;
    msg.status = MessageStatus::Delivered;
    msg.timestamp = QDateTime::currentDateTime();
    msg.deliveredAt = QDateTime::currentDateTime();
    
    m_messages[messageId] = msg;
    
    // Automatically store received message to persistent storage
    if (m_messageStorage) {
        m_messageStorage->storeMessage(msg);
    }
    
    emit messageReceived(messageId);
    
    return true;
}

Message MessagingCore::getMessage(const QString& messageId) const {
    return m_messages.value(messageId, Message());
}

bool MessagingCore::updateMessageStatus(const QString& messageId, MessageStatus status) {
    if (!m_messages.contains(messageId)) {
        return false;
    }
    
    Message& msg = m_messages[messageId];
    msg.status = status;
    
    QDateTime timestamp = QDateTime::currentDateTime();
    if (status == MessageStatus::Delivered && !msg.deliveredAt.isValid()) {
        msg.deliveredAt = timestamp;
    } else if (status == MessageStatus::Read && !msg.readAt.isValid()) {
        msg.readAt = timestamp;
    }
    
    // Update status in persistent storage
    if (m_messageStorage) {
        m_messageStorage->updateMessageStatus(messageId, status, timestamp);
    }
    
    emit messageStatusUpdated(messageId, status);
    
    return true;
}

QList<Message> MessagingCore::getPendingMessages() const {
    QList<Message> pending;
    for (const Message& msg : m_messages.values()) {
        if (msg.status == MessageStatus::Pending) {
            pending.append(msg);
        }
    }
    return pending;
}

QString MessagingCore::generateMessageId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void MessagingCore::queueMessage(const Message& message) {
    if (!m_messageQueue.contains(message.messageId)) {
        m_messageQueue.append(message.messageId);
    }
}

void MessagingCore::processMessageQueue() {
    // Process pending messages in queue
    QList<QString> processedMessages;
    
    for (const QString& messageId : m_messageQueue) {
        if (m_messages.contains(messageId)) {
            Message& msg = m_messages[messageId];
            if (msg.status == MessageStatus::Pending) {
                // Attempt to send message
                if (attemptSendMessage(msg)) {
                    msg.status = MessageStatus::Sent;
                    m_retryCounts.remove(messageId);  // Reset retry count on success
                    emit messageStatusUpdated(messageId, MessageStatus::Sent);
                    processedMessages.append(messageId);
                } else {
                    // Increment retry count
                    int retryCount = m_retryCounts.value(messageId, 0) + 1;
                    m_retryCounts[messageId] = retryCount;
                    
                    if (retryCount >= MAX_RETRY_ATTEMPTS) {
                        // Mark as failed after max retries
                        msg.status = MessageStatus::Failed;
                        emit messageStatusUpdated(messageId, MessageStatus::Failed);
                        processedMessages.append(messageId);
                    }
                    // Otherwise, keep in queue for next retry
                }
            } else if (msg.status == MessageStatus::Sent || msg.status == MessageStatus::Delivered) {
                // Already successfully processed, remove from queue
                processedMessages.append(messageId);
            } else if (msg.status == MessageStatus::Failed) {
                // Failed messages stay in queue until manually retried via retryMessage()
                // Don't remove from queue automatically
            }
        } else {
            // Message not found, remove from queue
            processedMessages.append(messageId);
        }
    }
    
    // Remove processed messages from queue
    for (const QString& messageId : processedMessages) {
        m_messageQueue.removeAll(messageId);
    }
}

bool MessagingCore::attemptSendMessage(const Message& message) {
    if (m_meshNetwork && !message.recipientId.isEmpty()) {
        // Use mesh network to send message
        bool success = m_meshNetwork->sendMessage(message.recipientId, message.data);
        return success;
    }
    
    // If no mesh network, simulate success for testing
    // In production, this should always use mesh network
    return true;
}

QList<Message> MessagingCore::getFailedMessages() const {
    QList<Message> failed;
    for (const Message& msg : m_messages.values()) {
        if (msg.status == MessageStatus::Failed) {
            failed.append(msg);
        }
    }
    return failed;
}

bool MessagingCore::retryMessage(const QString& messageId) {
    if (!m_messages.contains(messageId)) {
        return false;
    }
    
    Message& msg = m_messages[messageId];
    if (msg.status != MessageStatus::Failed) {
        return false;  // Can only retry failed messages
    }
    
    // Reset retry count and status
    m_retryCounts.remove(messageId);
    msg.status = MessageStatus::Pending;
    
    // Re-queue message
    queueMessage(msg);
    
    // Update status in storage
    if (m_messageStorage) {
        m_messageStorage->updateMessageStatus(messageId, MessageStatus::Pending);
    }
    
    return true;
}

void MessagingCore::setMessageStorage(MessageStorage* messageStorage) {
    m_messageStorage = messageStorage;
}

void MessagingCore::setMeshNetwork(MeshNetwork* meshNetwork) {
    m_meshNetwork = meshNetwork;
}

