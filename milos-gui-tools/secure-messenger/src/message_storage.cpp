#include "message_storage.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

MessageStorage::MessageStorage(QObject* parent)
    : QObject(parent)
{
}

MessageStorage::~MessageStorage() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool MessageStorage::initialize(const QString& dbPath) {
    m_database = QSqlDatabase::addDatabase("QSQLITE", "SecureMessengerMessages");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open message database:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qWarning() << "Failed to create message tables";
        return false;
    }
    
    return true;
}

bool MessageStorage::storeMessage(const Message& message) {
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO messages (message_id, conversation_id, sender_id, recipient_id, "
                  "type, content, data, status, timestamp, delivered_at, read_at, metadata) "
                  "VALUES (:message_id, :conversation_id, :sender_id, :recipient_id, "
                  ":type, :content, :data, :status, :timestamp, :delivered_at, :read_at, :metadata)");
    
    query.bindValue(":message_id", message.messageId);
    query.bindValue(":conversation_id", message.conversationId);
    query.bindValue(":sender_id", message.senderId);
    query.bindValue(":recipient_id", message.recipientId);
    query.bindValue(":type", static_cast<int>(message.type));
    query.bindValue(":content", message.content);
    query.bindValue(":data", message.data);
    query.bindValue(":status", static_cast<int>(message.status));
    query.bindValue(":timestamp", message.timestamp);
    query.bindValue(":delivered_at", message.deliveredAt);
    query.bindValue(":read_at", message.readAt);
    
    // Store metadata as JSON
    QJsonDocument doc = QJsonDocument::fromVariant(message.metadata);
    query.bindValue(":metadata", doc.toJson());
    
    if (!query.exec()) {
        qWarning() << "Failed to store message:" << query.lastError().text();
        return false;
    }
    
    return true;
}

Message MessageStorage::retrieveMessage(const QString& messageId) const {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM messages WHERE message_id = :message_id");
    query.bindValue(":message_id", messageId);
    
    if (!query.exec() || !query.next()) {
        return Message();
    }
    
    return messageFromQuery(query);
}

QList<Message> MessageStorage::getMessagesForConversation(const QString& conversationId,
                                                          int limit,
                                                          int offset) const {
    QList<Message> messages;
    
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM messages WHERE conversation_id = :conversation_id "
                  "ORDER BY timestamp DESC LIMIT :limit OFFSET :offset");
    query.bindValue(":conversation_id", conversationId);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    
    if (!query.exec()) {
        qWarning() << "Failed to retrieve messages:" << query.lastError().text();
        return messages;
    }
    
    while (query.next()) {
        messages.append(messageFromQuery(query));
    }
    
    return messages;
}

bool MessageStorage::updateMessageStatus(const QString& messageId,
                                        MessageStatus status,
                                        const QDateTime& timestamp) {
    QSqlQuery query(m_database);
    
    if (status == MessageStatus::Delivered) {
        query.prepare("UPDATE messages SET status = :status, delivered_at = :timestamp "
                      "WHERE message_id = :message_id");
        query.bindValue(":timestamp", timestamp);
    } else if (status == MessageStatus::Read) {
        query.prepare("UPDATE messages SET status = :status, read_at = :timestamp "
                      "WHERE message_id = :message_id");
        query.bindValue(":timestamp", timestamp);
    } else {
        query.prepare("UPDATE messages SET status = :status WHERE message_id = :message_id");
    }
    
    query.bindValue(":status", static_cast<int>(status));
    query.bindValue(":message_id", messageId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update message status:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool MessageStorage::deleteMessage(const QString& messageId) {
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM messages WHERE message_id = :message_id");
    query.bindValue(":message_id", messageId);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete message:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool MessageStorage::createTables() {
    QSqlQuery query(m_database);
    
    QString createTable = "CREATE TABLE IF NOT EXISTS messages ("
                          "message_id TEXT PRIMARY KEY, "
                          "conversation_id TEXT NOT NULL, "
                          "sender_id TEXT NOT NULL, "
                          "recipient_id TEXT NOT NULL, "
                          "type INTEGER NOT NULL, "
                          "content TEXT, "
                          "data BLOB, "
                          "status INTEGER NOT NULL, "
                          "timestamp TEXT NOT NULL, "
                          "delivered_at TEXT, "
                          "read_at TEXT, "
                          "metadata TEXT"
                          ")";
    
    if (!query.exec(createTable)) {
        qWarning() << "Failed to create messages table:" << query.lastError().text();
        return false;
    }
    
    // Create index on conversation_id for faster queries
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_conversation_id ON messages(conversation_id)")) {
        qWarning() << "Failed to create index:" << query.lastError().text();
        return false;
    }
    
    return true;
}

Message MessageStorage::messageFromQuery(const QSqlQuery& query) const {
    Message message;
    
    message.messageId = query.value("message_id").toString();
    message.conversationId = query.value("conversation_id").toString();
    message.senderId = query.value("sender_id").toString();
    message.recipientId = query.value("recipient_id").toString();
    message.type = static_cast<MessageType>(query.value("type").toInt());
    message.content = query.value("content").toString();
    message.data = query.value("data").toByteArray();
    message.status = static_cast<MessageStatus>(query.value("status").toInt());
    message.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);
    
    QString deliveredAtStr = query.value("delivered_at").toString();
    if (!deliveredAtStr.isEmpty()) {
        message.deliveredAt = QDateTime::fromString(deliveredAtStr, Qt::ISODate);
    }
    
    QString readAtStr = query.value("read_at").toString();
    if (!readAtStr.isEmpty()) {
        message.readAt = QDateTime::fromString(readAtStr, Qt::ISODate);
    }
    
    QString metadataStr = query.value("metadata").toString();
    if (!metadataStr.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(metadataStr.toUtf8());
        message.metadata = doc.object().toVariantMap();
    }
    
    return message;
}

