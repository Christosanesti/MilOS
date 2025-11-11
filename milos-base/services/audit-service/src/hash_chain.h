#ifndef HASH_CHAIN_H
#define HASH_CHAIN_H

#include <string>
#include <memory>

class ConfigParser;
class LogStorage;

/**
 * @brief Hash Chain Manager
 * 
 * Manages cryptographic hash chains for tamper-proof audit logging.
 * Each log entry includes a hash of the previous entry, creating
 * an immutable chain that enables tamper detection.
 */
class HashChain {
public:
    HashChain();
    ~HashChain();

    /**
     * @brief Initialize hash chain
     * @param configParser Configuration parser instance
     * @param logStorage Log storage instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser, LogStorage* logStorage);

    /**
     * @brief Generate hash for log entry
     * @param entryData Log entry data (JSON string)
     * @param previousHash Hash of previous log entry
     * @return SHA-256 hash of log entry
     */
    std::string generateHash(const std::string& entryData, const std::string& previousHash);

    /**
     * @brief Verify hash chain integrity
     * @return true if integrity verified, false if tampering detected
     */
    bool verifyIntegrity();

    /**
     * @brief Get initialization vector (for first log entry)
     * @return Initialization vector hash
     */
    std::string getInitializationVector() const;

    /**
     * @brief Check if hash chain is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    LogStorage* m_logStorage;
    std::string m_algorithm;  // sha256, sha512
    std::string m_initVector;

    /**
     * @brief Initialize hash chain (load last hash or create init vector)
     */
    bool initializeChain();

    /**
     * @brief Compute SHA-256 hash
     * @param data Data to hash
     * @return SHA-256 hash (hex string)
     */
    std::string computeSHA256(const std::string& data) const;
};

#endif // HASH_CHAIN_H

