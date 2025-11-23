#ifndef VERIFICATION_SCHEDULER_H
#define VERIFICATION_SCHEDULER_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <ctime>

class IntegrityVerifier;
class BaselineManager;
class AuditLogger;

/**
 * @brief Verification Schedule
 */
struct VerificationSchedule {
    std::string schedule_id;
    std::string name;
    std::string description;
    std::vector<std::string> file_paths;  // Empty means all monitored files
    std::string cron_expression;  // Simple cron-like: "0 */6 * * *" (every 6 hours)
    bool enabled;
    std::string created_at;
    std::string last_run_at;
    std::string next_run_at;
};

/**
 * @brief Verification History Entry
 */
struct VerificationHistoryEntry {
    std::string entry_id;
    std::string schedule_id;
    std::string file_path;
    bool is_valid;
    std::string baseline_id;
    std::string error_message;
    std::string verified_at;
    std::string duration_ms;  // Verification duration in milliseconds
};

/**
 * @brief Verification Scheduler
 * 
 * Manages scheduled integrity verifications and tracks verification history.
 */
class VerificationScheduler {
public:
    VerificationScheduler();
    ~VerificationScheduler();

    /**
     * @brief Initialize verification scheduler
     * @param integrityVerifier Integrity verifier instance
     * @param baselineManager Baseline manager instance
     * @param auditLogger Audit logger instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(IntegrityVerifier* integrityVerifier,
                    BaselineManager* baselineManager,
                    AuditLogger* auditLogger);

    /**
     * @brief Start the scheduler
     */
    void start();

    /**
     * @brief Stop the scheduler
     */
    void stop();

    /**
     * @brief Check if scheduler is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Create a new verification schedule
     * @param schedule Schedule configuration
     * @return Schedule ID if successful, empty string otherwise
     */
    std::string createSchedule(const VerificationSchedule& schedule);

    /**
     * @brief Update an existing schedule
     * @param scheduleId Schedule ID
     * @param schedule Updated schedule configuration
     * @return true if update successful, false otherwise
     */
    bool updateSchedule(const std::string& scheduleId, const VerificationSchedule& schedule);

    /**
     * @brief Delete a schedule
     * @param scheduleId Schedule ID
     * @return true if deletion successful, false otherwise
     */
    bool deleteSchedule(const std::string& scheduleId);

    /**
     * @brief Get a schedule
     * @param scheduleId Schedule ID
     * @return Schedule, or empty if not found
     */
    VerificationSchedule getSchedule(const std::string& scheduleId) const;

    /**
     * @brief Get all schedules
     * @return List of schedules
     */
    std::vector<VerificationSchedule> getAllSchedules() const;

    /**
     * @brief Enable/disable a schedule
     * @param scheduleId Schedule ID
     * @param enabled Enable flag
     * @return true if update successful, false otherwise
     */
    bool setScheduleEnabled(const std::string& scheduleId, bool enabled);

    /**
     * @brief Get verification history
     * @param scheduleId Schedule ID (empty for all)
     * @param filePath File path filter (empty for all)
     * @param limit Maximum number of entries to return
     * @return List of verification history entries
     */
    std::vector<VerificationHistoryEntry> getHistory(const std::string& scheduleId = "",
                                                     const std::string& filePath = "",
                                                     int limit = 100) const;

    /**
     * @brief Get verification history entry
     * @param entryId Entry ID
     * @return History entry, or empty if not found
     */
    VerificationHistoryEntry getHistoryEntry(const std::string& entryId) const;

    /**
     * @brief Clear verification history
     * @param scheduleId Schedule ID (empty for all)
     * @param olderThanDays Delete entries older than N days (0 for all)
     * @return Number of entries deleted
     */
    int clearHistory(const std::string& scheduleId = "", int olderThanDays = 0);

private:
    bool m_initialized;
    std::atomic<bool> m_running;
    IntegrityVerifier* m_integrityVerifier;
    BaselineManager* m_baselineManager;
    AuditLogger* m_auditLogger;
    std::map<std::string, VerificationSchedule> m_schedules;
    std::vector<VerificationHistoryEntry> m_history;
    std::mutex m_schedulesMutex;
    std::mutex m_historyMutex;
    std::thread m_schedulerThread;

    /**
     * @brief Scheduler thread function
     */
    void schedulerThreadFunction();

    /**
     * @brief Parse cron expression and calculate next run time
     * @param cronExpression Cron expression (simplified: "minute hour day month weekday")
     * @return Next run time as time_t
     */
    std::time_t calculateNextRunTime(const std::string& cronExpression) const;

    /**
     * @brief Execute a scheduled verification
     * @param schedule Schedule to execute
     */
    void executeSchedule(const VerificationSchedule& schedule);

    /**
     * @brief Generate schedule ID
     * @return Schedule ID
     */
    std::string generateScheduleId() const;

    /**
     * @brief Generate history entry ID
     * @return Entry ID
     */
    std::string generateEntryId() const;
};

#endif // VERIFICATION_SCHEDULER_H

