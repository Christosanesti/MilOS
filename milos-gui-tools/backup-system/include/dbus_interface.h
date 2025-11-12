#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>

// Forward declaration
class BackupService;

/**
 * @brief D-Bus Interface for Backup System
 * 
 * Implements org.milos.BackupSystem D-Bus interface.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.BackupSystem")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param backupService Backup service instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BackupService* backupService);

    /**
     * @brief Start D-Bus interface
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop D-Bus interface
     */
    void stop();

    /**
     * @brief Check if D-Bus interface is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Create backup
     * @param sourcePath Source path to backup
     * @param destinationId Destination ID
     * @param backupName Backup name
     * @return Backup ID if successful, empty string otherwise
     */
    QString CreateBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName);

    /**
     * @brief Restore backup
     * @param backupId Backup ID to restore
     * @param restorePath Restore destination path
     * @return true if restore successful, false otherwise
     */
    bool RestoreBackup(const QString& backupId, const QString& restorePath);

    /**
     * @brief List backups
     * @return JSON string with backup list
     */
    QString ListBackups();

    /**
     * @brief Delete backup
     * @param backupId Backup ID to delete
     * @return true if delete successful, false otherwise
     */
    bool DeleteBackup(const QString& backupId);

Q_SIGNALS:
    /**
     * @brief Signal emitted when backup status changes
     */
    void BackupStatusChanged(const QString& status);

    /**
     * @brief Signal emitted when backup completes
     */
    void BackupCompleted(const QString& backupId);

    /**
     * @brief Signal emitted when backup fails
     */
    void BackupFailed(const QString& error);
};

#endif // DBUS_INTERFACE_H

