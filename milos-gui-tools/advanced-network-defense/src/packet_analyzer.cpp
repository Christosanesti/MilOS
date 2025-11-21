#include "packet_analyzer.h"
#include <QDebug>

PacketAnalyzer::PacketAnalyzer(QObject* parent)
    : QObject(parent)
{
}

PacketAnalyzer::~PacketAnalyzer() {
}

bool PacketAnalyzer::initialize() {
    return true;
}

PacketAnalysisResult PacketAnalyzer::analyzePacket(const QByteArray& packetData) {
    PacketAnalysisResult result;
    
    if (packetData.isEmpty()) {
        return result;
    }
    
    // Detect protocol
    result.protocol = detectProtocol(packetData);
    
    // Parse headers based on protocol
    if (result.protocol == "TCP") {
        result.headers = parseTCPHeader(packetData);
    } else if (result.protocol == "UDP") {
        result.headers = parseUDPHeader(packetData);
    } else {
        result.headers = parseIPHeader(packetData);
    }
    
    // Extract source/destination
    QVariantMap ipHeader = parseIPHeader(packetData);
    result.source = ipHeader.value("source").toString();
    result.destination = ipHeader.value("destination").toString();
    result.sourcePort = ipHeader.value("source_port").toInt();
    result.destinationPort = ipHeader.value("destination_port").toInt();
    
    // Extract payload (after headers)
    int headerSize = 20 + (result.protocol == "TCP" ? 20 : 8);  // IP + TCP/UDP headers
    if (packetData.size() > headerSize) {
        result.payload = packetData.mid(headerSize);
    }
    
    // Detect anomalies
    result.anomalies = detectAnomalies(packetData);
    result.isSuspicious = !result.anomalies.isEmpty();
    
    // Protocol-specific analysis
    result.analysis = extractProtocolInfo(packetData);
    
    return result;
}

QVariantMap PacketAnalyzer::analyzeFlow(const QList<QByteArray>& packets) const {
    QVariantMap flowAnalysis;
    
    flowAnalysis["packet_count"] = packets.size();
    
    int totalBytes = 0;
    QStringList protocols;
    for (const QByteArray& packet : packets) {
        totalBytes += packet.size();
        QString protocol = detectProtocol(packet);
        if (!protocols.contains(protocol)) {
            protocols.append(protocol);
        }
    }
    
    flowAnalysis["total_bytes"] = totalBytes;
    flowAnalysis["protocols"] = protocols;
    flowAnalysis["avg_packet_size"] = packets.size() > 0 ? totalBytes / packets.size() : 0;
    
    return flowAnalysis;
}

QStringList PacketAnalyzer::detectAnomalies(const QByteArray& packetData) const {
    QStringList anomalies;
    
    // Detect various anomalies
    if (packetData.size() < 64) {
        anomalies.append("Packet too small");
    }
    if (packetData.size() > 1500) {
        anomalies.append("Packet too large (possible fragmentation)");
    }
    
    // Check for suspicious patterns
    if (packetData.contains("\x00\x00\x00\x00")) {
        anomalies.append("Null bytes detected");
    }
    
    return anomalies;
}

QVariantMap PacketAnalyzer::extractProtocolInfo(const QByteArray& packetData) const {
    QVariantMap info;
    
    QString protocol = detectProtocol(packetData);
    info["protocol"] = protocol;
    info["packet_size"] = packetData.size();
    
    if (protocol == "TCP") {
        QVariantMap tcpHeader = parseTCPHeader(packetData);
        info["tcp_flags"] = tcpHeader.value("flags");
        info["sequence_number"] = tcpHeader.value("sequence");
    } else if (protocol == "UDP") {
        QVariantMap udpHeader = parseUDPHeader(packetData);
        info["udp_length"] = udpHeader.value("length");
    }
    
    return info;
}

