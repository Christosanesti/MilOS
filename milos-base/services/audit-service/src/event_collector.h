#ifndef EVENT_COLLECTOR_H
#define EVENT_COLLECTOR_H

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>

class ConfigParser;
class LogStorage;
class HashChain;

/**
 * @brief Event Collector
 * 
 * Collects security events from D-Bus, local sockets, and Linux audit framework.
 * Handles event validation, queuing, and batch storage.
 */
class EventCollector {
public:
    EventCollector();
    ~EventCollector();

    /**
     * @brief Initialize event collector
     * @param configParser Configuration parser instance
     * @param logStorage Log storage instance
     * @param hashChain Hash chain instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        LogStorage* logStorage,
        HashChain* hashChain
    );

    /**
     * @brief Start event collector
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop event collector
     */
    void stop();

    /**
     * @brief Check if event collector is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Collect event from D-Bus
     * @param eventData Event data (JSON string)
     * @return true if collected successfully, false otherwise
     */
    bool collectEvent(const std::string& eventData);

    /**
     * @brief Collect event from local socket
     * @param eventData Event data (JSON string)
     * @return true if collected successfully, false otherwise
     */
    bool collectEventFromSocket(const std::string& eventData);

    /**
     * @brief Check if event collector is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

private:
    bool m_running;
    bool m_initialized;
    
    ConfigParser* m_configParser;
    LogStorage* m_logStorage;
    HashChain* m_hashChain;

    // Event queue for batch processing
    std::queue<std::string> m_eventQueue;
    std::mutex m_queueMutex;
    std::thread* m_processingThread;
    bool m_processingRunning;

    // Configuration
    size_t m_maxQueueSize;
    size_t m_batchSize;
    int m_flushInterval;

    /**
     * @brief Process event queue (batch insert)
     */
    void processEventQueue();

    /**
     * @brief Validate event data
     * @param eventData Event data (JSON string)
     * @return true if valid, false otherwise
     */
    bool validateEvent(const std::string& eventData);

    /**
     * @brief Store event in database with hash chain
     * @param eventData Event data (JSON string)
     * @return true if stored successfully, false otherwise
     */
    bool storeEvent(const std::string& eventData);
};

#endif // EVENT_COLLECTOR_H

