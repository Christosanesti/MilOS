#ifndef PACKET_CAPTURE_H
#define PACKET_CAPTURE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>

#ifdef HAVE_LIBPCAP
#include <pcap/pcap.h>
#endif

// Forward declaration
struct PacketData;

/**
 * @brief Packet Capture Callback
 * 
 * @param packet Packet data
 */
using PacketCaptureCallback = std::function<void(const PacketData& packet)>;

/**
 * @brief Packet Data Structure
 */
struct PacketData {
    std::vector<uint8_t> data;
    size_t length;
    uint64_t timestamp;
    std::string interface;
    std::string protocol;
    std::string source_address;
    std::string destination_address;
    uint16_t source_port;
    uint16_t destination_port;
};

/**
 * @brief Packet Capture
 * 
 * Provides packet capture functionality using libpcap (with Wireshark integration).
 * Supports real-time packet capture up to 1Gbps.
 */
class PacketCapture {
public:
    PacketCapture();
    ~PacketCapture();

    /**
     * @brief Initialize packet capture
     * @param interface Network interface name (empty for default)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& interface = "");

    /**
     * @brief Start packet capture
     * @param filter BPF filter string (optional)
     * @param callback Packet capture callback
     * @return true if start successful, false otherwise
     */
    bool startCapture(
        const std::string& filter = "",
        PacketCaptureCallback callback = nullptr
    );

    /**
     * @brief Stop packet capture
     * @return true if stop successful, false otherwise
     */
    bool stopCapture();

    /**
     * @brief Check if capture is running
     * @return true if running, false otherwise
     */
    bool isCapturing() const { return m_capturing.load(); }

    /**
     * @brief Get available network interfaces
     * @return Vector of interface names
     */
    std::vector<std::string> getAvailableInterfaces() const;

    /**
     * @brief Set capture buffer size
     * @param bufferSize Buffer size in bytes
     */
    void setBufferSize(size_t bufferSize) { m_bufferSize = bufferSize; }

    /**
     * @brief Get capture statistics
     * @param packetsCaptured Number of packets captured
     * @param bytesCaptured Number of bytes captured
     * @return true if statistics retrieved, false otherwise
     */
    bool getStatistics(uint64_t& packetsCaptured, uint64_t& bytesCaptured) const;

private:
    bool m_initialized;
    std::atomic<bool> m_capturing;
    std::string m_interface;
    std::string m_filter;
    PacketCaptureCallback m_callback;
    size_t m_bufferSize;
    
#ifdef HAVE_LIBPCAP
    pcap_t* m_pcapHandle;
    char m_errorBuffer[PCAP_ERRBUF_SIZE];
#endif

    std::thread m_captureThread;
    std::mutex m_mutex;
    
    uint64_t m_packetsCaptured;
    uint64_t m_bytesCaptured;

    /**
     * @brief Capture thread function
     */
    void captureThreadFunction();

    /**
     * @brief Process captured packet
     */
    void processPacket(const uint8_t* packet, size_t length, const struct pcap_pkthdr* header);

    /**
     * @brief Parse packet data
     */
    PacketData parsePacket(const uint8_t* packet, size_t length) const;
};

#endif // PACKET_CAPTURE_H