QString PacketAnalyzer::detectProtocol(const QByteArray& packetData) const {
    if (packetData.size() < 20) {
        return "Unknown";
    }
    
    // Check IP protocol field (offset 9)
    unsigned char protocol = static_cast<unsigned char>(packetData[9]);
    
    switch (protocol) {
        case 6: return "TCP";
        case 17: return "UDP";
        case 1: return "ICMP";
        default: return "Unknown";
    }
}

QVariantMap PacketAnalyzer::parseEthernetHeader(const QByteArray& packetData) const {
    QVariantMap header;
    
    if (packetData.size() < 14) {
        return header;
    }
    
    // Extract MAC addresses (first 12 bytes)
    QByteArray destMac = packetData.left(6);
    QByteArray srcMac = packetData.mid(6, 6);
    
    header["destination_mac"] = destMac.toHex(':');
    header["source_mac"] = srcMac.toHex(':');
    
    return header;
}

QVariantMap PacketAnalyzer::parseIPHeader(const QByteArray& packetData) const {
    QVariantMap header;
    
    if (packetData.size() < 20) {
        return header;
    }
    
    // Skip Ethernet header (14 bytes) if present
    int offset = (packetData.size() > 14) ? 14 : 0;
    
    // Parse IP header
    header["version"] = (packetData[offset] >> 4) & 0x0F;
    header["header_length"] = (packetData[offset] & 0x0F) * 4;
    header["protocol"] = static_cast<unsigned char>(packetData[offset + 9]);
    
    // Source IP (offset 12)
    QString sourceIP = QString("%1.%2.%3.%4")
                      .arg(static_cast<unsigned char>(packetData[offset + 12]))
                      .arg(static_cast<unsigned char>(packetData[offset + 13]))
                      .arg(static_cast<unsigned char>(packetData[offset + 14]))
                      .arg(static_cast<unsigned char>(packetData[offset + 15]));
    header["source"] = sourceIP;
    
    // Destination IP (offset 16)
    QString destIP = QString("%1.%2.%3.%4")
                    .arg(static_cast<unsigned char>(packetData[offset + 16]))
                    .arg(static_cast<unsigned char>(packetData[offset + 17]))
                    .arg(static_cast<unsigned char>(packetData[offset + 18]))
                    .arg(static_cast<unsigned char>(packetData[offset + 19]));
    header["destination"] = destIP;
    
    return header;
}

QVariantMap PacketAnalyzer::parseTCPHeader(const QByteArray& packetData) const {
    QVariantMap header;
    
    // Skip Ethernet (14) + IP (20) = 34 bytes
    int offset = 34;
    if (packetData.size() < offset + 20) {
        return header;
    }
    
    // Parse TCP header
    header["source_port"] = (static_cast<unsigned char>(packetData[offset]) << 8) | 
                           static_cast<unsigned char>(packetData[offset + 1]);
    header["destination_port"] = (static_cast<unsigned char>(packetData[offset + 2]) << 8) | 
                                static_cast<unsigned char>(packetData[offset + 3]);
    header["sequence"] = 0;  // Would extract full 32-bit sequence
    header["flags"] = static_cast<unsigned char>(packetData[offset + 13]);
    
    return header;
}

QVariantMap PacketAnalyzer::parseUDPHeader(const QByteArray& packetData) const {
    QVariantMap header;
    
    // Skip Ethernet (14) + IP (20) = 34 bytes
    int offset = 34;
    if (packetData.size() < offset + 8) {
        return header;
    }
    
    // Parse UDP header
    header["source_port"] = (static_cast<unsigned char>(packetData[offset]) << 8) | 
                           static_cast<unsigned char>(packetData[offset + 1]);
    header["destination_port"] = (static_cast<unsigned char>(packetData[offset + 2]) << 8) | 
                                 static_cast<unsigned char>(packetData[offset + 3]);
    header["length"] = (static_cast<unsigned char>(packetData[offset + 4]) << 8) | 
                      static_cast<unsigned char>(packetData[offset + 5]);
    
    return header;
}

