#include "verification_scheduler.h"
#include "integrity_verifier.h"
#include "baseline_manager.h"
#include "audit_logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>

VerificationScheduler::VerificationScheduler()
    : m_initialized(false)
    , m_running(false)
    , m_integrityVerifier(nullptr)
    , m_baselineManager(nullptr)
    , m_auditLogger(nullptr)
{
}

VerificationScheduler::~VerificationScheduler() {
    stop();
}

bool VerificationScheduler::initialize(IntegrityVerifier* integrityVerifier,
                                       BaselineManager* baselineManager,
                                       AuditLogger* auditLogger) {
    if (m_initialized) {
        return true;
    }

    if (!integrityVerifier || !baselineManager || !auditLogger) {
        std::cerr << "VerificationScheduler: Missing dependencies for initialization." << std::endl;
        return false;
    }

    m_integrityVerifier = integrityVerifier;
    m_baselineManager = baselineManager;
    m_auditLogger = auditLogger;
    m_initialized = true;
    return true;
}

void VerificationScheduler::start() {
    if (m_running) {
        return;
    }

    m_running = true;
    m_schedulerThread = std::thread(&VerificationScheduler::schedulerThreadFunction, this);
    std::cout << "VerificationScheduler: Started scheduler thread" << std::endl;
}

void VerificationScheduler::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
    if (m_schedulerThread.joinable()) {
        m_schedulerThread.join();
    }
    std::cout << "VerificationScheduler: Stopped scheduler thread" << std::endl;
}

std::string VerificationScheduler::createSchedule(const VerificationSchedule& schedule) {
    if (!m_initialized) {
        return "";
    }

    std::lock_guard<std::mutex> lock(m_schedulesMutex);

    VerificationSchedule newSchedule = schedule;
    if (newSchedule.schedule_id.empty()) {
        newSchedule.schedule_id = generateScheduleId();
    }

    // Check if ID already exists
    if (m_schedules.find(newSchedule.schedule_id) != m_schedules.end()) {
        std::cerr << "VerificationScheduler: Schedule ID already exists: " << newSchedule.schedule_id << std::endl;
        return "";
    }

    // Calculate next run time
    if (newSchedule.enabled && !newSchedule.cron_expression.empty()) {
        newSchedule.next_run_at = std::to_string(calculateNextRunTime(newSchedule.cron_expression));
    }

    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    newSchedule.created_at = timeStr;

    m_schedules[newSchedule.schedule_id] = newSchedule;

    m_auditLogger->logEvent("verification_schedule_created",
                           "Verification schedule created: " + newSchedule.name,
                           {{"schedule_id", newSchedule.schedule_id}});

    return newSchedule.schedule_id;
}

bool VerificationScheduler::updateSchedule(const std::string& scheduleId, const VerificationSchedule& schedule) {
    if (!m_initialized) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_schedulesMutex);

    auto it = m_schedules.find(scheduleId);
    if (it == m_schedules.end()) {
        return false;
    }

    VerificationSchedule updatedSchedule = schedule;
    updatedSchedule.schedule_id = scheduleId;
    updatedSchedule.created_at = it->second.created_at;  // Preserve creation time

    // Recalculate next run time if enabled and cron expression changed
    if (updatedSchedule.enabled && !updatedSchedule.cron_expression.empty()) {
        updatedSchedule.next_run_at = std::to_string(calculateNextRunTime(updatedSchedule.cron_expression));
    }

    m_schedules[scheduleId] = updatedSchedule;

    m_auditLogger->logEvent("verification_schedule_updated",
                           "Verification schedule updated: " + updatedSchedule.name,
                           {{"schedule_id", scheduleId}});

    return true;
}

bool VerificationScheduler::deleteSchedule(const std::string& scheduleId) {
    if (!m_initialized) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_schedulesMutex);

    auto it = m_schedules.find(scheduleId);
    if (it == m_schedules.end()) {
        return false;
    }

    std::string name = it->second.name;
    m_schedules.erase(it);

    m_auditLogger->logEvent("verification_schedule_deleted",
                           "Verification schedule deleted: " + name,
                           {{"schedule_id", scheduleId}});

    return true;
}

VerificationSchedule VerificationScheduler::getSchedule(const std::string& scheduleId) const {
    std::lock_guard<std::mutex> lock(m_schedulesMutex);
    auto it = m_schedules.find(scheduleId);
    if (it != m_schedules.end()) {
        return it->second;
    }
    return VerificationSchedule();  // Return empty if not found
}

