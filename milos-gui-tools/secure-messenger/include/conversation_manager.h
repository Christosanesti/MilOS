#ifndef CONVERSATION_MANAGER_H
#define CONVERSATION_MANAGER_H

#include "messaging_core.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSet>

/**
 * @brief Conversation Type
 */
enum class ConversationType {
    Direct,
    Group
};

/**
 * @brief Conversation
 */
struct Conversation {
    QString conversationId;
    ConversationType type;
    QSet<QString> participants;
    QString title;
    QDateTime createdAt;
    QDateTime lastMessageAt;
    QString lastMessageId;
    int unreadCount;
    QVariantMap metadata;
};

/**
 * @brief Conversation Manager
 * 
 * Manages conversations, grouping, and search.
 */
class ConversationManager : public QObject {
    Q_OBJECT

public:
    explicit ConversationManager(QObject* parent = nullptr);
    ~ConversationManager();

    /**
     * @brief Initialize conversation manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create conversation
     * @param type Conversation type
     * @param participants List of participant IDs
     * @param title Conversation title
     * @return Conversation ID or empty string if failed
     */
    QString createConversation(ConversationType type,
                              const QList<QString>& participants,
                              const QString& title = QString());

    /**
     * @brief Get conversation
     * @param conversationId Conversation ID
     * @return Conversation or empty if not found
     */
    Conversation getConversation(const QString& conversationId) const;

    /**
     * @brief Get conversations for user
     * @param userId User ID
     * @return List of conversations
     */
    QList<Conversation> getConversationsForUser(const QString& userId) const;

    /**
     * @brief Get all conversations
     * @return List of all conversations
     */
    QList<Conversation> getAllConversations() const;

    /**
     * @brief Search conversations
     * @param query Search query
     * @param userId Optional user ID filter
     * @return List of matching conversations
     */
    QList<Conversation> searchConversations(const QString& query, const QString& userId = QString()) const;

    /**
     * @brief Update conversation last message
     * @param conversationId Conversation ID
     * @param messageId Message ID
     * @return true if update successful, false otherwise
     */
    bool updateLastMessage(const QString& conversationId, const QString& messageId);

Q_SIGNALS:
    /**
     * @brief Emitted when conversation is created
     */
    void conversationCreated(const QString& conversationId);

    /**
     * @brief Emitted when conversation is updated
     */
    void conversationUpdated(const QString& conversationId);

private:
    QMap<QString, Conversation> m_conversations;
    QString generateConversationId() const;
};

#endif // CONVERSATION_MANAGER_H

