#include "email_storage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

EmailStorage::EmailStorage(QObject* parent)
    : QObject(parent)
{
}

EmailStorage::~EmailStorage() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool EmailStorage::initialize(const QString& dbPath) {
    m_database = QSqlDatabase::addDatabase("QSQLITE", "email_storage");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open email database:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qWarning() << "Failed to create email tables";
        return false;
    }
    
    return true;
}

bool EmailStorage::createTables() {
    QSqlQuery query(m_database);
    
    query.prepare("CREATE TABLE IF NOT EXISTS emails ("
                  "id TEXT PRIMARY KEY,"
                  "account_id TEXT,"
                  "from_address TEXT,"
                  "to_addresses TEXT,"
                  "subject TEXT,"
                  "sent_at TEXT,"
                  "received_at TEXT,"
                  "is_read INTEGER,"
                  "is_encrypted INTEGER,"
                  "encrypted_data BLOB"
                  ")");
    
    if (!query.exec()) {
        qWarning() << "Failed to create emails table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool EmailStorage::storeEmail(const EmailMessage& message, const QByteArray& encryptedData) {
    QSqlQuery query(m_database);
    
    query.prepare("INSERT OR REPLACE INTO emails "
                  "(id, account_id, from_address, to_addresses, subject, sent_at, received_at, is_read, is_encrypted, encrypted_data) "
                  "VALUES (:id, :account_id, :from_address, :to_addresses, :subject, :sent_at, :received_at, :is_read, :is_encrypted, :encrypted_data)");
    
    query.bindValue(":id", message.id);
    query.bindValue(":account_id", message.metadata.value("account_id").toString());
    query.bindValue(":from_address", message.from);
    query.bindValue(":to_addresses", message.to.join(";"));
    query.bindValue(":subject", message.subject);
    query.bindValue(":sent_at", message.sentAt.toString(Qt::ISODate));
    query.bindValue(":received_at", message.receivedAt.toString(Qt::ISODate));
    query.bindValue(":is_read", message.isRead ? 1 : 0);
    query.bindValue(":is_encrypted", message.isEncrypted ? 1 : 0);
    query.bindValue(":encrypted_data", encryptedData);
    
    if (!query.exec()) {
        qWarning() << "Failed to store email:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QByteArray EmailStorage::retrieveEmail(const QString& messageId) const {
    QSqlQuery query(m_database);
    
    query.prepare("SELECT encrypted_data FROM emails WHERE id = :id");
    query.bindValue(":id", messageId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toByteArray();
    }
    
    return QByteArray();
}

QList<QString> EmailStorage::getAllEmailIds(const QString& accountId) const {
    QList<QString> ids;
    QSqlQuery query(m_database);
    
    if (accountId.isEmpty()) {
        query.prepare("SELECT id FROM emails ORDER BY received_at DESC");
    } else {
        query.prepare("SELECT id FROM emails WHERE account_id = :account_id ORDER BY received_at DESC");
        query.bindValue(":account_id", accountId);
    }
    
    if (query.exec()) {
        while (query.next()) {
            ids.append(query.value(0).toString());
        }
    }
    
    return ids;
}

