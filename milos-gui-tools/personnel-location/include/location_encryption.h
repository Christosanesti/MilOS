#ifndef LOCATION_ENCRYPTION_H
#define LOCATION_ENCRYPTION_H

#include "location_tracker.h"
#include <QObject>
#include <QByteArray>

/**
 * @brief Location Encryption
 * 
 * Encrypts location data for transmission and storage.
 */
class LocationEncryption : public QObject {
    Q_OBJECT

public:
    explicit LocationEncryption(QObject* parent = nullptr);
    ~LocationEncryption();

    /**
     * @brief Initialize location encryption
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Encrypt location data
     * @param location Location data
     * @return Encrypted data or empty if failed
     */
    QByteArray encryptLocation(const LocationData& location);

    /**
     * @brief Decrypt location data
     * @param encryptedData Encrypted data
     * @return Location data or empty if failed
     */
    LocationData decryptLocation(const QByteArray& encryptedData);

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
    QByteArray m_encryptionKey;
    bool generateEncryptionKey();
};

#endif // LOCATION_ENCRYPTION_H

