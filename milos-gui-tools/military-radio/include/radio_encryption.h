#ifndef RADIO_ENCRYPTION_H
#define RADIO_ENCRYPTION_H

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief Radio Encryption
 * 
 * Handles encryption for radio communications.
 */
class RadioEncryption : public QObject {
    Q_OBJECT

public:
    explicit RadioEncryption(QObject* parent = nullptr);
    ~RadioEncryption();

    /**
     * @brief Initialize radio encryption
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Encrypt radio message
     * @param payload Message payload
     * @param key Encryption key
     * @return Encrypted data
     */
    QByteArray encrypt(const QByteArray& payload, const QByteArray& key) const;

    /**
     * @brief Decrypt radio message
     * @param encryptedData Encrypted data
     * @param key Decryption key
     * @return Decrypted payload
     */
    QByteArray decrypt(const QByteArray& encryptedData, const QByteArray& key) const;

    /**
     * @brief Generate encryption key
     * @param length Key length in bytes
     * @return Generated key
     */
    QByteArray generateKey(int length = 32) const;

private:
    QByteArray applyEncryption(const QByteArray& data, const QByteArray& key, bool encrypt) const;
};

#endif // RADIO_ENCRYPTION_H