std::vector<VerificationSchedule> VerificationScheduler::getAllSchedules() const {
    std::lock_guard<std::mutex> lock(m_schedulesMutex);
    std::vector<VerificationSchedule> schedules;
    for (const auto& [id, schedule] : m_schedules) {
        schedules.push_back(schedule);
    }
    return schedules;
}

bool VerificationScheduler::setScheduleEnabled(const std::string& scheduleId, bool enabled) {
    if (!m_initialized) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_schedulesMutex);

    auto it = m_schedules.find(scheduleId);
    if (it == m_schedules.end()) {
        return false;
    }

    it->second.enabled = enabled;
    if (enabled && !it->second.cron_expression.empty()) {
        it->second.next_run_at = std::to_string(calculateNextRunTime(it->second.cron_expression));
    } else {
        it->second.next_run_at = "";
    }

    m_auditLogger->logEvent(enabled ? "verification_schedule_enabled" : "verification_schedule_disabled",
                           "Verification schedule " + std::string(enabled ? "enabled" : "disabled"),
                           {{"schedule_id", scheduleId}});

    return true;
}

std::vector<VerificationHistoryEntry> VerificationScheduler::getHistory(const std::string& scheduleId,
                                                                          const std::string& filePath,
                                                                          int limit) const {
    std::lock_guard<std::mutex> lock(m_historyMutex);
    std::vector<VerificationHistoryEntry> filteredHistory;

    for (const auto& entry : m_history) {
        // Apply filters
        if (!scheduleId.empty() && entry.schedule_id != scheduleId) {
            continue;
        }
        if (!filePath.empty() && entry.file_path != filePath) {
            continue;
        }
        filteredHistory.push_back(entry);
    }

    // Sort by verified_at (newest first)
    std::sort(filteredHistory.begin(), filteredHistory.end(),
              [](const VerificationHistoryEntry& a, const VerificationHistoryEntry& b) {
                  return a.verified_at > b.verified_at;
              });

    // Apply limit
    if (limit > 0 && static_cast<int>(filteredHistory.size()) > limit) {
        filteredHistory.resize(limit);
    }

    return filteredHistory;
}

VerificationHistoryEntry VerificationScheduler::getHistoryEntry(const std::string& entryId) const {
    std::lock_guard<std::mutex> lock(m_historyMutex);
    for (const auto& entry : m_history) {
        if (entry.entry_id == entryId) {
            return entry;
        }
    }
    return VerificationHistoryEntry();  // Return empty if not found
}

int VerificationScheduler::clearHistory(const std::string& scheduleId, int olderThanDays) {
    if (!m_initialized) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_historyMutex);

    int deletedCount = 0;
    auto now = std::time(nullptr);

    m_history.erase(
        std::remove_if(m_history.begin(), m_history.end(),
                       [&scheduleId, olderThanDays, now](const VerificationHistoryEntry& entry) {
                           // Filter by schedule ID
                           if (!scheduleId.empty() && entry.schedule_id != scheduleId) {
                               return false;
                           }

                           // Filter by age
                           if (olderThanDays > 0) {
                               // Parse verified_at (format: "YYYY-MM-DD HH:MM:SS")
                               std::tm tm = {};
                               std::istringstream ss(entry.verified_at);
                               ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                               if (ss.fail()) {
                                   return false;  // Skip invalid dates
                               }
                               std::time_t entryTime = std::mktime(&tm);
                               double daysDiff = std::difftime(now, entryTime) / (60 * 60 * 24);
                               if (daysDiff < olderThanDays) {
                                   return false;  // Not old enough
                               }
                           }

                           deletedCount++;
                           return true;
                       }),
        m_history.end()
    );

    if (deletedCount > 0) {
        m_auditLogger->logEvent("verification_history_cleared",
                               "Cleared " + std::to_string(deletedCount) + " verification history entries",
                               {{"schedule_id", scheduleId}, {"older_than_days", std::to_string(olderThanDays)}});
    }

    return deletedCount;
}

