#ifndef MESSAGE_THREADING_H
#define MESSAGE_THREADING_H

#include "messaging_core.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

/**
 * @brief Message Thread
 */
struct MessageThread {
    QString threadId;
    QString conversationId;
    QString title;
    QList<QString> messageIds;
    QDateTime createdAt;
    QDateTime lastMessageAt;
    int unreadCount;
};

/**
 * @brief Message Threading
 * 
 * Manages message threading and thread organization.
 */
class MessageThreading : public QObject {
    Q_OBJECT

public:
    explicit MessageThreading(QObject* parent = nullptr);
    ~MessageThreading();

    /**
     * @brief Initialize message threading
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create thread
     * @param conversationId Conversation ID
     * @param title Thread title
     * @return Thread ID or empty string if failed
     */
    QString createThread(const QString& conversationId, const QString& title);

    /**
     * @brief Add message to thread
     * @param threadId Thread ID
     * @param messageId Message ID
     * @return true if add successful, false otherwise
     */
    bool addMessageToThread(const QString& threadId, const QString& messageId);

    /**
     * @brief Get thread
     * @param threadId Thread ID
     * @return Thread or empty if not found
     */
    MessageThread getThread(const QString& threadId) const;

    /**
     * @brief Get threads for conversation
     * @param conversationId Conversation ID
     * @return List of threads
     */
    QList<MessageThread> getThreadsForConversation(const QString& conversationId) const;

    /**
     * @brief Get all threads
     * @return List of all threads
     */
    QList<MessageThread> getAllThreads() const;

Q_SIGNALS:
    /**
     * @brief Emitted when thread is created
     */
    void threadCreated(const QString& threadId);

    /**
     * @brief Emitted when message is added to thread
     */
    void messageAddedToThread(const QString& threadId, const QString& messageId);

private:
    QMap<QString, MessageThread> m_threads;
    QString generateThreadId() const;
};

#endif // MESSAGE_THREADING_H

