#ifndef FILE_MONITOR_H
#define FILE_MONITOR_H

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

class ChangeDetector;

/**
 * @brief File Monitor
 * 
 * Monitors files for changes using inotify (Linux) or polling.
 */
class FileMonitor {
public:
    FileMonitor();
    ~FileMonitor();

    /**
     * @brief Initialize file monitor
     * @param changeDetector Change detector instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ChangeDetector* changeDetector);

    /**
     * @brief Start monitoring
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop monitoring
     */
    void stop();

    /**
     * @brief Add directory to monitor
     * @param directoryPath Directory path
     */
    void addDirectory(const std::string& directoryPath);

    /**
     * @brief Remove directory from monitoring
     * @param directoryPath Directory path
     */
    void removeDirectory(const std::string& directoryPath);

    /**
     * @brief Reload configuration
     */
    void reloadConfiguration();

    /**
     * @brief Check if monitoring is active
     * @return true if active, false otherwise
     */
    bool isMonitoring() const { return m_monitoring; }

private:
    bool m_initialized;
    bool m_monitoring;
    std::atomic<bool> m_running;
    ChangeDetector* m_changeDetector;
    std::vector<std::string> m_monitoredDirectories;
    std::unique_ptr<std::thread> m_monitorThread;

#ifdef HAVE_INOTIFY
    int m_inotifyFd;
    std::vector<int> m_watchDescriptors;
#endif

    /**
     * @brief Monitor thread function
     */
    void monitorLoop();

    /**
     * @brief Process file event
     * @param filePath File path
     */
    void processFileEvent(const std::string& filePath);
};

#endif // FILE_MONITOR_H