void VerificationScheduler::schedulerThreadFunction() {
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::minutes(1));  // Check every minute

        if (!m_initialized) {
            continue;
        }

        std::time_t now = std::time(nullptr);
        std::lock_guard<std::mutex> lock(m_schedulesMutex);

        for (auto& [scheduleId, schedule] : m_schedules) {
            if (!schedule.enabled || schedule.cron_expression.empty()) {
                continue;
            }

            // Check if it's time to run
            std::time_t nextRun = std::stoll(schedule.next_run_at);
            if (now >= nextRun) {
                // Execute schedule
                executeSchedule(schedule);

                // Update next run time
                schedule.last_run_at = std::to_string(now);
                schedule.next_run_at = std::to_string(calculateNextRunTime(schedule.cron_expression));
            }
        }
    }
}

std::time_t VerificationScheduler::calculateNextRunTime(const std::string& cronExpression) const {
    // Simplified cron parser: "minute hour day month weekday"
    // For now, support simple patterns like "0 */6 * * *" (every 6 hours)
    // or "0 0 * * *" (daily at midnight)
    
    std::istringstream iss(cronExpression);
    std::string minute, hour, day, month, weekday;
    iss >> minute >> hour >> day >> month >> weekday;

    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);

    // For simplicity, if hour contains "*/", parse interval
    if (hour.find("*/") == 0) {
        int interval = std::stoi(hour.substr(2));
        int currentHour = tm->tm_hour;
        int nextHour = ((currentHour / interval) + 1) * interval;
        if (nextHour >= 24) {
            nextHour = 0;
            tm->tm_mday++;  // Next day
        }
        tm->tm_hour = nextHour;
        tm->tm_min = 0;
        tm->tm_sec = 0;
    } else if (hour == "*" && minute == "0") {
        // Every hour at minute 0
        tm->tm_hour++;
        if (tm->tm_hour >= 24) {
            tm->tm_hour = 0;
            tm->tm_mday++;
        }
        tm->tm_min = 0;
        tm->tm_sec = 0;
    } else {
        // Default: next hour
        tm->tm_hour++;
        if (tm->tm_hour >= 24) {
            tm->tm_hour = 0;
            tm->tm_mday++;
        }
        tm->tm_min = 0;
        tm->tm_sec = 0;
    }

    return std::mktime(tm);
}

void VerificationScheduler::executeSchedule(const VerificationSchedule& schedule) {
    if (!m_integrityVerifier) {
        return;
    }

    std::cout << "VerificationScheduler: Executing schedule: " << schedule.name << std::endl;

    // Get files to verify
    std::vector<std::string> filesToVerify;
    if (schedule.file_paths.empty()) {
        // Get all monitored files from BaselineManager
        if (m_baselineManager) {
            filesToVerify = m_baselineManager->getAllMonitoredFiles();
        }
    } else {
        filesToVerify = schedule.file_paths;
    }

    // Verify each file
    for (const auto& filePath : filesToVerify) {
        auto startTime = std::chrono::steady_clock::now();

        auto result = m_integrityVerifier->verifyIntegrity(filePath);

        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        // Create history entry
        VerificationHistoryEntry entry;
        entry.entry_id = generateEntryId();
        entry.schedule_id = schedule.schedule_id;
        entry.file_path = result.file_path;
        entry.is_valid = result.is_valid;
        entry.baseline_id = result.baseline_id;
        entry.error_message = result.error_message;
        entry.duration_ms = std::to_string(duration);

        char timeStr[64];
        std::time_t now = std::time(nullptr);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        entry.verified_at = timeStr;

        // Store history
        {
            std::lock_guard<std::mutex> lock(m_historyMutex);
            m_history.push_back(entry);
            // Limit history size (keep last 10000 entries)
            if (m_history.size() > 10000) {
                m_history.erase(m_history.begin(), m_history.begin() + (m_history.size() - 10000));
            }
        }

        // Log verification
        m_auditLogger->logEvent(result.is_valid ? "verification_passed" : "verification_failed",
                               "Scheduled verification " + std::string(result.is_valid ? "passed" : "failed") + " for " + filePath,
                               {{"schedule_id", schedule.schedule_id},
                                {"file_path", filePath},
                                {"is_valid", result.is_valid ? "true" : "false"}});
    }
}

std::string VerificationScheduler::generateScheduleId() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    ss << "schedule-";
    for (int i = 0; i < 16; i++) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

std::string VerificationScheduler::generateEntryId() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    ss << "entry-";
    for (int i = 0; i < 16; i++) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

