#include "file_monitor.h"
#include "change_detector.h"
#include <iostream>
#include <filesystem>

#ifdef HAVE_INOTIFY
#include <sys/inotify.h>
#include <unistd.h>
#include <poll.h>
#endif

FileMonitor::FileMonitor()
    : m_initialized(false)
    , m_monitoring(false)
    , m_running(false)
    , m_changeDetector(nullptr)
#ifdef HAVE_INOTIFY
    , m_inotifyFd(-1)
#endif
{
}

FileMonitor::~FileMonitor() {
    stop();
}

bool FileMonitor::initialize(ChangeDetector* changeDetector) {
    if (m_initialized) {
        return true;
    }

    if (!changeDetector) {
        return false;
    }

    m_changeDetector = changeDetector;

#ifdef HAVE_INOTIFY
    m_inotifyFd = inotify_init1(IN_NONBLOCK);
    if (m_inotifyFd < 0) {
        std::cerr << "Failed to initialize inotify" << std::endl;
        return false;
    }
#endif

    m_initialized = true;
    return true;
}

bool FileMonitor::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_monitoring) {
        return true;
    }

    m_running = true;
    m_monitoring = true;

    // Start monitor thread
    m_monitorThread = std::make_unique<std::thread>(&FileMonitor::monitorLoop, this);

    return true;
}

void FileMonitor::stop() {
    if (!m_monitoring) {
        return;
    }

    m_running = false;
    m_monitoring = false;

    if (m_monitorThread && m_monitorThread->joinable()) {
        m_monitorThread->join();
    }

#ifdef HAVE_INOTIFY
    // Remove all watch descriptors
    for (int wd : m_watchDescriptors) {
        inotify_rm_watch(m_inotifyFd, wd);
    }
    m_watchDescriptors.clear();

    if (m_inotifyFd >= 0) {
        close(m_inotifyFd);
        m_inotifyFd = -1;
    }
#endif
}

void FileMonitor::addDirectory(const std::string& directoryPath) {
    if (!std::filesystem::exists(directoryPath)) {
        std::cerr << "Directory does not exist: " << directoryPath << std::endl;
        return;
    }

    // Check if already monitoring
    for (const auto& dir : m_monitoredDirectories) {
        if (dir == directoryPath) {
            return;
        }
    }

    m_monitoredDirectories.push_back(directoryPath);

#ifdef HAVE_INOTIFY
    if (m_inotifyFd >= 0) {
        int wd = inotify_add_watch(m_inotifyFd, directoryPath.c_str(),
                                   IN_MODIFY | IN_DELETE | IN_CREATE | IN_ATTRIB);
        if (wd >= 0) {
            m_watchDescriptors.push_back(wd);
        }
    }
#endif
}

void FileMonitor::removeDirectory(const std::string& directoryPath) {
    auto it = std::find(m_monitoredDirectories.begin(), m_monitoredDirectories.end(), directoryPath);
    if (it != m_monitoredDirectories.end()) {
        m_monitoredDirectories.erase(it);
    }

#ifdef HAVE_INOTIFY
    // Find and remove watch descriptor
    for (size_t i = 0; i < m_watchDescriptors.size(); i++) {
        // Note: We'd need to track directory->wd mapping for proper removal
        // For now, this is simplified
    }
#endif
}

void FileMonitor::reloadConfiguration() {
    // TODO: Reload monitored directories from configuration
}

void FileMonitor::monitorLoop() {
#ifdef HAVE_INOTIFY
    struct pollfd pfd;
    pfd.fd = m_inotifyFd;
    pfd.events = POLLIN;

    char buffer[4096];

    while (m_running) {
        int pollResult = poll(&pfd, 1, 1000);  // 1 second timeout

        if (pollResult > 0 && (pfd.revents & POLLIN)) {
            ssize_t length = read(m_inotifyFd, buffer, sizeof(buffer));
            if (length < 0) {
                continue;
            }

            struct inotify_event* event = reinterpret_cast<struct inotify_event*>(buffer);
            size_t i = 0;

            while (i < length) {
                if (event->len > 0) {
                    std::string filePath = std::string(event->name);
                    processFileEvent(filePath);
                }

                i += sizeof(struct inotify_event) + event->len;
                event = reinterpret_cast<struct inotify_event*>(buffer + i);
            }
        }
    }
#else
    // Fallback: Polling-based monitoring
    while (m_running) {
        for (const auto& dir : m_monitoredDirectories) {
            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
                    if (entry.is_regular_file()) {
                        processFileEvent(entry.path().string());
                    }
                }
            } catch (const std::exception& e) {
                // Ignore errors
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
#endif
}

void FileMonitor::processFileEvent(const std::string& filePath) {
    if (m_changeDetector) {
        m_changeDetector->detectChange(filePath);
    }
}

