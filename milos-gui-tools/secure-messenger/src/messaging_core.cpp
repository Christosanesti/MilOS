#include "messaging_core.h"
#include <QUuid>
#include <QDebug>
#include <QTimer>

MessagingCore::MessagingCore(QObject* parent)
    : QObject(parent)
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
    
    if (status == MessageStatus::Delivered && !msg.deliveredAt.isValid()) {
        msg.deliveredAt = QDateTime::currentDateTime();
    } else if (status == MessageStatus::Read && !msg.readAt.isValid()) {
        msg.readAt = QDateTime::currentDateTime();
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
    // In production, would send via mesh network
    for (const QString& messageId : m_messageQueue) {
        if (m_messages.contains(messageId)) {
            Message& msg = m_messages[messageId];
            if (msg.status == MessageStatus::Pending) {
                // Attempt to send message
                // In production, would use mesh network to send
                msg.status = MessageStatus::Sent;
                emit messageStatusUpdated(messageId, MessageStatus::Sent);
            }
        }
    }
    
    // Remove processed messages from queue
    m_messageQueue.clear();
}

