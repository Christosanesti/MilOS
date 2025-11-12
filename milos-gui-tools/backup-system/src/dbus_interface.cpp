#include "dbus_interface.h"
#include "backup_service.h"
#include <QDBusConnection>
#include <QDBusError>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_backupService(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(BackupService* backupService) {
    if (m_initialized) {
        return true;
    }

    m_backupService = backupService;

    if (!m_backupService) {
        std::cerr << "Backup service is null" << std::endl;
        return false;
    }

    // Connect to backup service signals
    connect(m_backupService, &BackupService::backupCompleted,
            this, &DBusInterface::BackupCompleted);
    connect(m_backupService, &BackupService::backupFailed,
            this, &DBusInterface::BackupFailed);
    connect(m_backupService, &BackupService::backupStatusChanged,
            this, [this]() {
                emit BackupStatusChanged(m_backupService->currentBackupStatus());
            });

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        std::cerr << "D-Bus interface not initialized" << std::endl;
        return false;
    }

    if (m_running) {
        return true;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        std::cerr << "Cannot connect to D-Bus session bus: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    QString objectPath = "/org/milos/BackupSystem";
    if (!connection.registerObject(objectPath, this)) {
        std::cerr << "Failed to register D-Bus object: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    QString serviceName = "org.milos.BackupSystem";
    if (!connection.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: "
                  << connection.lastError().message().toStdString() << std::endl;
        connection.unregisterObject(objectPath);
        return false;
    }

    std::cout << "D-Bus interface registered: " << serviceName.toStdString()
              << " at " << objectPath.toStdString() << std::endl;

    m_running = true;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();

    QString objectPath = "/org/milos/BackupSystem";
    QString serviceName = "org.milos.BackupSystem";

    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);

    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
}

QString DBusInterface::CreateBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName) {
    if (!m_backupService) {
        return QString();
    }

    return m_backupService->createBackup(sourcePath, destinationId, backupName);
}

bool DBusInterface::RestoreBackup(const QString& backupId, const QString& restorePath) {
    if (!m_backupService) {
        return false;
    }

    return m_backupService->restoreBackup(backupId, restorePath);
}

QString DBusInterface::ListBackups() {
    if (!m_backupService) {
        return QString("[]");
    }

    return m_backupService->listBackups();
}

bool DBusInterface::DeleteBackup(const QString& backupId) {
    if (!m_backupService) {
        return false;
    }

    return m_backupService->deleteBackup(backupId);
}

