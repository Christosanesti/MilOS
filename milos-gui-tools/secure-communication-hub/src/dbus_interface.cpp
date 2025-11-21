#include "dbus_interface.h"
#include "communication_hub.h"
#include <QDebug>
#include <QDBusConnection>

SecureCommunicationHubDBusInterface::SecureCommunicationHubDBusInterface(QObject* parent)
    : QObject(parent)
    , m_hub(nullptr)
{
}

SecureCommunicationHubDBusInterface::~SecureCommunicationHubDBusInterface() {
}

bool SecureCommunicationHubDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.SecureCommunicationHub")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SecureCommunicationHub", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void SecureCommunicationHubDBusInterface::setHub(CommunicationHub* hub) {
    m_hub = hub;
}

bool SecureCommunicationHubDBusInterface::SendMessage(const QString& type, const QString& to, const QString& content) {
    if (!m_hub) {
        return false;
    }
    
    UnifiedMessage message;
    message.type = (type == "email") ? CommunicationType::Email :
                   (type == "voice") ? CommunicationType::Voice :
                   (type == "video") ? CommunicationType::Video :
                   CommunicationType::Chat;
    message.to = QStringList() << to;
    message.content = content;
    
    return m_hub->sendMessage(message);
}

QStringList SecureCommunicationHubDBusInterface::ReceiveMessages(const QString& type, int limit) {
    QStringList result;
    
    if (!m_hub) {
        return result;
    }
    
    QList<UnifiedMessage> messages = m_hub->receiveMessages(type, limit);
    for (const UnifiedMessage& msg : messages) {
        result.append(msg.id);
    }
    
    return result;
}

QVariantMap SecureCommunicationHubDBusInterface::GetMessage(const QString& messageId) {
    QVariantMap result;
    
    if (!m_hub) {
        return result;
    }
    
    UnifiedMessage message = m_hub->getMessage(messageId);
    
    result["id"] = message.id;
    result["type"] = static_cast<int>(message.type);
    result["from"] = message.from;
    result["to"] = message.to;
    result["subject"] = message.subject;
    result["content"] = message.content;
    result["timestamp"] = message.timestamp.toString(Qt::ISODate);
    result["is_encrypted"] = message.isEncrypted;
    result["is_read"] = message.isRead;
    
    return result;
}

bool SecureCommunicationHubDBusInterface::MarkAsRead(const QString& messageId) {
    if (!m_hub) {
        return false;
    }
    
    return m_hub->markAsRead(messageId);
}

int SecureCommunicationHubDBusInterface::GetUnreadCount() {
    if (!m_hub) {
        return 0;
    }
    
    return m_hub->unreadCount();
}

QString SecureCommunicationHubDBusInterface::CreateGroup(const QString& name, const QStringList& participants) {
    if (!m_hub) {
        return QString();
    }
    
    return m_hub->createGroup(name, participants);
}

