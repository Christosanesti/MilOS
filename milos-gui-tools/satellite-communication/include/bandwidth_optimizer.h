#ifndef BANDWIDTH_OPTIMIZER_H
#define BANDWIDTH_OPTIMIZER_H

#include <QObject>
#include <QByteArray>
#include <QVariantMap>

/**
 * @brief Bandwidth Optimizer
 * 
 * Optimizes bandwidth usage for satellite communications.
 */
class BandwidthOptimizer : public QObject {
    Q_OBJECT

public:
    explicit BandwidthOptimizer(QObject* parent = nullptr);
    ~BandwidthOptimizer();

    /**
     * @brief Initialize bandwidth optimizer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Compress data
     * @param data Original data
     * @return Compressed data
     */
    QByteArray compress(const QByteArray& data) const;

    /**
     * @brief Decompress data
     * @param compressedData Compressed data
     * @return Decompressed data
     */
    QByteArray decompress(const QByteArray& compressedData) const;

    /**
     * @brief Optimize message for transmission
     * @param message Message data
     * @return Optimized message
     */
    QByteArray optimizeMessage(const QByteArray& message) const;

    /**
     * @brief Get compression ratio
     * @param originalSize Original size
     * @param compressedSize Compressed size
     * @return Compression ratio (0.0 to 1.0)
     */
    double getCompressionRatio(int originalSize, int compressedSize) const;

private:
    QByteArray applyCompression(const QByteArray& data) const;
};

#endif // BANDWIDTH_OPTIMIZER_H

