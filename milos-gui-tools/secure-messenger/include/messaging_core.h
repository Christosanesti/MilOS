#ifndef MESSAGING_CORE_H
#define MESSAGING_CORE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QVariantMap>

/**
 * @brief Message Status
 */
enum class MessageStatus {
    Pending,
    Sent,
    Delivered,
    Read,
    Failed
};

/**
 * @brief Message Type
 */
enum class MessageType {
    Text,
    File,
    Voice,
    Video
};

/**
 * @brief Message
 */
struct Message {
    QString messageId;
    QString conversationId;
    QString senderId;
    QString recipientId;
    MessageType type;
    QString content;
    QByteArray data;
    MessageStatus status;
    QDateTime timestamp;
    QDateTime deliveredAt;
    QDateTime readAt;
    QVariantMap metadata;
};

/**
 * @brief Messaging Core
 * 
 * Core messaging infrastructure for message routing and queue management.
 */
class MessagingCore : public QObject {
    Q_OBJECT

public:
    explicit MessagingCore(QObject* parent = nullptr);
    ~MessagingCore();

    /**
     * @brief Initialize messaging core
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Send message
     * @param message Message to send
     * @return Message ID or empty string if failed
     */
    QString sendMessage(const Message& message);

    /**
     * @brief Receive message
     * @param messageId Message ID
     * @param data Message data
     * @return true if receive successful, false otherwise
     */
    bool receiveMessage(const QString& messageId, const QByteArray& data);

    /**
     * @brief Get message
     * @param messageId Message ID
     * @return Message or empty if not found
     */
    Message getMessage(const QString& messageId) const;

    /**
     * @brief Update message status
     * @param messageId Message ID
     * @param status New status
     * @return true if update successful, false otherwise
     */
    bool updateMessageStatus(const QString& messageId, MessageStatus status);

    /**
     * @brief Get pending messages
     * @return List of pending messages
     */
    QList<Message> getPendingMessages() const;

Q_SIGNALS:
    /**
     * @brief Emitted when message is sent
     */
    void messageSent(const QString& messageId);

    /**
     * @brief Emitted when message is received
     */
    void messageReceived(const QString& messageId);

    /**
     * @brief Emitted when message status is updated
     */
    void messageStatusUpdated(const QString& messageId, MessageStatus status);

private:
    QMap<QString, Message> m_messages;
    QList<QString> m_messageQueue;
    
    QString generateMessageId() const;
    void queueMessage(const Message& message);
    void processMessageQueue();
};

#endif // MESSAGING_CORE_H

