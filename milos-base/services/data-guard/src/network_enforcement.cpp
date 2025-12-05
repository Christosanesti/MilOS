#include "network_enforcement.h"
#include "config_parser.h"
#include "policy_manager.h"
#include "audit_logger.h"
#include <pcap/pcap.h>
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <atomic>
#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <sstream>
#include <arpa/inet.h>
#include <mutex>

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
        LOG_WARNING("Failed to initialize network hooks (graceful degradation enabled)");
        // Continue with reduced functionality if graceful degradation is enabled
        bool gracefulDegradation = m_configParser ? 
            m_configParser->getBool("network_enforcement.graceful_degradation", true) : true;
        if (!gracefulDegradation) {
            LOG_ERROR("Graceful degradation disabled, initialization failed");
            return false;
        }
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
            LOG_ERROR("Failed to create packet capture thread");
            return false;
        }
        m_captureThread = reinterpret_cast<void*>(thread);
    }

    m_running = true;
    LOG_INFO("Network enforcement started");
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
    LOG_INFO("Network enforcement stopped");
}

bool NetworkEnforcement::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check network hook status
    if (m_pcapHandle == nullptr) {
        // If graceful degradation is enabled, this is acceptable
        bool gracefulDegradation = m_configParser ? 
            m_configParser->getBool("network_enforcement.graceful_degradation", true) : true;
        if (!gracefulDegradation) {
            return false;
        }
    }
    
    // Check if capture thread is running (if hooks are available)
    if (m_pcapHandle != nullptr && !m_captureRunning) {
        return false;
    }
    
    return true;
}

