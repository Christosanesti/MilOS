#include "milos/encryption/file_encryption.h"
#include "milos/pqc/algorithm_factory.h"
#include "milos/pqc/hybrid_encryption.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>

// Constants
const size_t CHUNK_SIZE = 64 * 1024;  // 64 KB chunks for streaming

FileEncryption::FileEncryption()
    : m_initialized(false)
    , m_hybridEnabled(false)
    , m_algorithmId("")
{
}

FileEncryption::~FileEncryption() {
}

bool FileEncryption::initialize(const std::string& algorithmId) {
    if (m_initialized) {
        return true;
    }

    m_algorithmId = algorithmId.empty() ? "Kyber512" : algorithmId;  // Default to Kyber512

    if (!initializeAlgorithm()) {
        std::cerr << "Failed to initialize algorithm: " << m_algorithmId << std::endl;
        return false;
    }

    // Initialize hybrid encryption if enabled
    if (m_hybridEnabled && m_algorithm) {
        m_hybridEncryption = std::make_unique<milos::pqc::HybridEncryption>();
        // Note: HybridEncryption needs both PQC and classical algorithms
        // For now, we'll use PQC only
        m_hybridEncryption->initialize(std::move(m_algorithm));
    }

    m_initialized = true;
    return true;
}

bool FileEncryption::initializeAlgorithm() {
    milos::pqc::AlgorithmFactory& factory = milos::pqc::AlgorithmFactory::getInstance();
    
    m_algorithm = factory.createAlgorithm(m_algorithmId);
    if (!m_algorithm) {
        std::cerr << "Algorithm not found: " << m_algorithmId << std::endl;
        return false;
    }

    if (!m_algorithm->isHealthy()) {
        std::cerr << "Algorithm not healthy: " << m_algorithmId << std::endl;
        return false;
    }

    return true;
}

FileEncryptionResult FileEncryption::encryptFile(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::vector<uint8_t>& publicKey,
    FileEncryptionProgressCallback progressCallback
) {
    FileEncryptionResult result;
    result.success = false;

    if (!m_initialized) {
        result.error_message = "File encryption not initialized";
        return result;
    }

    // Open input file
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        result.error_message = "Cannot open input file: " + inputPath;
        return result;
    }

    // Get file size
    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Open output file
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        result.error_message = "Cannot open output file: " + outputPath;
        return result;
    }

    // Write file header (algorithm ID, file size, etc.)
    // Simplified header: algorithm ID length + algorithm ID + file size
    uint32_t algoIdLen = static_cast<uint32_t>(m_algorithmId.length());
    outputFile.write(reinterpret_cast<const char*>(&algoIdLen), sizeof(algoIdLen));
    outputFile.write(m_algorithmId.c_str(), algoIdLen);
    outputFile.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));

    // Encrypt file in chunks
    size_t bytesProcessed = 0;
    std::vector<uint8_t> buffer(CHUNK_SIZE);
    
    while (inputFile.good()) {
        inputFile.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
        size_t bytesRead = inputFile.gcount();
        
        if (bytesRead == 0) {
            break;
        }

        // Resize buffer to actual bytes read
        buffer.resize(bytesRead);

        // Encrypt chunk
        std::vector<uint8_t> encryptedChunk = encryptChunk(buffer, publicKey);
        if (encryptedChunk.empty()) {
            result.error_message = "Encryption failed for chunk";
            return result;
        }

        // Write encrypted chunk size and data
        uint32_t chunkSize = static_cast<uint32_t>(encryptedChunk.size());
        outputFile.write(reinterpret_cast<const char*>(&chunkSize), sizeof(chunkSize));
        outputFile.write(reinterpret_cast<const char*>(encryptedChunk.data()), encryptedChunk.size());

        bytesProcessed += bytesRead;

        // Call progress callback
        if (progressCallback) {
            double percentage = (static_cast<double>(bytesProcessed) / fileSize) * 100.0;
            progressCallback(bytesProcessed, fileSize, percentage);
        }
    }

    inputFile.close();
    outputFile.close();

    // Get output file size
    std::ifstream checkFile(outputPath, std::ios::binary | std::ios::ate);
    result.encrypted_size = checkFile.tellg();
    checkFile.close();

    result.success = true;
    result.encrypted_file_path = outputPath;
    result.algorithm_used = m_algorithmId;

    return result;
}

