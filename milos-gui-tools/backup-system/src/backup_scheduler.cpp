#include "backup_scheduler.h"
#include "backup_service.h"
#include <QDateTime>
#include <iostream>

BackupScheduler::BackupScheduler(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
    , m_backupService(nullptr)
    , m_checkTimer(nullptr)
{
}

BackupScheduler::~BackupScheduler() {
    stop();
}

bool BackupScheduler::initialize(BackupService* backupService) {
    if (m_initialized) {
        return true;
    }

    m_backupService = backupService;
    if (!m_backupService) {
        std::cerr << "Backup service is null" << std::endl;
        return false;
    }

    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &BackupScheduler::checkSchedules);
    m_checkTimer->setInterval(60000);  // Check every minute

    m_initialized = true;
    return true;
}

bool BackupScheduler::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running) {
        return true;
    }

    if (m_checkTimer) {
        m_checkTimer->start();
    }

    m_running = true;
    return true;
}

void BackupScheduler::stop() {
    if (!m_running) {
        return;
    }

    if (m_checkTimer) {
        m_checkTimer->stop();
    }

    m_running = false;
}

bool BackupScheduler::addSchedule(const BackupScheduleConfig& config) {
    // Check if schedule already exists
    for (const auto& schedule : m_schedules) {
        if (schedule.scheduleId == config.scheduleId) {
            return false;
        }
    }

    m_schedules.push_back(config);
    return true;
}

bool BackupScheduler::removeSchedule(const QString& scheduleId) {
    auto it = std::remove_if(m_schedules.begin(), m_schedules.end(),
        [&scheduleId](const BackupScheduleConfig& config) {
            return config.scheduleId == scheduleId;
        });
    
    if (it != m_schedules.end()) {
        m_schedules.erase(it, m_schedules.end());
        return true;
    }
    
    return false;
}

std::vector<BackupScheduleConfig> BackupScheduler::getSchedules() const {
    return m_schedules;
}

void BackupScheduler::checkSchedules() {
    QDateTime now = QDateTime::currentDateTime();

    for (const auto& schedule : m_schedules) {
        if (!schedule.enabled || schedule.intervalMinutes == 0) {
            continue;
        }

        // Check if backup is due
        QDateTime nextBackup = schedule.lastBackup.addSecs(schedule.intervalMinutes * 60);
        if (now >= nextBackup || schedule.lastBackup.isNull()) {
            emit scheduledBackup(schedule.scheduleId);
        }
    }
}

