#include "event_collector.h"
#include "config_parser.h"
#include "log_storage.h"
#include "hash_chain.h"
#include "log_storage.h"
#include <milos/logging/logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QUuid>
#include <chrono>
#include <thread>

EventCollector::EventCollector()
    : m_running(false)
    , m_initialized(false)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
    , m_hashChain(nullptr)
    , m_processingThread(nullptr)
    , m_processingRunning(false)
    , m_maxQueueSize(10000)
    , m_batchSize(100)
    , m_flushInterval(5)
{
}

EventCollector::~EventCollector() {
    stop();
}

bool EventCollector::initialize(
    ConfigParser* configParser,
    LogStorage* logStorage,
    HashChain* hashChain
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_logStorage = logStorage;
    m_hashChain = hashChain;

    // Get configuration
    m_maxQueueSize = static_cast<size_t>(configParser->getInt("event_collection.max_queue_size", 10000));
    m_batchSize = static_cast<size_t>(configParser->getInt("event_collection.batch_size", 100));
    m_flushInterval = configParser->getInt("event_collection.flush_interval", 5);

    m_initialized = true;
    return true;
}

bool EventCollector::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running) {
        return true;
    }

    // Start processing thread
    m_processingRunning = true;
    m_processingThread = new std::thread(&EventCollector::processEventQueue, this);

    m_running = true;
    LOG_INFO("Event collector started");
    return true;
}

void EventCollector::stop() {
    if (!m_running) {
        return;
    }

    // Stop processing thread
    m_processingRunning = false;
    if (m_processingThread) {
        m_processingThread->join();
        delete m_processingThread;
        m_processingThread = nullptr;
    }

    // Process remaining events in queue
    while (!m_eventQueue.empty()) {
        std::string eventData = m_eventQueue.front();
        m_eventQueue.pop();
        storeEvent(eventData);
    }

    m_running = false;
    LOG_INFO("Event collector stopped");
}

bool EventCollector::collectEvent(const std::string& eventData) {
    if (!m_running) {
        return false;
    }

    // Validate event
    if (!validateEvent(eventData)) {
        LOG_ERROR("Invalid event data received");
        return false;
    }

    // Add to queue
    std::lock_guard<std::mutex> lock(m_queueMutex);
    
    if (m_eventQueue.size() >= m_maxQueueSize) {
        LOG_WARNING("Event queue full, dropping event");
        return false;
    }

    m_eventQueue.push(eventData);
    return true;
}

bool EventCollector::collectEventFromSocket(const std::string& eventData) {
    // Same as collectEvent, but from socket interface
    return collectEvent(eventData);
}

bool EventCollector::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check queue size (should not be too large)
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_queueMutex));
    return m_eventQueue.size() < m_maxQueueSize;
}

void EventCollector::processEventQueue() {
    while (m_processingRunning) {
        std::vector<std::string> batch;

        // Collect batch of events
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            size_t batchCount = std::min(m_batchSize, m_eventQueue.size());
            for (size_t i = 0; i < batchCount; ++i) {
                if (!m_eventQueue.empty()) {
                    batch.push_back(m_eventQueue.front());
                    m_eventQueue.pop();
                }
            }
        }

        // Store batch
        for (const auto& eventData : batch) {
            storeEvent(eventData);
        }

        // Sleep for flush interval
        std::this_thread::sleep_for(std::chrono::seconds(m_flushInterval));
    }
}

bool EventCollector::validateEvent(const std::string& eventData) {
    // Parse JSON to validate structure
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(eventData).toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    if (!doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();
    
    // Required fields
    if (!obj.contains("event_type") || !obj.contains("event_severity") || 
        !obj.contains("application_name")) {
        return false;
    }

    return true;
}

bool EventCollector::storeEvent(const std::string& eventData) {
    if (!m_logStorage || !m_hashChain) {
        return false;
    }

    // Parse event data
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(eventData).toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject obj = doc.object();

    // Create audit log entry
    AuditLogEntry entry;
    entry.log_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    entry.event_type = obj["event_type"].toString().toStdString();
    entry.event_severity = obj["event_severity"].toString().toStdString();
    entry.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
    
    if (obj.contains("user_id")) {
        entry.user_id = obj["user_id"].toString().toStdString();
    }
    
    entry.application_name = obj["application_name"].toString().toStdString();
    
    if (obj.contains("event_description")) {
        entry.event_description = obj["event_description"].toString().toStdString();
    }
    
    entry.event_data = eventData;  // Store full JSON
    
    if (obj.contains("ip_address")) {
        entry.ip_address = obj["ip_address"].toString().toStdString();
    }
    
    if (obj.contains("file_paths")) {
        QJsonArray paths = obj["file_paths"].toArray();
        for (const auto& path : paths) {
            entry.file_paths.push_back(path.toString().toStdString());
        }
    }
    
    if (obj.contains("policy_id")) {
        entry.policy_id = obj["policy_id"].toString().toStdString();
    }
    
    if (obj.contains("action_taken")) {
        entry.action_taken = obj["action_taken"].toString().toStdString();
    }

    // Get previous hash for hash chain
    std::string previousHash = m_logStorage->getLastLogHash();
    if (previousHash.empty()) {
        previousHash = m_hashChain->getInitializationVector();
    }

    // Generate hash for this entry
    std::ostringstream hashInput;
    hashInput << entry.log_id << entry.event_type << entry.event_severity 
              << entry.timestamp << entry.event_data;
    entry.log_hash = m_hashChain->generateHash(hashInput.str(), previousHash);
    entry.previous_log_hash = previousHash;

    // Store in database
    return m_logStorage->storeLogEntry(entry);
}

