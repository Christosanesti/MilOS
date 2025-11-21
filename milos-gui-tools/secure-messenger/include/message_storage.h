#ifndef MESSAGE_STORAGE_H
#define MESSAGE_STORAGE_H

#include "messaging_core.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSqlDatabase>

/**
 * @brief Message Storage
 * 
 * Manages message persistence in SQLite database.
 */
class MessageStorage : public QObject {
    Q_OBJECT

public:
    explicit MessageStorage(QObject* parent = nullptr);
    ~MessageStorage();

    /**
     * @brief Initialize message storage
     * @param dbPath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Store message
     * @param message Message to store
     * @return true if store successful, false otherwise
     */
    bool storeMessage(const Message& message);

    /**
     * @brief Retrieve message
     * @param messageId Message ID
     * @return Message or empty if not found
     */
    Message retrieveMessage(const QString& messageId) const;

    /**
     * @brief Get messages for conversation
     * @param conversationId Conversation ID
     * @param limit Maximum number of messages
     * @param offset Offset for pagination
     * @return List of messages
     */
    QList<Message> getMessagesForConversation(const QString& conversationId,
                                              int limit = 100,
                                              int offset = 0) const;

    /**
     * @brief Update message status
     * @param messageId Message ID
     * @param status New status
     * @param timestamp Timestamp for status change
     * @return true if update successful, false otherwise
     */
    bool updateMessageStatus(const QString& messageId,
                            MessageStatus status,
                            const QDateTime& timestamp = QDateTime::currentDateTime());

    /**
     * @brief Delete message
     * @param messageId Message ID
     * @return true if delete successful, false otherwise
     */
    bool deleteMessage(const QString& messageId);

    /**
     * @brief Get message statistics
     * @return Map with total, delivered, pending, failed counts
     */
    QVariantMap getMessageStatistics() const;

    /**
     * @brief Get total message count
     * @return Total number of messages
     */
    int getTotalMessageCount() const;

private:
    QSqlDatabase m_database;
    bool createTables();
    Message messageFromQuery(const QSqlQuery& query) const;
};

#endif // MESSAGE_STORAGE_H

