#include "message_threading.h"
#include <QUuid>
#include <QDebug>

MessageThreading::MessageThreading(QObject* parent)
    : QObject(parent)
{
}

MessageThreading::~MessageThreading() {
}

bool MessageThreading::initialize() {
    return true;
}

QString MessageThreading::createThread(const QString& conversationId, const QString& title) {
    MessageThread thread;
    thread.threadId = generateThreadId();
    thread.conversationId = conversationId;
    thread.title = title.isEmpty() ? "Thread" : title;
    thread.createdAt = QDateTime::currentDateTime();
    thread.lastMessageAt = QDateTime::currentDateTime();
    thread.unreadCount = 0;
    
    m_threads[thread.threadId] = thread;
    
    emit threadCreated(thread.threadId);
    
    return thread.threadId;
}

bool MessageThreading::addMessageToThread(const QString& threadId, const QString& messageId) {
    if (!m_threads.contains(threadId)) {
        return false;
    }
    
    MessageThread& thread = m_threads[threadId];
    if (!thread.messageIds.contains(messageId)) {
        thread.messageIds.append(messageId);
        thread.lastMessageAt = QDateTime::currentDateTime();
        emit messageAddedToThread(threadId, messageId);
    }
    
    return true;
}

MessageThread MessageThreading::getThread(const QString& threadId) const {
    return m_threads.value(threadId, MessageThread());
}

QList<MessageThread> MessageThreading::getThreadsForConversation(const QString& conversationId) const {
    QList<MessageThread> threads;
    for (const MessageThread& thread : m_threads.values()) {
        if (thread.conversationId == conversationId) {
            threads.append(thread);
        }
    }
    return threads;
}

QList<MessageThread> MessageThreading::getAllThreads() const {
    return m_threads.values();
}

QString MessageThreading::generateThreadId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

