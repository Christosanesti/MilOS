#include "packet_parser.h"
#include "wireshark_integration.h"
#include <iostream>

PacketParser::PacketParser()
    : m_initialized(false)
    , m_wiresharkAvailable(false)
{
}

PacketParser::~PacketParser() {
}

bool PacketParser::initialize() {
    if (m_initialized) {
        return true;
    }

    WiresharkIntegration wireshark;
    m_wiresharkAvailable = wireshark.isAvailable() && wireshark.initialize();

    m_initialized = true;
    return true;
}

PacketData PacketParser::parsePacket(const PacketData& packet) {
    if (!m_initialized) {
        return packet;
    }

    if (m_wiresharkAvailable) {
        return parseWithWireshark(packet);
    } else {
        return parseWithLibpcap(packet);
    }
}

ProtocolInfo PacketParser::analyzeProtocol(const PacketData& packet) {
    ProtocolInfo info;
    info.name = packet.protocol;
    info.description = "Network protocol: " + packet.protocol;
    info.packet_count = 1;
    info.byte_count = packet.length;

    info.metadata["source"] = packet.source_address;
    info.metadata["destination"] = packet.destination_address;
    if (packet.source_port > 0) {
        info.metadata["source_port"] = std::to_string(packet.source_port);
    }
    if (packet.destination_port > 0) {
        info.metadata["destination_port"] = std::to_string(packet.destination_port);
    }

    return info;
}

std::map<std::string, std::string> PacketParser::extractMetadata(const PacketData& packet) {
    std::map<std::string, std::string> metadata;

    metadata["protocol"] = packet.protocol;
    metadata["length"] = std::to_string(packet.length);
    metadata["timestamp"] = std::to_string(packet.timestamp);
    metadata["interface"] = packet.interface;

    if (!packet.source_address.empty()) {
        metadata["source"] = packet.source_address;
    }
    if (!packet.destination_address.empty()) {
        metadata["destination"] = packet.destination_address;
    }
    if (packet.source_port > 0) {
        metadata["source_port"] = std::to_string(packet.source_port);
    }
    if (packet.destination_port > 0) {
        metadata["destination_port"] = std::to_string(packet.destination_port);
    }

    return metadata;
}

PacketData PacketParser::parseWithWireshark(const PacketData& packet) {
    // Wireshark API parsing would go here
    // For now, return packet as-is (already parsed by libpcap)
    return packet;
}

PacketData PacketParser::parseWithLibpcap(const PacketData& packet) {
    // libpcap parsing already done in PacketCapture
    return packet;
}

