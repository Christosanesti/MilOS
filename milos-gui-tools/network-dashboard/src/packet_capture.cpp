#include "packet_capture.h"
#include <iostream>
#include <cstring>
#include <algorithm>

#ifdef HAVE_LIBPCAP
#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#endif

PacketCapture::PacketCapture()
    : m_initialized(false)
    , m_capturing(false)
    , m_bufferSize(1024 * 1024)  // 1 MB default
#ifdef HAVE_LIBPCAP
    , m_pcapHandle(nullptr)
#endif
    , m_packetsCaptured(0)
    , m_bytesCaptured(0)
{
    std::memset(m_errorBuffer, 0, sizeof(m_errorBuffer));
}

PacketCapture::~PacketCapture() {
    stopCapture();
}

bool PacketCapture::initialize(const std::string& interface) {
    if (m_initialized) {
        return true;
    }

#ifdef HAVE_LIBPCAP
    m_interface = interface.empty() ? "any" : interface;

    // Open network interface for capture
    m_pcapHandle = pcap_open_live(m_interface.c_str(), 65535, 1, 1000, m_errorBuffer);
    if (m_pcapHandle == nullptr) {
        std::cerr << "Failed to open interface " << m_interface << ": " << m_errorBuffer << std::endl;
        return false;
    }

    // Set buffer size
    if (pcap_set_buffer_size(m_pcapHandle, m_bufferSize) != 0) {
        std::cerr << "Warning: Failed to set buffer size" << std::endl;
    }

    m_initialized = true;
    return true;
#else
    std::cerr << "libpcap not available. Packet capture not supported." << std::endl;
    return false;
#endif
}

bool PacketCapture::startCapture(
    const std::string& filter,
    PacketCaptureCallback callback
) {
    if (!m_initialized) {
        std::cerr << "Packet capture not initialized" << std::endl;
        return false;
    }

    if (m_capturing.load()) {
        std::cerr << "Packet capture already running" << std::endl;
        return false;
    }

    m_filter = filter;
    m_callback = callback;
    m_packetsCaptured = 0;
    m_bytesCaptured = 0;

#ifdef HAVE_LIBPCAP
    // Compile filter if provided
    if (!filter.empty()) {
        struct bpf_program fp;
        if (pcap_compile(m_pcapHandle, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
            std::cerr << "Error compiling filter: " << pcap_geterr(m_pcapHandle) << std::endl;
            return false;
        }

        if (pcap_setfilter(m_pcapHandle, &fp) == -1) {
            std::cerr << "Error setting filter: " << pcap_geterr(m_pcapHandle) << std::endl;
            pcap_freecode(&fp);
            return false;
        }

        pcap_freecode(&fp);
    }

    m_capturing = true;

    // Start capture thread
    m_captureThread = std::thread(&PacketCapture::captureThreadFunction, this);

    return true;
#else
    return false;
#endif
}

bool PacketCapture::stopCapture() {
    if (!m_capturing.load()) {
        return true;
    }

    m_capturing = false;

#ifdef HAVE_LIBPCAP
    // Close pcap handle to stop capture
    if (m_pcapHandle != nullptr) {
        pcap_breakloop(m_pcapHandle);
    }
#endif

    // Wait for capture thread to finish
    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }

    return true;
}

std::vector<std::string> PacketCapture::getAvailableInterfaces() const {
    std::vector<std::string> interfaces;

#ifdef HAVE_LIBPCAP
    pcap_if_t* alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding devices: " << errbuf << std::endl;
        return interfaces;
    }

    for (pcap_if_t* dev = alldevs; dev != nullptr; dev = dev->next) {
        interfaces.push_back(dev->name);
    }

    pcap_freealldevs(alldevs);
#endif

    return interfaces;
}

bool PacketCapture::getStatistics(uint64_t& packetsCaptured, uint64_t& bytesCaptured) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    packetsCaptured = m_packetsCaptured;
    bytesCaptured = m_bytesCaptured;
    return true;
}

