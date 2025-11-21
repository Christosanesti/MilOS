#ifndef SATCOM_PROTOCOL_H
#define SATCOM_PROTOCOL_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

/**
 * @brief Satellite Protocol Type
 */
enum class SatComProtocolType {
    MILSTAR,
    AEHF,
    WGS,
    Iridium,
    Inmarsat,
    VSAT,
    Custom
};

/**
 * @brief Satellite Message
 */
struct SatComMessage {
    QString id;
    QString from;
    QString to;
    QByteArray payload;
    QDateTime timestamp;
    bool encrypted;
    int priority;
    QString protocol;
    double bandwidthUsed;  // in Mbps
};

/**
 * @brief Satellite Protocol Handler
 * 
 * Handles satellite communication protocols.
 */
class SatComProtocol : public QObject {
    Q_OBJECT

public:
    explicit SatComProtocol(QObject* parent = nullptr);
    ~SatComProtocol();

    /**
     * @brief Initialize satellite protocol
     * @param protocolType Protocol type
     * @param satelliteId Satellite ID
     * @return true if initialization successful, false otherwise
     */
    bool initialize(SatComProtocolType protocolType, const QString& satelliteId);

    /**
     * @brief Send message
     * @param message Satellite message
     * @return true if send successful, false otherwise
     */
    bool sendMessage(const SatComMessage& message);

    /**
     * @brief Receive message
     * @return Satellite message or empty if none
     */
    SatComMessage receiveMessage();

    /**
     * @brief Get available bandwidth
     * @return Available bandwidth in Mbps
     */
    double getAvailableBandwidth() const { return m_availableBandwidth; }

    /**
     * @brief Get protocol type
     * @return Protocol type
     */
    SatComProtocolType protocolType() const { return m_protocolType; }

Q_SIGNALS:
    /**
     * @brief Emitted when message is received
     */
    void messageReceived(const SatComMessage& message);

    /**
     * @brief Emitted when bandwidth changes
     */
    void bandwidthChanged(double availableBandwidth);

private:
    SatComProtocolType m_protocolType;
    QString m_satelliteId;
    double m_availableBandwidth;
    
    double calculateBandwidthUsage(const QByteArray& payload) const;
};

Q_DECLARE_METATYPE(SatComProtocolType)
Q_DECLARE_METATYPE(SatComMessage)

#endif // SATCOM_PROTOCOL_H

