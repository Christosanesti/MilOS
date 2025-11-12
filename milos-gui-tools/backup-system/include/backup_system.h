#ifndef BACKUP_SYSTEM_H
#define BACKUP_SYSTEM_H

#include <QObject>
#include <memory>

// Forward declarations
class BackupService;
class DBusInterface;
class ConfigParser;

/**
 * @brief Backup System
 * 
 * Main application class for Zero-Knowledge Backup System.
 */
class BackupSystem : public QObject {
    Q_OBJECT

public:
    explicit BackupSystem(QObject* parent = nullptr);
    ~BackupSystem();

    /**
     * @brief Initialize backup system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start backup system
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop backup system
     */
    void stop();

    /**
     * @brief Get Backup Service instance
     * @return Backup Service instance
     */
    BackupService* getBackupService() const { return m_backupService.get(); }

private:
    bool m_initialized;
    bool m_running;

    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<BackupService> m_backupService;
    std::unique_ptr<DBusInterface> m_dbusInterface;
};

#endif // BACKUP_SYSTEM_H

