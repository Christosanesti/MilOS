#include "usb_authorization.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStorageInfo>
#include <QFile>
#include <QTextStream>

USBAuthorization::USBAuthorization(QObject* parent)
    : QObject(parent)
{
}

USBAuthorization::~USBAuthorization() {
}

bool USBAuthorization::initialize() {
    return true;
}

QList<QString> USBAuthorization::detectUSBDevices() const {
    QList<QString> devices;
    
    // Detect USB storage devices
    // Check /media and /mnt for mounted USB devices
    QDir mediaDir("/media");
    if (mediaDir.exists()) {
        QStringList entries = mediaDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString& entry : entries) {
            QString path = mediaDir.absoluteFilePath(entry);
            QStorageInfo storage(path);
            if (storage.isRemovable() && storage.isValid()) {
                devices.append(path);
            }
        }
    }
    
    // Check /mnt
    QDir mntDir("/mnt");
    if (mntDir.exists()) {
        QStringList entries = mntDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString& entry : entries) {
            QString path = mntDir.absoluteFilePath(entry);
            QStorageInfo storage(path);
            if (storage.isRemovable() && storage.isValid()) {
                devices.append(path);
            }
        }
    }
    
    // Check /dev/sd* for USB block devices
    QDir devDir("/dev");
    QStringList blockDevices = devDir.entryList(QStringList() << "sd*", QDir::System);
    for (const QString& device : blockDevices) {
        QString devicePath = devDir.absoluteFilePath(device);
        if (QFileInfo(devicePath).exists()) {
            devices.append(devicePath);
        }
    }
    
    m_detectedDevices = devices;
    return devices;
}

USBKeyInfo USBAuthorization::validateUSBKey(const QString& devicePath) const {
    if (!checkDevicePath(devicePath)) {
        emit usbKeyValidationFailed(devicePath, "Invalid device path");
        return USBKeyInfo();
    }
    
    USBKeyInfo keyInfo = findKeyByDevicePath(devicePath);
    if (keyInfo.keyId.isEmpty()) {
        emit usbKeyValidationFailed(devicePath, "Key not found on device");
        return USBKeyInfo();
    }
    
    // Check if key is revoked
    if (keyInfo.status == USBKeyStatus::Revoked) {
        emit usbKeyValidationFailed(devicePath, "Key has been revoked");
        return USBKeyInfo();
    }
    
    // Check if key is expired
    if (keyInfo.status == USBKeyStatus::Expired || 
        (keyInfo.expiresAt.isValid() && QDateTime::currentDateTime() > keyInfo.expiresAt)) {
        emit usbKeyValidationFailed(devicePath, "Key has expired");
        return USBKeyInfo();
    }
    
    emit usbKeyValidated(keyInfo.keyId);
    return keyInfo;
}

bool USBAuthorization::isKeyValid(const QString& keyId) const {
    for (const USBKeyInfo& key : m_authorizedKeys) {
        if (key.keyId == keyId) {
            return key.status == USBKeyStatus::Active &&
                   (!key.expiresAt.isValid() || QDateTime::currentDateTime() <= key.expiresAt);
        }
    }
    return false;
}

bool USBAuthorization::checkDevicePath(const QString& devicePath) const {
    QFileInfo info(devicePath);
    return info.exists() && (info.isDir() || info.isBlockDevice());
}

USBKeyInfo USBAuthorization::findKeyByDevicePath(const QString& devicePath) const {
    // This would read key from USB device
    // For now, search in authorized keys
    for (const USBKeyInfo& key : m_authorizedKeys) {
        if (key.devicePath == devicePath) {
            return key;
        }
    }
    
    // Try to read key from USB device file
    QFileInfo info(devicePath);
    if (info.isDir()) {
        QString keyFilePath = QDir(devicePath).absoluteFilePath(".milos_key");
        QFile keyFile(keyFilePath);
        if (keyFile.exists() && keyFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&keyFile);
            USBKeyInfo keyInfo;
            keyInfo.keyId = stream.readLine();
            keyInfo.userId = stream.readLine();
            QString keyDataBase64 = stream.readLine();
            keyInfo.keyData = QByteArray::fromBase64(keyDataBase64.toUtf8());
            int roleInt = stream.readLine().toInt();
            keyInfo.role = static_cast<UserRole>(roleInt);
            QString createdAtStr = stream.readLine();
            keyInfo.createdAt = QDateTime::fromString(createdAtStr, Qt::ISODate);
            if (!stream.atEnd()) {
                QString expiresAtStr = stream.readLine();
                if (!expiresAtStr.isEmpty()) {
                    keyInfo.expiresAt = QDateTime::fromString(expiresAtStr, Qt::ISODate);
                }
            }
            keyInfo.devicePath = devicePath;
            keyInfo.status = USBKeyStatus::Active;
            keyFile.close();
            return keyInfo;
        }
    }
    
    return USBKeyInfo();
}

