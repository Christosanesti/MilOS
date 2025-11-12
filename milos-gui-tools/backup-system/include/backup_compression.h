#ifndef BACKUP_COMPRESSION_H
#define BACKUP_COMPRESSION_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Compression Algorithm
 */
enum class CompressionAlgorithm {
    None,
    Gzip,
    Bzip2,
    Xz
};

/**
 * @brief Backup Compression
 * 
 * Handles optional backup compression.
 */
class BackupCompression {
public:
    BackupCompression();
    ~BackupCompression();

    /**
     * @brief Initialize compression
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Compress backup data
     * @param data Uncompressed backup data
     * @param algorithm Compression algorithm
     * @return Compressed data
     */
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data,
                                  CompressionAlgorithm algorithm = CompressionAlgorithm::Gzip);

    /**
     * @brief Decompress backup data
     * @param compressedData Compressed backup data
     * @param algorithm Compression algorithm
     * @return Decompressed data
     */
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressedData,
                                     CompressionAlgorithm algorithm = CompressionAlgorithm::Gzip);

    /**
     * @brief Check if compression is supported
     * @param algorithm Compression algorithm
     * @return true if supported, false otherwise
     */
    bool isSupported(CompressionAlgorithm algorithm) const;

private:
    bool m_initialized;

    /**
     * @brief Compress using Gzip
     */
    std::vector<uint8_t> compressGzip(const std::vector<uint8_t>& data);

    /**
     * @brief Decompress using Gzip
     */
    std::vector<uint8_t> decompressGzip(const std::vector<uint8_t>& compressedData);
};

#endif // BACKUP_COMPRESSION_H

