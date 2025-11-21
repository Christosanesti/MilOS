#ifndef GROUP_MESSAGING_H
#define GROUP_MESSAGING_H

#include "conversation_manager.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSet>

/**
 * @brief Group Messaging
 * 
 * Handles group messaging with configurable participant limits.
 */
class GroupMessaging : public QObject {
    Q_OBJECT

public:
    explicit GroupMessaging(QObject* parent = nullptr);
    ~GroupMessaging();

    /**
     * @brief Initialize group messaging
     * @param maxParticipants Maximum number of participants
     * @return true if initialization successful, false otherwise
     */
    bool initialize(int maxParticipants = 50);

    /**
     * @brief Create group
     * @param name Group name
     * @param creatorId Creator user ID
     * @param initialParticipants List of initial participant IDs
     * @return Conversation ID or empty string if failed
     */
    QString createGroup(const QString& name, const QString& creatorId, const QStringList& initialParticipants);

    /**
     * @brief Add participant to group
     * @param conversationId Conversation ID
     * @param participantId Participant ID
     * @return true if add successful, false otherwise
     */
    bool addParticipant(const QString& conversationId, const QString& participantId);

    /**
     * @brief Remove participant from group
     * @param conversationId Conversation ID
     * @param participantId Participant ID
     * @return true if remove successful, false otherwise
     */
    bool removeParticipant(const QString& conversationId, const QString& participantId);

    /**
     * @brief Get group participants
     * @param conversationId Conversation ID
     * @return List of participant IDs
     */
    QStringList getParticipants(const QString& conversationId) const;

    /**
     * @brief Check if participant limit is reached
     * @param conversationId Conversation ID
     * @return true if limit reached, false otherwise
     */
    bool isParticipantLimitReached(const QString& conversationId) const;

    /**
     * @brief Set conversation manager
     */
    void setConversationManager(ConversationManager* conversationManager);

Q_SIGNALS:
    /**
     * @brief Emitted when participant is added
     */
    void participantAdded(const QString& conversationId, const QString& participantId);

    /**
     * @brief Emitted when participant is removed
     */
    void participantRemoved(const QString& conversationId, const QString& participantId);

private:
    ConversationManager* m_conversationManager;
    int m_maxParticipants;
};

#endif // GROUP_MESSAGING_H

