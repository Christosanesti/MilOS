#ifndef OPERATION_MANAGER_H
#define OPERATION_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <cstdint>

/**
 * @brief Operation Status
 */
enum class OperationStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    CANCELLED,
    FAILED
};

/**
 * @brief Operation Type
 */
enum class OperationType {
    FILE_ENCRYPTION,
    FILE_DECRYPTION,
    DISK_ENCRYPTION,
    KEY_DERIVATION
};

/**
 * @brief Operation Information
 */
struct OperationInfo {
    std::string operation_id;
    OperationType type;
    OperationStatus status;
    std::string description;
    size_t bytes_processed;
    size_t total_bytes;
    double progress_percentage;
    std::string error_message;
    std::string result_path;  // For file operations
};

/**
 * @brief Operation Progress Callback
 */
using OperationProgressCallback = std::function<void(const OperationInfo& info)>;

/**
 * @brief Encryption Operation Manager
 * 
 * Manages encryption operations, tracks progress, and supports cancellation.
 */
class EncryptionOperationManager {
public:
    /**
     * @brief Get singleton instance
     * @return Manager instance
     */
    static EncryptionOperationManager& getInstance();

    /**
     * @brief Start new operation
     * @param type Operation type
     * @param description Operation description
     * @param progressCallback Progress callback (optional)
     * @return Operation ID
     */
    std::string startOperation(
        OperationType type,
        const std::string& description,
        OperationProgressCallback progressCallback = nullptr
    );

    /**
     * @brief Update operation progress
     * @param operationId Operation ID
     * @param bytesProcessed Bytes processed
     * @param totalBytes Total bytes
     */
    void updateProgress(
        const std::string& operationId,
        size_t bytesProcessed,
        size_t totalBytes
    );

    /**
     * @brief Complete operation
     * @param operationId Operation ID
     * @param resultPath Result path (optional)
     */
    void completeOperation(
        const std::string& operationId,
        const std::string& resultPath = ""
    );

    /**
     * @brief Fail operation
     * @param operationId Operation ID
     * @param errorMessage Error message
     */
    void failOperation(
        const std::string& operationId,
        const std::string& errorMessage
    );

    /**
     * @brief Cancel operation
     * @param operationId Operation ID
     * @return true if cancellation successful, false otherwise
     */
    bool cancelOperation(const std::string& operationId);

    /**
     * @brief Get operation information
     * @param operationId Operation ID
     * @return Operation information, nullptr if not found
     */
    std::unique_ptr<OperationInfo> getOperationInfo(const std::string& operationId) const;

    /**
     * @brief Get all operations
     * @return Vector of operation information
     */
    std::vector<OperationInfo> getAllOperations() const;

    /**
     * @brief Check if operation is cancelled
     * @param operationId Operation ID
     * @return true if cancelled, false otherwise
     */
    bool isOperationCancelled(const std::string& operationId) const;

    /**
     * @brief Clear completed operations
     */
    void clearCompletedOperations();

private:
    EncryptionOperationManager() = default;
    ~EncryptionOperationManager() = default;
    EncryptionOperationManager(const EncryptionOperationManager&) = delete;
    EncryptionOperationManager& operator=(const EncryptionOperationManager&) = delete;

    struct Operation {
        OperationInfo info;
        std::atomic<bool> cancelled;
        OperationProgressCallback progressCallback;
    };

    std::map<std::string, std::unique_ptr<Operation>> m_operations;
    mutable std::mutex m_mutex;
    std::atomic<uint64_t> m_operationCounter;

    /**
     * @brief Generate unique operation ID
     * @return Operation ID
     */
    std::string generateOperationId();
};

#endif // OPERATION_MANAGER_H

