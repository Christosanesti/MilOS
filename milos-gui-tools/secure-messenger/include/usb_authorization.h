#ifndef USB_AUTHORIZATION_H
#define USB_AUTHORIZATION_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QVariantMap>
#include <QList>

/**
 * @brief USB Key Status
 */
enum class USBKeyStatus {
    Active,
    Revoked,
    Expired,
    Pending
};

/**
 * @brief User Role
 */
enum class UserRole {
    CommandingOfficer,
    TechPersonnel,
    User
};

/**
 * @brief USB Key Information
 */
struct USBKeyInfo {
    QString keyId;
    QString userId;
    QByteArray keyData;
    USBKeyStatus status;
    QDateTime createdAt;
    QDateTime expiresAt;
    QString devicePath;
    UserRole role;
};

/**
 * @brief USB Authorization
 * 
 * Manages USB-based authorization keys for secure messenger access.
 */
class USBAuthorization : public QObject {
    Q_OBJECT

public:
    explicit USBAuthorization(QObject* parent = nullptr);
    ~USBAuthorization();

    /**
     * @brief Initialize USB authorization
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Detect USB devices
     * @return List of detected USB device paths
     */
    QList<QString> detectUSBDevices() const;

    /**
     * @brief Validate USB key
     * @param devicePath USB device path
     * @return Key information if valid, empty if invalid
     */
    USBKeyInfo validateUSBKey(const QString& devicePath) const;

    /**
     * @brief Check if USB key is valid
     * @param keyId Key ID
     * @return true if key is valid, false otherwise
     */
    bool isKeyValid(const QString& keyId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when USB device is detected
     */
    void usbDeviceDetected(const QString& devicePath);

    /**
     * @brief Emitted when USB key is validated
     */
    void usbKeyValidated(const QString& keyId);

    /**
     * @brief Emitted when USB key validation fails
     */
    void usbKeyValidationFailed(const QString& devicePath, const QString& reason);

private:
    QList<USBKeyInfo> m_authorizedKeys;
    QStringList m_detectedDevices;
    
    bool checkDevicePath(const QString& devicePath) const;
    USBKeyInfo findKeyByDevicePath(const QString& devicePath) const;
};

#endif // USB_AUTHORIZATION_H

