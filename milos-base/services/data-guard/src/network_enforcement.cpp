#include "network_enforcement.h"
#include "config_parser.h"
#include "policy_manager.h"
#include <pcap/pcap.h>
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <atomic>

NetworkEnforcement::NetworkEnforcement()
    : m_running(false)
    , m_initialized(false)
    , m_blockedCount(0)
    , m_allowedCount(0)
    , m_configParser(nullptr)
    , m_policyManager(nullptr)
    , m_auditLogger(nullptr)
    , m_pcapHandle(nullptr)
    , m_captureRunning(false)
    , m_captureThread(nullptr)
{
}

NetworkEnforcement::~NetworkEnforcement() {
    stop();
}

bool NetworkEnforcement::initialize(ConfigParser* configParser, PolicyManager* policyManager, AuditLogger* auditLogger) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_policyManager = policyManager;
    m_auditLogger = auditLogger;

    // Initialize network hooks
    if (!initializeNetworkHooks()) {
        std::cerr << "Failed to initialize network hooks (graceful degradation enabled)" << std::endl;
        // Continue with reduced functionality if graceful degradation is enabled
        // TODO: Check config for graceful_degradation setting
    }

    m_initialized = true;
    return true;
}

bool NetworkEnforcement::start() {
    if (!m_initialized) {
        if (!initialize(m_configParser, m_policyManager)) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start packet capture thread if hooks are available
    if (m_pcapHandle) {
        m_captureRunning = true;
        pthread_t thread;
        if (pthread_create(&thread, nullptr, packetCaptureThread, this) != 0) {
            std::cerr << "Failed to create packet capture thread" << std::endl;
            return false;
        }
        m_captureThread = reinterpret_cast<void*>(thread);
    }

    m_running = true;
    std::cout << "Network enforcement started" << std::endl;
    return true;
}

void NetworkEnforcement::stop() {
    if (!m_running) {
        return;
    }

    // Stop packet capture thread
    if (m_captureThread) {
        m_captureRunning = false;
        pthread_join(reinterpret_cast<pthread_t>(m_captureThread), nullptr);
        m_captureThread = nullptr;
    }

    cleanupNetworkHooks();
    m_running = false;
    std::cout << "Network enforcement stopped" << std::endl;
}

bool NetworkEnforcement::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // TODO: Add health checks (e.g., network hook status)
    return true;
}

bool NetworkEnforcement::initializeNetworkHooks() {
    char errbuf[PCAP_ERRBUF_SIZE];
    
    // Try to find a network interface
    char* dev = pcap_lookupdev(errbuf);
    if (dev == nullptr) {
        std::cerr << "Could not find network device: " << errbuf << std::endl;
        // Graceful degradation: continue without packet capture
        return false;
    }

    // Open device for packet capture
    m_pcapHandle = pcap_open_live(dev, 65535, 1, 1000, errbuf);
    if (m_pcapHandle == nullptr) {
        std::cerr << "Could not open device " << dev << ": " << errbuf << std::endl;
        // Graceful degradation: continue without packet capture
        return false;
    }

    // Set non-blocking mode
    if (pcap_setnonblock(reinterpret_cast<pcap_t*>(m_pcapHandle), 1, errbuf) < 0) {
        std::cerr << "Could not set non-blocking mode: " << errbuf << std::endl;
        pcap_close(reinterpret_cast<pcap_t*>(m_pcapHandle));
        m_pcapHandle = nullptr;
        return false;
    }

    std::cout << "Network hooks initialized on device: " << dev << std::endl;
    return true;
}

void NetworkEnforcement::cleanupNetworkHooks() {
    if (m_pcapHandle) {
        pcap_close(reinterpret_cast<pcap_t*>(m_pcapHandle));
        m_pcapHandle = nullptr;
    }
}

bool NetworkEnforcement::inspectPacket(const void* packetData, size_t packetSize) {
    if (!m_policyManager) {
        return true;  // Allow if no policy manager
    }

    // Get all policies (sorted by priority)
    std::vector<NetworkPolicy> policies = m_policyManager->getPolicies();
    
    // Check if packet is encrypted
    bool encrypted = isPacketEncrypted(packetData, packetSize);

    // Apply policy rules in priority order
    for (const auto& policy : policies) {
        if (!policy.enabled) {
            continue;  // Skip disabled policies
        }

        // Evaluate policy rules
        for (const auto& rule : policy.rules) {
            // Check if rule matches packet
            bool ruleMatches = true;
            
            // TODO: Implement full rule matching (source, destination, protocol, port)
            // For MVP, we'll use simplified matching based on encryption requirement
            
            if (ruleMatches) {
                // Check encryption requirement
                if (rule.encryption_required && !encrypted) {
                    // Encryption required but packet is not encrypted
                    if (rule.action == "BLOCK") {
                        m_blockedCount++;
                        return false;  // Block packet
                    } else if (rule.action == "LOG") {
                        // Log but allow
                        // TODO: Log to audit service
                    }
                } else if (!rule.encryption_required || encrypted) {
                    // Encryption not required or packet is encrypted
                    if (rule.action == "ALLOW") {
                        m_allowedCount++;
                        return true;  // Allow packet
                    }
                }
            }
        }
    }

    // Default: block if encryption required but not encrypted
    if (!encrypted) {
        m_blockedCount++;
        return false;
    }

    m_allowedCount++;
    return true;
}

