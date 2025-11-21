#ifndef PACKET_ANALYZER_H
#define PACKET_ANALYZER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <QList>

/**
 * @brief Packet Analysis Result
 */
struct PacketAnalysisResult {
    QString protocol;
    QString source;
    QString destination;
    int sourcePort;
    int destinationPort;
    QByteArray payload;
    QVariantMap headers;
    QVariantMap analysis;
    bool isSuspicious;
    QStringList anomalies;
};

/**
 * @brief Packet Analyzer
 * 
 * Advanced packet analysis tools.
 */
class PacketAnalyzer : public QObject {
    Q_OBJECT

public:
    explicit PacketAnalyzer(QObject* parent = nullptr);
    ~PacketAnalyzer();

    /**
     * @brief Initialize packet analyzer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Analyze packet
     * @param packetData Raw packet data
     * @return Analysis result
     */
    PacketAnalysisResult analyzePacket(const QByteArray& packetData);

    /**
     * @brief Analyze packet flow
     * @param packets List of packets
     * @return Flow analysis result
     */
    QVariantMap analyzeFlow(const QList<QByteArray>& packets) const;

    /**
     * @brief Detect anomalies in packet
     * @param packetData Raw packet data
     * @return List of detected anomalies
     */
    QStringList detectAnomalies(const QByteArray& packetData) const;

    /**
     * @brief Extract protocol information
     * @param packetData Raw packet data
     * @return Protocol information map
     */
    QVariantMap extractProtocolInfo(const QByteArray& packetData) const;

private:
    QString detectProtocol(const QByteArray& packetData) const;
    QVariantMap parseEthernetHeader(const QByteArray& packetData) const;
    QVariantMap parseIPHeader(const QByteArray& packetData) const;
    QVariantMap parseTCPHeader(const QByteArray& packetData) const;
    QVariantMap parseUDPHeader(const QByteArray& packetData) const;
};

Q_DECLARE_METATYPE(PacketAnalysisResult)

#endif // PACKET_ANALYZER_H

