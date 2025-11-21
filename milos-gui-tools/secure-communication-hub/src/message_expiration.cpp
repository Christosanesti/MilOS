#include "message_expiration.h"
#include <QTimer>
#include <QDebug>

MessageExpirationManager::MessageExpirationManager(QObject* parent)
    : QObject(parent)
    , m_hub(nullptr)
    , m_checkTimer(new QTimer(this))
{
    connect(m_checkTimer, &QTimer::timeout, this, &MessageExpirationManager::onExpirationCheck);
}

MessageExpirationManager::~MessageExpirationManager() {
}

bool MessageExpirationManager::initialize(CommunicationHub* hub) {
    if (!hub) {
        return false;
    }
    
    m_hub = hub;
    m_checkTimer->setInterval(60000);  // Check every minute
    m_checkTimer->start();
    
    return true;
}

bool MessageExpirationManager::setExpiration(const QString& messageId, const QDateTime& expirationTime) {
    m_expirations[messageId] = expirationTime;
    return true;
}

QDateTime MessageExpirationManager::getExpiration(const QString& messageId) const {
    return m_expirations.value(messageId, QDateTime());
}

void MessageExpirationManager::checkExpiredMessages() {
    QDateTime now = QDateTime::currentDateTime();
    QStringList expiredIds;
    
    for (auto it = m_expirations.begin(); it != m_expirations.end(); ++it) {
        if (it.value() <= now) {
            expiredIds.append(it.key());
        }
    }
    
    for (const QString& messageId : expiredIds) {
        if (m_hub) {
            m_hub->deleteMessage(messageId);
        }
        m_expirations.remove(messageId);
        emit messageExpired(messageId);
    }
}

void MessageExpirationManager::onExpirationCheck() {
    checkExpiredMessages();
}

