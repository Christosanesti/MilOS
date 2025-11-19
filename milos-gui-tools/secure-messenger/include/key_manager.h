#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include "usb_authorization.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

/**
 * @brief Key Manager
 * 
 * Manages USB authorization keys: distribution, revocation, and tracking.
 */
class KeyManager : public QObject {
    Q_OBJECT

public:
    explicit KeyManager(QObject* parent = nullptr);
    ~KeyManager();

    /**
     * @brief Initialize key manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Distribute key
     * @param keyInfo Key information
     * @param devicePath USB device path
     * @return true if distribution successful, false otherwise
     */
    bool distributeKey(const USBKeyInfo& keyInfo, const QString& devicePath);

    /**
     * @brief Revoke key
     * @param keyId Key ID
     * @return true if revocation successful, false otherwise
     */
    bool revokeKey(const QString& keyId);

    /**
     * @brief Get key information
     * @param keyId Key ID
     * @return Key information or empty if not found
     */
    USBKeyInfo getKeyInfo(const QString& keyId) const;

    /**
     * @brief Get all keys
     * @param userId Optional user ID filter
     * @return List of key information
     */
    QList<USBKeyInfo> getAllKeys(const QString& userId = QString()) const;

    /**
     * @brief Get revoked keys
     * @return List of revoked key IDs
     */
    QList<QString> getRevokedKeys() const;

Q_SIGNALS:
    /**
     * @brief Emitted when key is distributed
     */
    void keyDistributed(const QString& keyId, const QString& devicePath);

    /**
     * @brief Emitted when key is revoked
     */
    void keyRevoked(const QString& keyId);

private:
    QList<USBKeyInfo> m_keys;
    QList<QString> m_revokedKeys;
    QString generateDistributionId() const;
};

#endif // KEY_MANAGER_H

