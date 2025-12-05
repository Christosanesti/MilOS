#include "hash_chain.h"
#include "config_parser.h"
#include "log_storage.h"
#include <milos/logging/logger.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <ctime>

HashChain::HashChain()
    : m_initialized(false)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
    , m_algorithm("sha256")
{
}

HashChain::~HashChain() {
}

bool HashChain::initialize(ConfigParser* configParser, LogStorage* logStorage) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_logStorage = logStorage;

    // Get hash algorithm from configuration
    m_algorithm = configParser->getString("hash_chain.algorithm");
    if (m_algorithm.empty()) {
        m_algorithm = "sha256";
    }

    if (!initializeChain()) {
        LOG_ERROR("Failed to initialize hash chain");
        return false;
    }

    m_initialized = true;
    return true;
}

bool HashChain::initializeChain() {
    if (!m_logStorage) {
        return false;
    }

    // Get last log hash from storage
    std::string lastHash = m_logStorage->getLastLogHash();

    if (lastHash.empty()) {
        // No existing logs, create initialization vector
        std::ostringstream initData;
        initData << "MilOS Audit Service Initialization Vector ";
        initData << std::time(nullptr);
        m_initVector = computeSHA256(initData.str());
        LOG_INFO("Hash chain initialized with new initialization vector");
    } else {
        // Use last hash as starting point
        m_initVector = lastHash;
        LOG_INFO("Hash chain initialized from existing logs");
    }

    // Verify integrity if configured
    bool verifyOnStart = m_configParser->getBool("hash_chain.verify_on_start", true);
    if (verifyOnStart) {
        if (!verifyIntegrity()) {
            LOG_WARNING("Hash chain integrity verification failed!");
            // Continue anyway, but log the warning
        }
    }

    return true;
}

std::string HashChain::generateHash(const std::string& entryData, const std::string& previousHash) {
    // Combine entry data with previous hash
    std::ostringstream hashInput;
    hashInput << entryData;
    if (!previousHash.empty()) {
        hashInput << previousHash;
    } else {
        hashInput << m_initVector;
    }

    return computeSHA256(hashInput.str());
}

bool HashChain::verifyIntegrity() {
    if (!m_logStorage || !m_initialized) {
        return false;
    }

    // Get all log entries in chronological order
    std::map<std::string, std::string> filters;
    std::vector<struct AuditLogEntry> entries = m_logStorage->queryLogEntries(filters, 10000, 0);

    if (entries.empty()) {
        return true;  // No entries to verify
    }

    // Verify hash chain starting from first entry
    std::string expectedPreviousHash = m_initVector;

    for (const auto& entry : entries) {
        // Recompute hash for this entry
        std::ostringstream entryData;
        entryData << entry.log_id << entry.event_type << entry.event_severity 
                  << entry.timestamp << entry.event_data;
        
        std::string computedHash = generateHash(entryData.str(), expectedPreviousHash);

        // Compare with stored hash
        if (computedHash != entry.log_hash) {
            LOG_ERROR(QString("Hash chain integrity violation detected at log_id: %1").arg(QString::fromStdString(entry.log_id)));
            return false;
        }

        // Verify previous hash matches
        if (!entry.previous_log_hash.empty() && entry.previous_log_hash != expectedPreviousHash) {
            LOG_ERROR(QString("Previous hash mismatch at log_id: %1").arg(QString::fromStdString(entry.log_id)));
            return false;
        }

        expectedPreviousHash = entry.log_hash;
    }

    return true;
}

std::string HashChain::getInitializationVector() const {
    return m_initVector;
}

bool HashChain::isHealthy() const {
    if (!m_initialized) {
        return false;
    }

    // Hash chain is healthy if initialized
    return true;
}

std::string HashChain::computeSHA256(const std::string& data) const {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.length());
    SHA256_Final(hash, &sha256);

    // Convert to hex string
    std::ostringstream hexHash;
    hexHash << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hexHash << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }

    return hexHash.str();
}

