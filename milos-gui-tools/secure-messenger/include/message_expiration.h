#ifndef MESSAGE_EXPIRATION_H
#define MESSAGE_EXPIRATION_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QMap>

/**
 * @brief Expiration Policy
 */
enum class ExpirationPolicy {
    Never,
    AfterRead,
    AfterTime,
    AfterDelivery
};

/**
 * @brief Message Expiration Info
 */
struct MessageExpirationInfo {
    QString messageId;
    ExpirationPolicy policy;
    QDateTime expirationTime;
    bool expired;
};

/**
 * @brief Message Expiration
 * 
 * Manages message expiration policies and auto-delete functionality.
 */
class MessageExpiration : public QObject {
    Q_OBJECT

public:
    explicit MessageExpiration(QObject* parent = nullptr);
    ~MessageExpiration();

    /**
     * @brief Initialize message expiration
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Set expiration policy for message
     * @param messageId Message ID
     * @param policy Expiration policy
     * @param expirationSeconds Expiration time in seconds (for AfterTime policy)
     * @return true if set successful, false otherwise
     */
    bool setExpirationPolicy(const QString& messageId, ExpirationPolicy policy, int expirationSeconds = 0);

    /**
     * @brief Check if message is expired
     * @param messageId Message ID
     * @return true if expired, false otherwise
     */
    bool isMessageExpired(const QString& messageId) const;

    /**
     * @brief Mark message as read (for AfterRead policy)
     * @param messageId Message ID
     */
    void markMessageAsRead(const QString& messageId);

    /**
     * @brief Mark message as delivered (for AfterDelivery policy)
     * @param messageId Message ID
     */
    void markMessageAsDelivered(const QString& messageId);

    /**
     * @brief Delete expired messages
     * @return Number of messages deleted
     */
    int deleteExpiredMessages();

Q_SIGNALS:
    /**
     * @brief Emitted when message is about to expire
     */
    void messageExpiring(const QString& messageId);

    /**
     * @brief Emitted when message is expired
     */
    void messageExpired(const QString& messageId);

private:
    QMap<QString, MessageExpirationInfo> m_expirationInfo;
    QTimer* m_expirationTimer;
    
    void checkExpirations();
    void expireMessage(const QString& messageId);
};

#endif // MESSAGE_EXPIRATION_H

