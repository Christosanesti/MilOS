#ifndef E2E_ENCRYPTION_H
#define E2E_ENCRYPTION_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>

/**
 * @brief End-to-End Encryption
 * 
 * Provides end-to-end encryption for all message types using PQC.
 */
class E2EEncryption : public QObject {
    Q_OBJECT

public:
    explicit E2EEncryption(QObject* parent = nullptr);
    ~E2EEncryption();

    /**
     * @brief Initialize E2E encryption
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Encrypt message
     * @param messageData Message data
     * @param recipientId Recipient ID
     * @return Encrypted data or empty if failed
     */
    QByteArray encryptMessage(const QByteArray& messageData, const QString& recipientId);

    /**
     * @brief Decrypt message
     * @param encryptedData Encrypted data
     * @param senderId Sender ID
     * @return Decrypted data or empty if failed
     */
    QByteArray decryptMessage(const QByteArray& encryptedData, const QString& senderId);

    /**
     * @brief Encrypt file
     * @param fileData File data
     * @param recipientId Recipient ID
     * @return Encrypted data or empty if failed
     */
    QByteArray encryptFile(const QByteArray& fileData, const QString& recipientId);

    /**
     * @brief Decrypt file
     * @param encryptedData Encrypted data
     * @param senderId Sender ID
     * @return Decrypted data or empty if failed
     */
    QByteArray decryptFile(const QByteArray& encryptedData, const QString& senderId);

    /**
     * @brief Encrypt media
     * @param mediaData Media data
     * @param recipientId Recipient ID
     * @return Encrypted data or empty if failed
     */
    QByteArray encryptMedia(const QByteArray& mediaData, const QString& recipientId);

    /**
     * @brief Decrypt media
     * @param encryptedData Encrypted data
     * @param senderId Sender ID
     * @return Decrypted data or empty if failed
     */
    QByteArray decryptMedia(const QByteArray& encryptedData, const QString& senderId);

    /**
     * @brief Encrypt message for group (encrypts for all participants)
     * @param messageData Message data
     * @param participantIds List of participant IDs
     * @return Encrypted data map (participant ID -> encrypted data) or empty if failed
     */
    QMap<QString, QByteArray> encryptForGroup(const QByteArray& messageData, const QStringList& participantIds);

Q_SIGNALS:
    /**
     * @brief Emitted when encryption fails
     */
    void encryptionFailed(const QString& error);

    /**
     * @brief Emitted when decryption fails
     */
    void decryptionFailed(const QString& error);

private:
    QMap<QString, QByteArray> m_recipientKeys;  // Recipient ID -> Public Key
    QByteArray m_localPrivateKey;
    QByteArray m_localPublicKey;
    
    bool generateKeyPair();
    QByteArray getRecipientPublicKey(const QString& recipientId) const;
    void setRecipientPublicKey(const QString& recipientId, const QByteArray& publicKey);
};

#endif // E2E_ENCRYPTION_H

