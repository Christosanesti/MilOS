#include "emergency_eject.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

EmergencyEject::EmergencyEject(QObject* parent)
    : QObject(parent)
    , m_ejecting(false)
{
}

EmergencyEject::~EmergencyEject() {
}

bool EmergencyEject::initialize() {
    // Load stored confirmation hash from secure storage
    QSettings settings(QSettings::SystemScope, "MilOS", "EmergencyEject");
    m_confirmationHash = settings.value("confirmationHash", "").toString();
    
    // If no hash exists, generate one from system-specific data
    if (m_confirmationHash.isEmpty()) {
        generateInitialConfirmationHash();
    }
    
    return true;
}

bool EmergencyEject::executeEject(const QString& confirmationCode) {
    if (m_ejecting) {
        emit ejectFailed("Eject already in progress");
        return false;
    }

    // SECURITY: Verify confirmation code using cryptographic hash
    // This prevents simple string matching attacks
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(confirmationCode.toUtf8());
    hash.addData(getSystemSalt().toUtf8());
    QString codeHash = hash.result().toHex();
    
    if (codeHash != m_confirmationHash) {
        emit ejectFailed("Invalid confirmation code");
        return false;
    }

    m_ejecting = true;
    emit ejectInitiated();

    // Delete all data
    if (!deleteAllData()) {
        m_ejecting = false;
        emit ejectFailed("Failed to delete all data");
        return false;
    }

    // Dismantle system
    if (!dismantleSystem()) {
        m_ejecting = false;
        emit ejectFailed("Failed to dismantle system");
        return false;
    }

    // Verify deletion
    if (!verifyDeletion()) {
        m_ejecting = false;
        emit ejectFailed("Failed to verify deletion");
        return false;
    }

    m_ejecting = false;
    emit ejectCompleted();

    return true;
}

bool EmergencyEject::isEjecting() const {
    return m_ejecting;
}

bool EmergencyEject::deleteAllData() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    
    if (appDataDir.exists()) {
        return appDataDir.removeRecursively();
    }
    
    return true;
}

bool EmergencyEject::dismantleSystem() {
    // Stop all MilOS services via systemd
    QStringList services = {
        "milos-data-guard.service",
        "milos-audit.service",
        "milos-update.service",
        "milos-file-integrity.service",
        "milos-secrets-management.service"
    };
    
    QProcess process;
    for (const QString& service : services) {
        process.start("systemctl", QStringList() << "--user" << "stop" << service);
        if (!process.waitForFinished(5000)) {
            qWarning() << "Failed to stop service:" << service;
            // Continue with other services
        }
    }
    
    // Close all D-Bus connections
    QDBusConnection::sessionBus().disconnectFromBus();
    QDBusConnection::systemBus().disconnectFromBus();
    
    // Kill any remaining MilOS processes
    process.start("pkill", QStringList() << "-f" << "milos");
    process.waitForFinished(3000);
    
    return true;
}

QString EmergencyEject::getSystemSalt() const {
    // Generate system-specific salt from machine ID and user data
    QFile machineId("/etc/machine-id");
    QString salt = "MilOS-Emergency-Eject-Salt";
    
    if (machineId.open(QIODevice::ReadOnly)) {
        salt += machineId.readAll().trimmed();
        machineId.close();
    }
    
    // Add user-specific data
    salt += QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    
    return salt;
}

void EmergencyEject::generateInitialConfirmationHash() {
    // Generate initial hash from system data
    // User must set their own confirmation code via secure interface
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(getSystemSalt().toUtf8());
    hash.addData(QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8());
    m_confirmationHash = hash.result().toHex();
    
    // Store hash securely
    QSettings settings(QSettings::SystemScope, "MilOS", "EmergencyEject");
    settings.setValue("confirmationHash", m_confirmationHash);
    settings.sync();
}