FileDecryptionResult FileEncryption::decryptFile(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::vector<uint8_t>& privateKey,
    FileEncryptionProgressCallback progressCallback
) {
    FileDecryptionResult result;
    result.success = false;

    if (!m_initialized) {
        result.error_message = "File encryption not initialized";
        return result;
    }

    // Open input file
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        result.error_message = "Cannot open input file: " + inputPath;
        return result;
    }

    // Read file header
    uint32_t algoIdLen;
    inputFile.read(reinterpret_cast<char*>(&algoIdLen), sizeof(algoIdLen));
    if (inputFile.gcount() != sizeof(algoIdLen)) {
        result.error_message = "Invalid file header";
        return result;
    }

    std::string storedAlgorithmId(algoIdLen, '\0');
    inputFile.read(&storedAlgorithmId[0], algoIdLen);
    if (inputFile.gcount() != algoIdLen) {
        result.error_message = "Invalid algorithm ID in header";
        return result;
    }

    // If algorithm ID doesn't match, reinitialize
    if (storedAlgorithmId != m_algorithmId) {
        m_algorithmId = storedAlgorithmId;
        if (!initializeAlgorithm()) {
            result.error_message = "Cannot initialize algorithm: " + storedAlgorithmId;
            return result;
        }
    }

    size_t originalFileSize;
    inputFile.read(reinterpret_cast<char*>(&originalFileSize), sizeof(originalFileSize));
    if (inputFile.gcount() != sizeof(originalFileSize)) {
        result.error_message = "Invalid file size in header";
        return result;
    }

    // Open output file
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        result.error_message = "Cannot open output file: " + outputPath;
        return result;
    }

    // Decrypt file in chunks
    size_t bytesProcessed = 0;
    
    while (inputFile.good()) {
        // Read chunk size
        uint32_t chunkSize;
        inputFile.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
        if (inputFile.gcount() != sizeof(chunkSize)) {
            break;  // End of file
        }

        // Read encrypted chunk
        std::vector<uint8_t> encryptedChunk(chunkSize);
        inputFile.read(reinterpret_cast<char*>(encryptedChunk.data()), chunkSize);
        if (inputFile.gcount() != chunkSize) {
            result.error_message = "Incomplete chunk data";
            return result;
        }

        // Decrypt chunk
        std::vector<uint8_t> decryptedChunk = decryptChunk(encryptedChunk, privateKey);
        if (decryptedChunk.empty()) {
            result.error_message = "Decryption failed for chunk";
            return result;
        }

        // Write decrypted chunk
        outputFile.write(reinterpret_cast<const char*>(decryptedChunk.data()), decryptedChunk.size());

        bytesProcessed += decryptedChunk.size();

        // Call progress callback
        if (progressCallback) {
            double percentage = (static_cast<double>(bytesProcessed) / originalFileSize) * 100.0;
            progressCallback(bytesProcessed, originalFileSize, percentage);
        }
    }

    inputFile.close();
    outputFile.close();

    result.success = true;
    result.decrypted_file_path = outputPath;
    result.decrypted_size = bytesProcessed;
    result.algorithm_used = m_algorithmId;

    return result;
}

std::vector<uint8_t> FileEncryption::encryptData(
    const std::vector<uint8_t>& data,
    const std::vector<uint8_t>& publicKey
) {
    if (!m_initialized || !m_algorithm) {
        return {};
    }

    return encryptChunk(data, publicKey);
}

std::vector<uint8_t> FileEncryption::decryptData(
    const std::vector<uint8_t>& encryptedData,
    const std::vector<uint8_t>& privateKey
) {
    if (!m_initialized || !m_algorithm) {
        return {};
    }

    return decryptChunk(encryptedData, privateKey);
}

std::vector<uint8_t> FileEncryption::encryptChunk(
    const std::vector<uint8_t>& data,
    const std::vector<uint8_t>& publicKey
) {
    if (!m_algorithm) {
        return {};
    }

    if (m_hybridEnabled && m_hybridEncryption) {
        // Use hybrid encryption
        milos::pqc::EncryptionResult result = m_hybridEncryption->encrypt(data, milos::pqc::HybridMode::HYBRID, publicKey);
        if (result.success) {
            return result.ciphertext;
        }
        return {};
    } else {
        // Use PQC algorithm directly
        milos::pqc::EncryptionResult result = m_algorithm->encrypt(data, publicKey);
        if (result.success) {
            return result.ciphertext;
        }
        return {};
    }
}

std::vector<uint8_t> FileEncryption::decryptChunk(
    const std::vector<uint8_t>& encryptedData,
    const std::vector<uint8_t>& privateKey
) {
    if (!m_algorithm) {
        return {};
    }

    if (m_hybridEnabled && m_hybridEncryption) {
        // Use hybrid decryption
        milos::pqc::DecryptionResult result = m_hybridEncryption->decrypt(encryptedData, milos::pqc::HybridMode::HYBRID, privateKey);
        if (result.success) {
            return result.plaintext;
        }
        return {};
    } else {
        // Use PQC algorithm directly
        milos::pqc::DecryptionResult result = m_algorithm->decrypt(encryptedData, privateKey);
        if (result.success) {
            return result.plaintext;
        }
        return {};
    }
}

