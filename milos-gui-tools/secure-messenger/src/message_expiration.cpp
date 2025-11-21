#include "message_expiration.h"
#include "message_storage.h"
#include <QDebug>

MessageExpiration::MessageExpiration(QObject* parent)
    : QObject(parent)
    , m_messageStorage(nullptr)
    , m_expirationTimer(new QTimer(this))
{
    connect(m_expirationTimer, &QTimer::timeout, this, &MessageExpiration::checkExpirations);
    m_expirationTimer->start(60000);  // Check every minute
}

MessageExpiration::~MessageExpiration() {
}

bool MessageExpiration::initialize() {
    return true;
}

bool MessageExpiration::setExpirationPolicy(const QString& messageId, ExpirationPolicy policy, int expirationSeconds) {
    MessageExpirationInfo info;
    info.messageId = messageId;
    info.policy = policy;
    info.expired = false;

    if (policy == ExpirationPolicy::AfterTime && expirationSeconds > 0) {
        info.expirationTime = QDateTime::currentDateTime().addSecs(expirationSeconds);
    } else {
        info.expirationTime = QDateTime();  // Will be set when condition is met
    }

    m_expirationInfo[messageId] = info;
    return true;
}

bool MessageExpiration::isMessageExpired(const QString& messageId) const {
    if (!m_expirationInfo.contains(messageId)) {
        return false;
    }

    MessageExpirationInfo info = m_expirationInfo.value(messageId);
    return info.expired;
}

void MessageExpiration::markMessageAsRead(const QString& messageId) {
    if (!m_expirationInfo.contains(messageId)) {
        return;
    }

    MessageExpirationInfo& info = m_expirationInfo[messageId];
    if (info.policy == ExpirationPolicy::AfterRead) {
        expireMessage(messageId);
    }
}

void MessageExpiration::markMessageAsDelivered(const QString& messageId) {
    if (!m_expirationInfo.contains(messageId)) {
        return;
    }

    MessageExpirationInfo& info = m_expirationInfo[messageId];
    if (info.policy == ExpirationPolicy::AfterDelivery) {
        expireMessage(messageId);
    }
}

int MessageExpiration::deleteExpiredMessages() {
    int deleted = 0;
    QList<QString> expiredMessages;

    for (auto it = m_expirationInfo.begin(); it != m_expirationInfo.end(); ++it) {
        if (it.value().expired) {
            expiredMessages.append(it.key());
        }
    }

    for (const QString& messageId : expiredMessages) {
        // Delete from storage if available
        if (m_messageStorage) {
            m_messageStorage->deleteMessage(messageId);
        }
        
        m_expirationInfo.remove(messageId);
        deleted++;
    }

    return deleted;
}

void MessageExpiration::checkExpirations() {
    QDateTime now = QDateTime::currentDateTime();

    for (auto it = m_expirationInfo.begin(); it != m_expirationInfo.end(); ++it) {
        MessageExpirationInfo& info = it.value();
        
        if (info.expired) {
            continue;
        }

        if (info.policy == ExpirationPolicy::AfterTime) {
            if (info.expirationTime.isValid() && now >= info.expirationTime) {
                emit messageExpiring(info.messageId);
                expireMessage(info.messageId);
            }
        }
    }
}

void MessageExpiration::expireMessage(const QString& messageId) {
    if (!m_expirationInfo.contains(messageId)) {
        return;
    }

    MessageExpirationInfo& info = m_expirationInfo[messageId];
    info.expired = true;
    
    emit messageExpired(messageId);
    
    // Automatically delete expired messages from storage
    if (m_messageStorage) {
        m_messageStorage->deleteMessage(messageId);
    }
}

void MessageExpiration::setMessageStorage(MessageStorage* messageStorage) {
    m_messageStorage = messageStorage;
}

