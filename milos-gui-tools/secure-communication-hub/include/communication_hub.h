#ifndef COMMUNICATION_HUB_H
#define COMMUNICATION_HUB_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Communication Type
 */
enum class CommunicationType {
    Email,
    Chat,
    Voice,
    Video
};

/**
 * @brief Unified Message
 */
struct UnifiedMessage {
    QString id;
    CommunicationType type;
    QString from;
    QStringList to;
    QString subject;
    QString content;
    QDateTime timestamp;
    bool isEncrypted;
    bool isRead;
    QStringList attachments;
    QVariantMap metadata;
};

/**
 * @brief Communication Hub
 * 
 * Unified encrypted messaging hub (email, chat, voice).
 */
class CommunicationHub : public QObject {
    Q_OBJECT
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)

public:
    explicit CommunicationHub(QObject* parent = nullptr);
    ~CommunicationHub();

    /**
     * @brief Initialize communication hub
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Send message
     * @param message Unified message
     * @return true if send successful, false otherwise
     */
    bool sendMessage(const UnifiedMessage& message);

    /**
     * @brief Receive messages
     * @param type Communication type (optional, all if empty)
     * @param limit Maximum number of messages
     * @return List of unified messages
     */
    QList<UnifiedMessage> receiveMessages(const QString& type = QString(), int limit = 50);

    /**
     * @brief Get message
     * @param messageId Message ID
     * @return Unified message or empty if not found
     */
    UnifiedMessage getMessage(const QString& messageId) const;

    /**
     * @brief Mark message as read
     * @param messageId Message ID
     * @return true if mark successful, false otherwise
     */
    bool markAsRead(const QString& messageId);

    /**
     * @brief Delete message
     * @param messageId Message ID
     * @return true if delete successful, false otherwise
     */
    bool deleteMessage(const QString& messageId);

    /**
     * @brief Get unread count
     * @return Number of unread messages
     */
    int unreadCount() const { return m_unreadCount; }

    /**
     * @brief Create group conversation
     * @param name Group name
     * @param participants List of participant IDs
     * @return Group ID or empty if failed
     */
    QString createGroup(const QString& name, const QStringList& participants);

    /**
     * @brief Send group message
     * @param groupId Group ID
     * @param content Message content
     * @return true if send successful, false otherwise
     */
    bool sendGroupMessage(const QString& groupId, const QString& content);

Q_SIGNALS:
    /**
     * @brief Emitted when message is received
     */
    void messageReceived(const UnifiedMessage& message);

    /**
     * @brief Emitted when unread count changes
     */
    void unreadCountChanged(int count);

private:
    QMap<QString, UnifiedMessage> m_messages;
    QMap<QString, QVariantMap> m_groups;
    int m_unreadCount;
    
    void updateUnreadCount();
    QString generateMessageId() const;
};

Q_DECLARE_METATYPE(CommunicationType)
Q_DECLARE_METATYPE(UnifiedMessage)

#endif // COMMUNICATION_HUB_H