bool NetworkEnforcement::isPacketEncrypted(const void* packetData, size_t packetSize) {
    if (!packetData || packetSize == 0) {
        return false;
    }

    const unsigned char* data = static_cast<const unsigned char*>(packetData);
    
    // Check for Ethernet frame (minimum 14 bytes)
    if (packetSize < 14) {
        return false;
    }

    // Skip Ethernet header (14 bytes) to get IP header
    const unsigned char* ipHeader = data + 14;
    size_t ipHeaderSize = packetSize - 14;

    // Check for IP version (IPv4 = 0x45, IPv6 = 0x60)
    if (ipHeaderSize < 1) {
        return false;
    }

    // For IPv4 (0x45)
    if ((ipHeader[0] & 0xF0) == 0x40 && ipHeaderSize >= 20) {
        // Get protocol field (offset 9 in IP header)
        unsigned char protocol = ipHeader[9];
        
        // TCP protocol (6)
        if (protocol == 6 && ipHeaderSize >= 40) {
            // Get TCP header (starts after IP header, which is 20 bytes minimum)
            size_t ipHeaderLength = (ipHeader[0] & 0x0F) * 4;
            if (ipHeaderSize < ipHeaderLength + 20) {
                return false;
            }
            
            const unsigned char* tcpHeader = ipHeader + ipHeaderLength;
            size_t tcpDataSize = ipHeaderSize - ipHeaderLength - 20;  // Subtract TCP header (minimum 20 bytes)
            
            // Check for TLS handshake (0x16) in TCP data
            if (tcpDataSize >= 1 && tcpHeader[20] == 0x16) {
                return true;  // TLS handshake detected
            }
            
            // Check for TLS application data (0x17)
            if (tcpDataSize >= 1 && tcpHeader[20] == 0x17) {
                return true;  // TLS application data detected
            }
        }
        
        // UDP protocol (17) - check for DTLS
        if (protocol == 17 && ipHeaderSize >= 28) {
            size_t ipHeaderLength = (ipHeader[0] & 0x0F) * 4;
            if (ipHeaderSize < ipHeaderLength + 8) {
                return false;
            }
            
            const unsigned char* udpHeader = ipHeader + ipHeaderLength;
            size_t udpDataSize = ipHeaderSize - ipHeaderLength - 8;  // Subtract UDP header (8 bytes)
            
            // Check for DTLS (similar to TLS but over UDP)
            if (udpDataSize >= 1 && (udpHeader[8] == 0x16 || udpHeader[8] == 0x17)) {
                return true;  // DTLS detected
            }
        }
    }
    
    // Check for SSH (port 22) - simplified detection
    // SSH typically starts with "SSH-" in the protocol version string
    if (packetSize >= 20) {
        const char* sshMagic = "SSH-";
        if (memcmp(data, sshMagic, 4) == 0) {
            return true;  // SSH detected
        }
    }

    return false;
}

void* NetworkEnforcement::packetCaptureThread(void* arg) {
    NetworkEnforcement* self = static_cast<NetworkEnforcement*>(arg);
    pcap_t* handle = reinterpret_cast<pcap_t*>(self->m_pcapHandle);
    
    struct pcap_pkthdr* header;
    const unsigned char* packet;
    
    while (self->m_captureRunning) {
        int result = pcap_next_ex(handle, &header, &packet);
        
        if (result == 1) {
            // Packet captured
            self->processPacket(header, packet);
        } else if (result == 0) {
            // Timeout (non-blocking mode)
            continue;
        } else if (result == -1) {
            // Error
            std::cerr << "Error reading packet: " << pcap_geterr(handle) << std::endl;
            break;
        } else if (result == -2) {
            // End of file (should not happen with live capture)
            break;
        }
    }
    
    return nullptr;
}

void NetworkEnforcement::processPacket(const void* header, const unsigned char* packet) {
    const pcap_pkthdr* hdr = static_cast<const pcap_pkthdr*>(header);
    
    // Inspect packet
    bool allowed = inspectPacket(packet, hdr->caplen);
    
    // Extract packet information for logging
    QString source = "unknown";
    QString destination = "unknown";
    QString protocol = "unknown";
    
    // TODO: Extract actual source/destination/protocol from packet headers
    // For now, use placeholder values
    
    if (!allowed) {
        // Packet blocked - log to audit service
        if (m_auditLogger) {
            m_auditLogger->logTransmissionAttempt(source, destination, protocol, false, "BLOCK");
        }
        std::cout << "Packet blocked: size=" << hdr->caplen << std::endl;
    } else {
        // Packet allowed - log to audit service (optional, can be configured)
        if (m_auditLogger) {
            // Only log if configured to log allowed transmissions
            // For now, only log blocked transmissions
        }
    }
}

