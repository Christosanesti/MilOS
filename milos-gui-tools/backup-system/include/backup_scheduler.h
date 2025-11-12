#ifndef BACKUP_SCHEDULER_H
#define BACKUP_SCHEDULER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class BackupService;

/**
 * @brief Backup Schedule Configuration
 */
struct BackupScheduleConfig {
    QString scheduleId;
    QString name;
    QString sourcePath;
    QString destinationId;
    int intervalMinutes;  // 0 = manual only
    QDateTime lastBackup;
    bool enabled;
};

/**
 * @brief Backup Scheduler
 * 
 * Manages automatic backup scheduling.
 */
class BackupScheduler : public QObject {
    Q_OBJECT

public:
    explicit BackupScheduler(QObject* parent = nullptr);
    ~BackupScheduler();

    /**
     * @brief Initialize scheduler
     * @param backupService Backup service instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BackupService* backupService);

    /**
     * @brief Start scheduler
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop scheduler
     */
    void stop();

    /**
     * @brief Add backup schedule
     * @param config Schedule configuration
     * @return true if add successful, false otherwise
     */
    bool addSchedule(const BackupScheduleConfig& config);

    /**
     * @brief Remove backup schedule
     * @param scheduleId Schedule ID
     * @return true if remove successful, false otherwise
     */
    bool removeSchedule(const QString& scheduleId);

    /**
     * @brief Get all schedules
     * @return Vector of schedule configurations
     */
    std::vector<BackupScheduleConfig> getSchedules() const;

Q_SIGNALS:
    /**
     * @brief Emitted when scheduled backup should run
     */
    void scheduledBackup(const QString& scheduleId);

private slots:
    /**
     * @brief Check schedules and trigger backups if needed
     */
    void checkSchedules();

private:
    bool m_initialized;
    bool m_running;
    BackupService* m_backupService;
    QTimer* m_checkTimer;
    std::vector<BackupScheduleConfig> m_schedules;
};

#endif // BACKUP_SCHEDULER_H

