#ifndef KEY_GENERATOR_H
#define KEY_GENERATOR_H

#include "usb_authorization.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QByteArray>

/**
 * @brief Key Generator
 * 
 * Generates cryptographic keys for USB authorization using post-quantum cryptography.
 */
class KeyGenerator : public QObject {
    Q_OBJECT

public:
    explicit KeyGenerator(QObject* parent = nullptr);
    ~KeyGenerator();

    /**
     * @brief Initialize key generator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Generate USB authorization key
     * @param userId User ID
     * @param role User role
     * @param expirationDays Expiration in days (0 for no expiration)
     * @return Key information
     */
    USBKeyInfo generateKey(const QString& userId,
                          UserRole role,
                          int expirationDays = 0);

    /**
     * @brief Derive key from master key
     * @param masterKey Master key
     * @param userId User ID
     * @return Derived key data
     */
    QByteArray deriveKey(const QByteArray& masterKey, const QString& userId) const;

    /**
     * @brief Write key to USB device
     * @param keyInfo Key information
     * @param devicePath USB device path
     * @return true if write successful, false otherwise
     */
    bool writeKeyToUSB(const USBKeyInfo& keyInfo, const QString& devicePath);

Q_SIGNALS:
    /**
     * @brief Emitted when key is generated
     */
    void keyGenerated(const QString& keyId);

    /**
     * @brief Emitted when key is written to USB
     */
    void keyWrittenToUSB(const QString& keyId, const QString& devicePath);

private:
    QByteArray m_masterKey;
    QString generateKeyId() const;
    QByteArray generateRandomKey(int length) const;
};

#endif // KEY_GENERATOR_H