bool EmergencyEject::setConfirmationCode(const QString& currentCode, const QString& newCode) {
    if (newCode.isEmpty() || newCode.length() < 8) {
        emit ejectFailed("Confirmation code must be at least 8 characters");
        return false;
    }
    
    // Verify current code if hash exists
    if (!m_confirmationHash.isEmpty()) {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(currentCode.toUtf8());
        hash.addData(getSystemSalt().toUtf8());
        QString currentHash = hash.result().toHex();
        
        if (currentHash != m_confirmationHash) {
            emit ejectFailed("Current confirmation code is incorrect");
            return false;
        }
    }
    
    // Set new confirmation code hash
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(newCode.toUtf8());
    hash.addData(getSystemSalt().toUtf8());
    m_confirmationHash = hash.result().toHex();
    
    // Store hash securely
    QSettings settings(QSettings::SystemScope, "MilOS", "EmergencyEject");
    settings.setValue("confirmationHash", m_confirmationHash);
    settings.sync();
    
    return true;
}

bool EmergencyEject::verifyDeletion() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    
    return !appDataDir.exists();
}


    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    
    if (appDataDir.exists()) {
        return appDataDir.removeRecursively();
    }
    
    return true;
}

bool EmergencyEject::dismantleSystem() {
    // Stop all MilOS services via systemd
    QStringList services = {
        "milos-data-guard.service",
        "milos-audit.service",
        "milos-update.service",
        "milos-file-integrity.service",
        "milos-secrets-management.service"
    };
    
    QProcess process;
    for (const QString& service : services) {
        process.start("systemctl", QStringList() << "--user" << "stop" << service);
        if (!process.waitForFinished(5000)) {
            qWarning() << "Failed to stop service:" << service;
            // Continue with other services
        }
    }
    
    // Close all D-Bus connections
    QDBusConnection::sessionBus().disconnectFromBus();
    QDBusConnection::systemBus().disconnectFromBus();
    
    // Kill any remaining MilOS processes
    process.start("pkill", QStringList() << "-f" << "milos");
    process.waitForFinished(3000);
    
    return true;
}

QString EmergencyEject::getSystemSalt() const {
    // Generate system-specific salt from machine ID and user data
    QFile machineId("/etc/machine-id");
    QString salt = "MilOS-Emergency-Eject-Salt";
    
    if (machineId.open(QIODevice::ReadOnly)) {
        salt += machineId.readAll().trimmed();
        machineId.close();
    }
    
    // Add user-specific data
    salt += QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    
    return salt;
}

void EmergencyEject::generateInitialConfirmationHash() {
    // Generate initial hash from system data
    // User must set their own confirmation code via secure interface
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(getSystemSalt().toUtf8());
    hash.addData(QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8());
    m_confirmationHash = hash.result().toHex();
    
    // Store hash securely
    QSettings settings(QSettings::SystemScope, "MilOS", "EmergencyEject");
    settings.setValue("confirmationHash", m_confirmationHash);
    settings.sync();
}

bool EmergencyEject::setConfirmationCode(const QString& currentCode, const QString& newCode) {
    if (newCode.isEmpty() || newCode.length() < 8) {
        emit ejectFailed("Confirmation code must be at least 8 characters");
        return false;
    }
    
    // Verify current code if hash exists
    if (!m_confirmationHash.isEmpty()) {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(currentCode.toUtf8());
        hash.addData(getSystemSalt().toUtf8());
        QString currentHash = hash.result().toHex();
        
        if (currentHash != m_confirmationHash) {
            emit ejectFailed("Current confirmation code is incorrect");
            return false;
        }
    }
    
    // Set new confirmation code hash
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(newCode.toUtf8());
    hash.addData(getSystemSalt().toUtf8());
    m_confirmationHash = hash.result().toHex();
    
    // Store hash securely
    QSettings settings(QSettings::SystemScope, "MilOS", "EmergencyEject");
    settings.setValue("confirmationHash", m_confirmationHash);
    settings.sync();
    
    return true;
}

bool EmergencyEject::verifyDeletion() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    
    return !appDataDir.exists();
}

