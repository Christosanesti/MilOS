#ifndef MESSAGE_EXPIRATION_H
#define MESSAGE_EXPIRATION_H

#include "communication_hub.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>

/**
 * @brief Message Expiration Manager
 * 
 * Manages message expiration and auto-delete.
 */
class MessageExpirationManager : public QObject {
    Q_OBJECT

public:
    explicit MessageExpirationManager(QObject* parent = nullptr);
    ~MessageExpirationManager();

    /**
     * @brief Initialize message expiration manager
     * @param hub Communication hub
     * @return true if initialization successful, false otherwise
     */
    bool initialize(CommunicationHub* hub);

    /**
     * @brief Set expiration for message
     * @param messageId Message ID
     * @param expirationTime Expiration time
     * @return true if set successful, false otherwise
     */
    bool setExpiration(const QString& messageId, const QDateTime& expirationTime);

    /**
     * @brief Get expiration time for message
     * @param messageId Message ID
     * @return Expiration time or invalid if not set
     */
    QDateTime getExpiration(const QString& messageId) const;

    /**
     * @brief Check and delete expired messages
     */
    void checkExpiredMessages();

Q_SIGNALS:
    /**
     * @brief Emitted when message expires
     */
    void messageExpired(const QString& messageId);

private slots:
    void onExpirationCheck();

private:
    CommunicationHub* m_hub;
    QMap<QString, QDateTime> m_expirations;
    QTimer* m_checkTimer;
};

#endif // MESSAGE_EXPIRATION_MANAGER_H