void PacketCapture::captureThreadFunction() {
#ifdef HAVE_LIBPCAP
    struct pcap_pkthdr* header;
    const uint8_t* packet;

    while (m_capturing.load()) {
        int result = pcap_next_ex(m_pcapHandle, &header, &packet);
        
        if (result == 1) {
            // Packet captured
            processPacket(packet, header->caplen, header);
        } else if (result == 0) {
            // Timeout
            continue;
        } else if (result == -1) {
            // Error
            std::cerr << "Error reading packet: " << pcap_geterr(m_pcapHandle) << std::endl;
            break;
        } else if (result == -2) {
            // Break loop
            break;
        }
    }
#endif
}

#ifdef HAVE_LIBPCAP
void PacketCapture::processPacket(
    const uint8_t* packet,
    size_t length,
    const struct pcap_pkthdr* header
) {
#else
void PacketCapture::processPacket(
    const uint8_t* packet,
    size_t length,
    void* header
) {
    // Placeholder for when libpcap is not available
    (void)header;
#endif
    std::lock_guard<std::mutex> lock(m_mutex);

    m_packetsCaptured++;
    m_bytesCaptured += length;

    // Parse packet
    PacketData packetData = parsePacket(packet, length);
#ifdef HAVE_LIBPCAP
    if (header) {
        packetData.timestamp = header->ts.tv_sec * 1000000ULL + header->ts.tv_usec;
    }
#else
    // Use current time if header not available
    packetData.timestamp = 0;  // Would use gettimeofday or similar
#endif
    packetData.interface = m_interface;

    // Call callback if provided
    if (m_callback) {
        m_callback(packetData);
    }
}

PacketData PacketCapture::parsePacket(const uint8_t* packet, size_t length) const {
    PacketData data;
    data.data.assign(packet, packet + length);
    data.length = length;
    data.protocol = "Unknown";
    data.source_port = 0;
    data.destination_port = 0;

#ifdef HAVE_LIBPCAP
    // Parse Ethernet header
    if (length < sizeof(struct ether_header)) {
        return data;
    }

    const struct ether_header* eth = reinterpret_cast<const struct ether_header*>(packet);
    uint16_t ethertype = ntohs(eth->ether_type);

    // Parse IP header
    if (ethertype == ETHERTYPE_IP && length >= sizeof(struct ether_header) + sizeof(struct ip)) {
        const struct ip* iph = reinterpret_cast<const struct ip*>(packet + sizeof(struct ether_header));
        
        char src[INET_ADDRSTRLEN];
        char dst[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &iph->ip_src, src, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &iph->ip_dst, dst, INET_ADDRSTRLEN);
        
        data.source_address = src;
        data.destination_address = dst;

        // Parse TCP/UDP
        if (iph->ip_p == IPPROTO_TCP && length >= sizeof(struct ether_header) + (iph->ip_hl * 4) + sizeof(struct tcphdr)) {
            const struct tcphdr* tcph = reinterpret_cast<const struct tcphdr*>(
                packet + sizeof(struct ether_header) + (iph->ip_hl * 4)
            );
            data.protocol = "TCP";
            data.source_port = ntohs(tcph->source);
            data.destination_port = ntohs(tcph->dest);
        } else if (iph->ip_p == IPPROTO_UDP && length >= sizeof(struct ether_header) + (iph->ip_hl * 4) + sizeof(struct udphdr)) {
            const struct udphdr* udph = reinterpret_cast<const struct udphdr*>(
                packet + sizeof(struct ether_header) + (iph->ip_hl * 4)
            );
            data.protocol = "UDP";
            data.source_port = ntohs(udph->source);
            data.destination_port = ntohs(udph->dest);
        } else if (iph->ip_p == IPPROTO_ICMP) {
            data.protocol = "ICMP";
        }
    }
#endif

    return data;
}

