#include "encryption_storage.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>

EncryptionStorage::EncryptionStorage(QObject* parent)
    : QObject(parent)
    , m_enabled(false)
{
}

EncryptionStorage::~EncryptionStorage() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool EncryptionStorage::initialize(const QString& dbPath, const QByteArray& encryptionKey) {
    if (encryptionKey.isEmpty()) {
        return false;
    }

    m_encryptionKey = encryptionKey;
    m_database = QSqlDatabase::addDatabase("QSQLITE", "SecureMessengerEncrypted");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open encrypted database:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qWarning() << "Failed to create encrypted tables";
        return false;
    }
    
    return true;
}

bool EncryptionStorage::storeEncryptedMessage(const QString& messageId, const QByteArray& encryptedData) {
    if (!m_enabled) {
        return false;
    }

    QByteArray encrypted = encryptData(encryptedData);
    
    QSqlQuery query(m_database);
    query.prepare("INSERT OR REPLACE INTO encrypted_messages (message_id, encrypted_data) VALUES (:message_id, :encrypted_data)");
    query.bindValue(":message_id", messageId);
    query.bindValue(":encrypted_data", encrypted);
    
    if (!query.exec()) {
        qWarning() << "Failed to store encrypted message:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QByteArray EncryptionStorage::retrieveEncryptedMessage(const QString& messageId) const {
    if (!m_enabled) {
        return QByteArray();
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT encrypted_data FROM encrypted_messages WHERE message_id = :message_id");
    query.bindValue(":message_id", messageId);
    
    if (!query.exec() || !query.next()) {
        return QByteArray();
    }
    
    QByteArray encrypted = query.value(0).toByteArray();
    return decryptData(encrypted);
}

bool EncryptionStorage::deleteEncryptedMessage(const QString& messageId) {
    if (!m_enabled) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare("DELETE FROM encrypted_messages WHERE message_id = :message_id");
    query.bindValue(":message_id", messageId);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete encrypted message:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool EncryptionStorage::isEnabled() const {
    return m_enabled;
}

void EncryptionStorage::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool EncryptionStorage::createTables() {
    QSqlQuery query(m_database);
    
    QString createTable = "CREATE TABLE IF NOT EXISTS encrypted_messages ("
                          "message_id TEXT PRIMARY KEY, "
                          "encrypted_data BLOB NOT NULL"
                          ")";
    
    if (!query.exec(createTable)) {
        qWarning() << "Failed to create encrypted_messages table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QByteArray EncryptionStorage::encryptData(const QByteArray& data) const {
    // In production, would use PQC Abstraction Layer to encrypt
    // For now, placeholder encryption
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(data);
    hash.addData(m_encryptionKey);
    return data + hash.result();
}

QByteArray EncryptionStorage::decryptData(const QByteArray& encryptedData) const {
    // In production, would use PQC Abstraction Layer to decrypt
    // For now, placeholder decryption
    if (encryptedData.size() < 32) {
        return QByteArray();
    }
    return encryptedData.left(encryptedData.size() - 32);
}

