#include "conversation_manager.h"
#include <QUuid>
#include <QDebug>

ConversationManager::ConversationManager(QObject* parent)
    : QObject(parent)
{
}

ConversationManager::~ConversationManager() {
}

bool ConversationManager::initialize() {
    return true;
}

QString ConversationManager::createConversation(ConversationType type,
                                                const QList<QString>& participants,
                                                const QString& title) {
    Conversation conv;
    conv.conversationId = generateConversationId();
    conv.type = type;
    conv.participants = QSet<QString>(participants.begin(), participants.end());
    conv.title = title.isEmpty() ? "Conversation" : title;
    conv.createdAt = QDateTime::currentDateTime();
    conv.lastMessageAt = QDateTime::currentDateTime();
    conv.unreadCount = 0;
    
    m_conversations[conv.conversationId] = conv;
    
    emit conversationCreated(conv.conversationId);
    
    return conv.conversationId;
}

Conversation ConversationManager::getConversation(const QString& conversationId) const {
    return m_conversations.value(conversationId, Conversation());
}

QList<Conversation> ConversationManager::getConversationsForUser(const QString& userId) const {
    QList<Conversation> conversations;
    for (const Conversation& conv : m_conversations.values()) {
        if (userId.isEmpty() || conv.participants.contains(userId)) {
            conversations.append(conv);
        }
    }
    return conversations;
}

QList<Conversation> ConversationManager::getAllConversations() const {
    return m_conversations.values();
}

QList<Conversation> ConversationManager::searchConversations(const QString& query, const QString& userId) const {
    QList<Conversation> results;
    
    for (const Conversation& conv : m_conversations.values()) {
        if (!userId.isEmpty() && !conv.participants.contains(userId)) {
            continue;
        }
        
        if (conv.title.contains(query, Qt::CaseInsensitive)) {
            results.append(conv);
        }
    }
    
    return results;
}

bool ConversationManager::updateLastMessage(const QString& conversationId, const QString& messageId) {
    if (!m_conversations.contains(conversationId)) {
        return false;
    }
    
    Conversation& conv = m_conversations[conversationId];
    conv.lastMessageId = messageId;
    conv.lastMessageAt = QDateTime::currentDateTime();
    conv.unreadCount++;
    
    emit conversationUpdated(conversationId);
    
    return true;
}

QString ConversationManager::generateConversationId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool ConversationManager::addParticipant(const QString& conversationId, const QString& participantId) {
    if (!m_conversations.contains(conversationId)) {
        return false;
    }
    
    Conversation& conv = m_conversations[conversationId];
    if (conv.participants.contains(participantId)) {
        return true;  // Already a participant
    }
    
    conv.participants.insert(participantId);
    emit conversationUpdated(conversationId);
    
    return true;
}

bool ConversationManager::removeParticipant(const QString& conversationId, const QString& participantId) {
    if (!m_conversations.contains(conversationId)) {
        return false;
    }
    
    Conversation& conv = m_conversations[conversationId];
    if (!conv.participants.contains(participantId)) {
        return false;  // Not a participant
    }
    
    conv.participants.remove(participantId);
    emit conversationUpdated(conversationId);
    
    return true;
}

