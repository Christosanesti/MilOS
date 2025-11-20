#ifndef ENCRYPTION_STORAGE_H
#define ENCRYPTION_STORAGE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QSqlDatabase>

/**
 * @brief Encryption Storage
 * 
 * Provides encrypted storage for message history using PQC encryption.
 */
class EncryptionStorage : public QObject {
    Q_OBJECT

public:
    explicit EncryptionStorage(QObject* parent = nullptr);
    ~EncryptionStorage();

    /**
     * @brief Initialize encryption storage
     * @param dbPath Database file path
     * @param encryptionKey Encryption key
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath, const QByteArray& encryptionKey);

    /**
     * @brief Store encrypted message
     * @param messageId Message ID
     * @param encryptedData Encrypted message data
     * @return true if store successful, false otherwise
     */
    bool storeEncryptedMessage(const QString& messageId, const QByteArray& encryptedData);

    /**
     * @brief Retrieve encrypted message
     * @param messageId Message ID
     * @return Encrypted data or empty if not found
     */
    QByteArray retrieveEncryptedMessage(const QString& messageId) const;

    /**
     * @brief Delete encrypted message
     * @param messageId Message ID
     * @return true if delete successful, false otherwise
     */
    bool deleteEncryptedMessage(const QString& messageId);

    /**
     * @brief Check if encryption storage is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const;

    /**
     * @brief Set encryption storage enabled
     * @param enabled Enabled state
     */
    void setEnabled(bool enabled);

private:
    QSqlDatabase m_database;
    QByteArray m_encryptionKey;
    bool m_enabled;
    
    bool createTables();
    QByteArray encryptData(const QByteArray& data) const;
    QByteArray decryptData(const QByteArray& encryptedData) const;
};

#endif // ENCRYPTION_STORAGE_H

