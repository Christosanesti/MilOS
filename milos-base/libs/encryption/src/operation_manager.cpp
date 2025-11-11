#include "milos/encryption/operation_manager.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

EncryptionOperationManager& EncryptionOperationManager::getInstance() {
    static EncryptionOperationManager instance;
    return instance;
}

std::string EncryptionOperationManager::startOperation(
    OperationType type,
    const std::string& description,
    OperationProgressCallback progressCallback
) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string operationId = generateOperationId();

    auto operation = std::make_unique<Operation>();
    operation->info.operation_id = operationId;
    operation->info.type = type;
    operation->info.status = OperationStatus::PENDING;
    operation->info.description = description;
    operation->info.bytes_processed = 0;
    operation->info.total_bytes = 0;
    operation->info.progress_percentage = 0.0;
    operation->info.error_message = "";
    operation->info.result_path = "";
    operation->cancelled = false;
    operation->progressCallback = progressCallback;

    m_operations[operationId] = std::move(operation);

    return operationId;
}

void EncryptionOperationManager::updateProgress(
    const std::string& operationId,
    size_t bytesProcessed,
    size_t totalBytes
) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return;
    }

    Operation* operation = it->second.get();
    operation->info.bytes_processed = bytesProcessed;
    operation->info.total_bytes = totalBytes;
    operation->info.progress_percentage = totalBytes > 0 ?
        (static_cast<double>(bytesProcessed) / totalBytes) * 100.0 : 0.0;
    operation->info.status = OperationStatus::RUNNING;

    // Call progress callback
    if (operation->progressCallback) {
        operation->progressCallback(operation->info);
    }
}

void EncryptionOperationManager::completeOperation(
    const std::string& operationId,
    const std::string& resultPath
) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return;
    }

    Operation* operation = it->second.get();
    operation->info.status = OperationStatus::COMPLETED;
    operation->info.progress_percentage = 100.0;
    operation->info.result_path = resultPath;

    // Call progress callback
    if (operation->progressCallback) {
        operation->progressCallback(operation->info);
    }
}

void EncryptionOperationManager::failOperation(
    const std::string& operationId,
    const std::string& errorMessage
) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return;
    }

    Operation* operation = it->second.get();
    operation->info.status = OperationStatus::FAILED;
    operation->info.error_message = errorMessage;

    // Call progress callback
    if (operation->progressCallback) {
        operation->progressCallback(operation->info);
    }
}

bool EncryptionOperationManager::cancelOperation(const std::string& operationId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return false;
    }

    Operation* operation = it->second.get();
    if (operation->info.status == OperationStatus::COMPLETED ||
        operation->info.status == OperationStatus::FAILED) {
        return false;  // Cannot cancel completed or failed operations
    }

    operation->cancelled = true;
    operation->info.status = OperationStatus::CANCELLED;

    // Call progress callback
    if (operation->progressCallback) {
        operation->progressCallback(operation->info);
    }

    return true;
}

std::unique_ptr<OperationInfo> EncryptionOperationManager::getOperationInfo(
    const std::string& operationId
) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return nullptr;
    }

    return std::make_unique<OperationInfo>(it->second->info);
}

std::vector<OperationInfo> EncryptionOperationManager::getAllOperations() const {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<OperationInfo> operations;
    operations.reserve(m_operations.size());

    for (const auto& pair : m_operations) {
        operations.push_back(pair.second->info);
    }

    return operations;
}

bool EncryptionOperationManager::isOperationCancelled(const std::string& operationId) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return false;
    }

    return it->second->cancelled.load();
}

void EncryptionOperationManager::clearCompletedOperations() {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_operations.begin();
    while (it != m_operations.end()) {
        if (it->second->info.status == OperationStatus::COMPLETED ||
            it->second->info.status == OperationStatus::FAILED ||
            it->second->info.status == OperationStatus::CANCELLED) {
            it = m_operations.erase(it);
        } else {
            ++it;
        }
    }
}

std::string EncryptionOperationManager::generateOperationId() {
    uint64_t counter = m_operationCounter.fetch_add(1) + 1;
    std::ostringstream oss;
    oss << "op_" << std::hex << std::setfill('0') << std::setw(16) << counter;
    return oss.str();
}

