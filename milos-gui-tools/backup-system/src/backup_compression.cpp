#include "backup_compression.h"
#include <zlib.h>
#include <cstring>
#include <iostream>

BackupCompression::BackupCompression()
    : m_initialized(false)
{
}

BackupCompression::~BackupCompression() {
}

bool BackupCompression::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

bool BackupCompression::isSupported(CompressionAlgorithm algorithm) const {
    switch (algorithm) {
        case CompressionAlgorithm::None:
            return true;
        case CompressionAlgorithm::Gzip:
            return true;  // zlib available
        case CompressionAlgorithm::Bzip2:
        case CompressionAlgorithm::Xz:
            return false;  // Not yet implemented
    }
    return false;
}

std::vector<uint8_t> BackupCompression::compress(const std::vector<uint8_t>& data,
                                                  CompressionAlgorithm algorithm) {
    if (!m_initialized) {
        return data;
    }

    if (algorithm == CompressionAlgorithm::None) {
        return data;
    }

    if (algorithm == CompressionAlgorithm::Gzip) {
        return compressGzip(data);
    }

    // Unsupported algorithm, return uncompressed
    return data;
}

std::vector<uint8_t> BackupCompression::compressGzip(const std::vector<uint8_t>& data) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        std::cerr << "Failed to initialize zlib for compression" << std::endl;
        return data;
    }

    zs.next_in = const_cast<Bytef*>(data.data());
    zs.avail_in = data.size();

    std::vector<uint8_t> compressed;
    compressed.resize(data.size() * 1.1 + 12);  // Estimate

    zs.next_out = compressed.data();
    zs.avail_out = compressed.size();

    int ret = deflate(&zs, Z_FINISH);
    if (ret != Z_STREAM_END) {
        deflateEnd(&zs);
        std::cerr << "Compression failed" << std::endl;
        return data;
    }

    compressed.resize(zs.total_out);
    deflateEnd(&zs);

    return compressed;
}

std::vector<uint8_t> BackupCompression::decompress(const std::vector<uint8_t>& compressedData,
                                                     CompressionAlgorithm algorithm) {
    if (!m_initialized) {
        return compressedData;
    }

    if (algorithm == CompressionAlgorithm::None) {
        return compressedData;
    }

    if (algorithm == CompressionAlgorithm::Gzip) {
        return decompressGzip(compressedData);
    }

    // Unsupported algorithm, return as-is
    return compressedData;
}

std::vector<uint8_t> BackupCompression::decompressGzip(const std::vector<uint8_t>& compressedData) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 15 + 16) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression" << std::endl;
        return compressedData;
    }

    zs.next_in = const_cast<Bytef*>(compressedData.data());
    zs.avail_in = compressedData.size();

    std::vector<uint8_t> decompressed;
    decompressed.resize(compressedData.size() * 4);  // Estimate

    zs.next_out = decompressed.data();
    zs.avail_out = decompressed.size();

    int ret = inflate(&zs, Z_FINISH);
    if (ret != Z_STREAM_END && ret != Z_OK) {
        inflateEnd(&zs);
        std::cerr << "Decompression failed" << std::endl;
        return compressedData;
    }

    decompressed.resize(zs.total_out);
    inflateEnd(&zs);

    return decompressed;
}

