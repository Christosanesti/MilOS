#ifndef WIRESHARK_INTEGRATION_H
#define WIRESHARK_INTEGRATION_H

#include <string>
#include <vector>
#include <memory>

#ifdef HAVE_WIRESHARK
// Wireshark API includes would go here
// #include <wireshark/epan/packet.h>
// #include <wireshark/epan/epan.h>
#endif

/**
 * @brief Wireshark API Version Information
 */
struct WiresharkVersion {
    int major;
    int minor;
    int patch;
    std::string version_string;
    bool is_compatible;
};

/**
 * @brief Wireshark Integration
 * 
 * Provides integration with Wireshark API for packet capture and analysis.
 * Handles version detection and backward compatibility.
 */
class WiresharkIntegration {
public:
    WiresharkIntegration();
    ~WiresharkIntegration();

    /**
     * @brief Initialize Wireshark API
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get Wireshark version information
     * @return Version information
     */
    WiresharkVersion getVersion() const;

    /**
     * @brief Check if Wireshark API is available
     * @return true if available, false otherwise
     */
    bool isAvailable() const { return m_available; }

    /**
     * @brief Check if Wireshark API is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

    /**
     * @brief Cleanup Wireshark API
     */
    void cleanup();

private:
    bool m_available;
    bool m_initialized;
    WiresharkVersion m_version;

    /**
     * @brief Detect Wireshark version
     * @return true if detection successful, false otherwise
     */
    bool detectVersion();

    /**
     * @brief Check version compatibility
     * @param version Version to check
     * @return true if compatible, false otherwise
     */
    bool checkCompatibility(const WiresharkVersion& version) const;
};

#endif // WIRESHARK_INTEGRATION_H

