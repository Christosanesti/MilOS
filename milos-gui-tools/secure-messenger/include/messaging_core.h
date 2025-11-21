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

    /**
     * @brief Get failed messages
     * @return List of failed messages
     */
    QList<Message> getFailedMessages() const;

    /**
     * @brief Retry failed message
     * @param messageId Message ID
     * @return true if retry successful, false otherwise
     */
    bool retryMessage(const QString& messageId);

    /**
     * @brief Set message storage (for automatic persistence)
     */
    void setMessageStorage(class MessageStorage* messageStorage);

    /**
     * @brief Set mesh network (for message sending)
     */
    void setMeshNetwork(class MeshNetwork* meshNetwork);

    /**
     * @brief Set E2E encryption (for message decryption)
     */
    void setE2EEncryption(class E2EEncryption* e2eEncryption);

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
    QMap<QString, int> m_retryCounts;  // Message ID -> retry count
    class MessageStorage* m_messageStorage;
    class MeshNetwork* m_meshNetwork;
    class E2EEncryption* m_e2eEncryption;
    static const int MAX_RETRY_ATTEMPTS = 3;
    
    QString generateMessageId() const;
    void queueMessage(const Message& message);
    void processMessageQueue();
    bool attemptSendMessage(const Message& message);
};

#endif // MESSAGING_CORE_H

