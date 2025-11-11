#include "packet_statistics.h"
#include <algorithm>
#include <cmath>

PacketStatistics::PacketStatistics()
    : m_totalPackets(0)
    , m_totalBytes(0)
{
}

PacketStatistics::~PacketStatistics() {
}

void PacketStatistics::initialize() {
    reset();
}

void PacketStatistics::updateStatistics(const PacketData& packet) {
    m_totalPackets++;
    m_totalBytes += packet.length;

    if (!packet.protocol.empty()) {
        getProtocolPacketCounter(packet.protocol)++;
        getProtocolByteCounter(packet.protocol) += packet.length;
    }
}

std::map<std::string, ProtocolStats> PacketStatistics::getProtocolStatistics() const {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::map<std::string, ProtocolStats> stats;
    uint64_t totalPackets = m_totalPackets.load();
    uint64_t totalBytes = m_totalBytes.load();

    for (const auto& pair : m_protocolPackets) {
        const std::string& protocol = pair.first;
        uint64_t packets = pair.second.load();
        uint64_t bytes = m_protocolBytes.at(protocol).load();

        ProtocolStats protocolStats;
        protocolStats.packet_count = packets;
        protocolStats.byte_count = bytes;
        protocolStats.percentage_packets = totalPackets > 0 ?
            (static_cast<double>(packets) / totalPackets) * 100.0 : 0.0;
        protocolStats.percentage_bytes = totalBytes > 0 ?
            (static_cast<double>(bytes) / totalBytes) * 100.0 : 0.0;

        stats[protocol] = protocolStats;
    }

    return stats;
}

ProtocolStats PacketStatistics::getProtocolStats(const std::string& protocol) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    ProtocolStats stats;
    uint64_t totalPackets = m_totalPackets.load();
    uint64_t totalBytes = m_totalBytes.load();

    auto packetIt = m_protocolPackets.find(protocol);
    auto byteIt = m_protocolBytes.find(protocol);

    if (packetIt != m_protocolPackets.end() && byteIt != m_protocolBytes.end()) {
        uint64_t packets = packetIt->second.load();
        uint64_t bytes = byteIt->second.load();

        stats.packet_count = packets;
        stats.byte_count = bytes;
        stats.percentage_packets = totalPackets > 0 ?
            (static_cast<double>(packets) / totalPackets) * 100.0 : 0.0;
        stats.percentage_bytes = totalBytes > 0 ?
            (static_cast<double>(bytes) / totalBytes) * 100.0 : 0.0;
    }

    return stats;
}

void PacketStatistics::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_totalPackets = 0;
    m_totalBytes = 0;
    m_protocolPackets.clear();
    m_protocolBytes.clear();
}

std::atomic<uint64_t>& PacketStatistics::getProtocolPacketCounter(const std::string& protocol) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_protocolPackets.find(protocol);
    if (it == m_protocolPackets.end()) {
        m_protocolPackets[protocol] = std::atomic<uint64_t>(0);
        return m_protocolPackets[protocol];
    }
    return it->second;
}

std::atomic<uint64_t>& PacketStatistics::getProtocolByteCounter(const std::string& protocol) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_protocolBytes.find(protocol);
    if (it == m_protocolBytes.end()) {
        m_protocolBytes[protocol] = std::atomic<uint64_t>(0);
        return m_protocolBytes[protocol];
    }
    return it->second;
}

