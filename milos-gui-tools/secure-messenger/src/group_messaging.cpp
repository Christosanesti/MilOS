#include "group_messaging.h"
#include "conversation_manager.h"
#include <QDebug>

GroupMessaging::GroupMessaging(QObject* parent)
    : QObject(parent)
    , m_conversationManager(nullptr)
    , m_maxParticipants(50)
{
}

GroupMessaging::~GroupMessaging() {
}

bool GroupMessaging::initialize(int maxParticipants) {
    m_maxParticipants = maxParticipants;
    return true;
}

QString GroupMessaging::createGroup(const QString& name, const QString& creatorId, const QStringList& initialParticipants) {
    if (!m_conversationManager) {
        return QString();
    }

    QStringList participants = initialParticipants;
    if (!participants.contains(creatorId)) {
        participants.prepend(creatorId);
    }

    if (participants.size() > m_maxParticipants) {
        return QString();
    }

    QString conversationId = m_conversationManager->createConversation(ConversationType::Group, participants, name);
    return conversationId;
}

bool GroupMessaging::addParticipant(const QString& conversationId, const QString& participantId) {
    if (!m_conversationManager) {
        return false;
    }

    Conversation conv = m_conversationManager->getConversation(conversationId);
    if (conv.conversationId.isEmpty() || conv.type != ConversationType::Group) {
        return false;
    }

    if (conv.participants.contains(participantId)) {
        return true;  // Already a participant
    }

    if (conv.participants.size() >= m_maxParticipants) {
        return false;  // Limit reached
    }

    // Update conversation with new participant using ConversationManager
    if (m_conversationManager->addParticipant(conversationId, participantId)) {
        emit participantAdded(conversationId, participantId);
        return true;
    }

    return false;
}

bool GroupMessaging::removeParticipant(const QString& conversationId, const QString& participantId) {
    if (!m_conversationManager) {
        return false;
    }

    Conversation conv = m_conversationManager->getConversation(conversationId);
    if (conv.conversationId.isEmpty() || conv.type != ConversationType::Group) {
        return false;
    }

    if (!conv.participants.contains(participantId)) {
        return false;  // Not a participant
    }

    // Update conversation to remove participant using ConversationManager
    if (m_conversationManager->removeParticipant(conversationId, participantId)) {
        emit participantRemoved(conversationId, participantId);
        return true;
    }

    return false;
}

QStringList GroupMessaging::getParticipants(const QString& conversationId) const {
    if (!m_conversationManager) {
        return QStringList();
    }

    Conversation conv = m_conversationManager->getConversation(conversationId);
    if (conv.conversationId.isEmpty() || conv.type != ConversationType::Group) {
        return QStringList();
    }

    return conv.participants.values();
}

bool GroupMessaging::isParticipantLimitReached(const QString& conversationId) const {
    if (!m_conversationManager) {
        return true;
    }

    Conversation conv = m_conversationManager->getConversation(conversationId);
    if (conv.conversationId.isEmpty() || conv.type != ConversationType::Group) {
        return true;
    }

    return conv.participants.size() >= m_maxParticipants;
}

void GroupMessaging::setConversationManager(ConversationManager* conversationManager) {
    m_conversationManager = conversationManager;
}

