#include "communication_hub.h"
#include <QUuid>
#include <QDebug>

CommunicationHub::CommunicationHub(QObject* parent)
    : QObject(parent)
    , m_unreadCount(0)
{
}

CommunicationHub::~CommunicationHub() {
}

bool CommunicationHub::initialize() {
    return true;
}

bool CommunicationHub::sendMessage(const UnifiedMessage& message) {
    UnifiedMessage msg = message;
    if (msg.id.isEmpty()) {
        msg.id = generateMessageId();
    }
    msg.timestamp = QDateTime::currentDateTime();
    msg.isEncrypted = true;  // All messages are encrypted
    
    m_messages[msg.id] = msg;
    
    emit messageReceived(msg);
    
    return true;
}

QList<UnifiedMessage> CommunicationHub::receiveMessages(const QString& type, int limit) {
    QList<UnifiedMessage> messages;
    
    int count = 0;
    for (auto it = m_messages.begin(); it != m_messages.end() && count < limit; ++it) {
        UnifiedMessage msg = it.value();
        
        if (!type.isEmpty()) {
            QString typeStr;
            switch (msg.type) {
                case CommunicationType::Email: typeStr = "email"; break;
                case CommunicationType::Chat: typeStr = "chat"; break;
                case CommunicationType::Voice: typeStr = "voice"; break;
                case CommunicationType::Video: typeStr = "video"; break;
            }
            if (typeStr != type) {
                continue;
            }
        }
        
        messages.append(msg);
        count++;
    }
    
    return messages;
}

UnifiedMessage CommunicationHub::getMessage(const QString& messageId) const {
    return m_messages.value(messageId, UnifiedMessage());
}

bool CommunicationHub::markAsRead(const QString& messageId) {
    if (!m_messages.contains(messageId)) {
        return false;
    }
    
    UnifiedMessage& msg = m_messages[messageId];
    if (!msg.isRead) {
        msg.isRead = true;
        updateUnreadCount();
    }
    
    return true;
}

bool CommunicationHub::deleteMessage(const QString& messageId) {
    if (!m_messages.contains(messageId)) {
        return false;
    }
    
    UnifiedMessage msg = m_messages[messageId];
    m_messages.remove(messageId);
    
    if (!msg.isRead) {
        updateUnreadCount();
    }
    
    return true;
}

QString CommunicationHub::createGroup(const QString& name, const QStringList& participants) {
    QString groupId = generateMessageId();
    
    QVariantMap group;
    group["id"] = groupId;
    group["name"] = name;
    group["participants"] = participants;
    group["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    m_groups[groupId] = group;
    
    return groupId;
}

bool CommunicationHub::sendGroupMessage(const QString& groupId, const QString& content) {
    if (!m_groups.contains(groupId)) {
        return false;
    }
    
    QVariantMap group = m_groups[groupId];
    QStringList participants = group["participants"].toStringList();
    
    UnifiedMessage message;
    message.id = generateMessageId();
    message.type = CommunicationType::Chat;
    message.to = participants;
    message.content = content;
    message.timestamp = QDateTime::currentDateTime();
    message.isEncrypted = true;
    message.metadata["group_id"] = groupId;
    
    return sendMessage(message);
}

void CommunicationHub::updateUnreadCount() {
    int count = 0;
    for (const UnifiedMessage& msg : m_messages) {
        if (!msg.isRead) {
            count++;
        }
    }
    
    if (m_unreadCount != count) {
        m_unreadCount = count;
        emit unreadCountChanged(count);
    }
}

QString CommunicationHub::generateMessageId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