bool NetworkEnforcement::initializeNetworkHooks() {
    char errbuf[PCAP_ERRBUF_SIZE];
    
    // Try to find a network interface
    char* dev = pcap_lookupdev(errbuf);
    if (dev == nullptr) {
        LOG_ERROR(QString("Could not find network device: %1").arg(errbuf));
        // Graceful degradation: continue without packet capture
        return false;
    }

    // Open device for packet capture
    m_pcapHandle = pcap_open_live(dev, 65535, 1, 1000, errbuf);
    if (m_pcapHandle == nullptr) {
        LOG_ERROR(QString("Could not open device %1: %2").arg(dev).arg(errbuf));
        // Graceful degradation: continue without packet capture
        return false;
    }

    // Set non-blocking mode
    if (pcap_setnonblock(reinterpret_cast<pcap_t*>(m_pcapHandle), 1, errbuf) < 0) {
        LOG_ERROR(QString("Could not set non-blocking mode: %1").arg(errbuf));
        pcap_close(reinterpret_cast<pcap_t*>(m_pcapHandle));
        m_pcapHandle = nullptr;
        return false;
    }

    LOG_INFO(QString("Network hooks initialized on device: %1").arg(dev));
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
            // Extract packet information for rule matching
            QString source, destination, protocol;
            int port = -1;
            extractPacketInfo(packetData, packetSize, source, destination, protocol, port);
            
            // Check if rule matches packet
            bool ruleMatches = true;
            
            // Match source (CIDR notation)
            if (!rule.source.empty() && !source.isEmpty()) {
                if (!matchCIDR(source.toStdString(), rule.source)) {
                    ruleMatches = false;
                }
            }
            
            // Match destination (CIDR notation)
            if (ruleMatches && !rule.destination.empty() && !destination.isEmpty()) {
                if (!matchCIDR(destination.toStdString(), rule.destination)) {
                    ruleMatches = false;
                }
            }
            
            // Match protocol
            if (ruleMatches && !rule.protocol.empty() && !protocol.isEmpty()) {
                if (protocol.toUpper().toStdString() != rule.protocol) {
                    ruleMatches = false;
                }
            }
            
            // Match port
            if (ruleMatches && rule.port != -1 && port != -1) {
                if (port != rule.port) {
                    ruleMatches = false;
                }
            }
            
            if (ruleMatches) {
                // Check encryption requirement
                if (rule.encryption_required && !encrypted) {
                    // Encryption required but packet is not encrypted
                    if (rule.action == "BLOCK") {
                        m_blockedCount++;
                        return false;  // Block packet
                } else if (rule.action == "LOG") {
                    // Log but allow
                    if (m_auditLogger) {
                        QString source, destination, protocol;
                        int port = -1;
                        extractPacketInfo(packetData, packetSize, source, destination, protocol, port);
                        m_auditLogger->logPolicyViolation(
                            QString::fromStdString(policy.policy_id),
                            "encryption_required_violation",
                            QVariantMap()
                        );
                    }
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

void NetworkEnforcement::extractPacketInfo(const void* packetData, size_t packetSize,
                                           QString& source, QString& destination, QString& protocol, int& port) {
    source = "unknown";
    destination = "unknown";
    protocol = "unknown";
    port = -1;
    
    if (!packetData || packetSize < 14) {
        return;  // Not enough data for Ethernet header
    }
    
    const unsigned char* data = static_cast<const unsigned char*>(packetData);
    
    // Skip Ethernet header (14 bytes) to get IP header
    const unsigned char* ipHeader = data + 14;
    size_t ipHeaderSize = packetSize - 14;
    
    if (ipHeaderSize < 1) {
        return;
    }
    
    // Check for IP version (IPv4 = 0x45, IPv6 = 0x60)
    if ((ipHeader[0] & 0xF0) == 0x40 && ipHeaderSize >= 20) {
        // IPv4
        struct in_addr srcAddr, dstAddr;
        memcpy(&srcAddr, ipHeader + 12, 4);
        memcpy(&dstAddr, ipHeader + 16, 4);
        
        char srcStr[INET_ADDRSTRLEN], dstStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &srcAddr, srcStr, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &dstAddr, dstStr, INET_ADDRSTRLEN);
        
        source = QString::fromUtf8(srcStr);
        destination = QString::fromUtf8(dstStr);
        
        // Get protocol field (offset 9 in IP header)
        unsigned char ipProtocol = ipHeader[9];
        size_t ipHeaderLength = (ipHeader[0] & 0x0F) * 4;
        
        if (ipProtocol == 6) {
            // TCP
            protocol = "TCP";
            if (ipHeaderSize >= ipHeaderLength + 4) {
                const unsigned char* tcpHeader = ipHeader + ipHeaderLength;
                port = (tcpHeader[0] << 8) | tcpHeader[1];  // Source port
                // Could also extract destination port: (tcpHeader[2] << 8) | tcpHeader[3]
            }
        } else if (ipProtocol == 17) {
            // UDP
            protocol = "UDP";
            if (ipHeaderSize >= ipHeaderLength + 4) {
                const unsigned char* udpHeader = ipHeader + ipHeaderLength;
                port = (udpHeader[0] << 8) | udpHeader[1];  // Source port
            }
        } else if (ipProtocol == 1) {
            protocol = "ICMP";
        } else {
            protocol = QString("IP-%1").arg(ipProtocol);
        }
    } else if ((ipHeader[0] & 0xF0) == 0x60 && ipHeaderSize >= 40) {
        // IPv6 (simplified - just mark as IPv6)
        protocol = "IPv6";
        source = "::";
        destination = "::";
    }
}

bool NetworkEnforcement::matchCIDR(const std::string& ip, const std::string& cidr) {
    if (cidr.empty() || ip.empty()) {
        return true;  // Empty rule matches all
    }
    
    // Parse CIDR notation (e.g., "192.168.1.0/24")
    size_t slashPos = cidr.find('/');
    if (slashPos == std::string::npos) {
        // No prefix length, exact match
        return ip == cidr;
    }
    
    std::string cidrIp = cidr.substr(0, slashPos);
    int prefixLen = std::stoi(cidr.substr(slashPos + 1));
    
    if (prefixLen < 0 || prefixLen > 32) {
        return false;
    }
    
    // Convert IPs to network byte order
    struct in_addr ipAddr, cidrAddr;
    if (inet_aton(ip.c_str(), &ipAddr) == 0 || inet_aton(cidrIp.c_str(), &cidrAddr) == 0) {
        return false;
    }
    
    uint32_t ipNet = ntohl(ipAddr.s_addr);
    uint32_t cidrNet = ntohl(cidrAddr.s_addr);
    uint32_t mask = (0xFFFFFFFF << (32 - prefixLen)) & 0xFFFFFFFF;
    
    return (ipNet & mask) == (cidrNet & mask);
}

std::vector<std::string> NetworkEnforcement::getBlockedTransmissions() const {
    std::lock_guard<std::mutex> lock(m_blockedMutex);
    std::vector<std::string> result;
    
    for (const auto& blocked : m_blockedTransmissions) {
        QJsonObject obj;
        obj["source"] = QString::fromStdString(blocked.source);
        obj["destination"] = QString::fromStdString(blocked.destination);
        obj["protocol"] = QString::fromStdString(blocked.protocol);
        obj["port"] = blocked.port;
        obj["timestamp"] = QString::fromStdString(blocked.timestamp);
        obj["reason"] = QString::fromStdString(blocked.reason);
        
        QJsonDocument doc(obj);
        result.push_back(QString::fromUtf8(doc.toJson()).toStdString());
    }
    
    return result;
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
            LOG_ERROR(QString("Error reading packet: %1").arg(pcap_geterr(handle)));
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
    
    // Extract packet information for logging
    QString source, destination, protocol;
    int port = -1;
    extractPacketInfo(packet, hdr->caplen, source, destination, protocol, port);
    
    // Inspect packet
    bool encrypted = isPacketEncrypted(packet, hdr->caplen);
    bool allowed = inspectPacket(packet, hdr->caplen);
    
    if (!allowed) {
        // Store blocked transmission (thread-safe)
        {
            std::lock_guard<std::mutex> lock(m_blockedMutex);
            BlockedTransmission blocked;
            blocked.source = source.toStdString();
            blocked.destination = destination.toStdString();
            blocked.protocol = protocol.toStdString();
            blocked.port = port;
            blocked.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
            blocked.reason = encrypted ? "policy_violation" : "encryption_required";
            
            m_blockedTransmissions.push_back(blocked);
            if (m_blockedTransmissions.size() > MAX_BLOCKED_HISTORY) {
                m_blockedTransmissions.erase(m_blockedTransmissions.begin());
            }
        }
        
        // Packet blocked - log to audit service
        if (m_auditLogger) {
            m_auditLogger->logTransmissionAttempt(source, destination, protocol, encrypted, "BLOCK");
        }
        LOG_INFO(QString("Packet blocked: %1 -> %2 (%3, size=%4)")
                 .arg(source).arg(destination).arg(protocol).arg(hdr->caplen));
    } else {
        // Packet allowed - log to audit service (optional, can be configured)
        if (m_auditLogger) {
            // Only log if configured to log allowed transmissions
            // For now, only log blocked transmissions
        }
    }
}

