#ifndef TPM_DEVICE_H
#define TPM_DEVICE_H

#include <string>
#include <vector>
#include <memory>

/**
 * @brief TPM Device Status
 */
enum class TPMStatus {
    UNAVAILABLE,
    AVAILABLE,
    INITIALIZED,
    ERROR
};

/**
 * @brief TPM Device Information
 */
struct TPMDeviceInfo {
    std::string device_path;
    std::string tpm_version;
    std::string manufacturer;
    std::string firmware_version;
    TPMStatus status;
    bool secure_boot_supported;
};

/**
 * @brief PCR (Platform Configuration Register) Value
 */
struct PCRValue {
    uint32_t pcr_index;
    std::vector<uint8_t> value;
    std::string algorithm;  // e.g., "sha256"
};

/**
 * @brief TPM Device
 * 
 * Provides TPM 2.0 device abstraction and communication.
 */
class TPMDevice {
public:
    TPMDevice();
    ~TPMDevice();

    /**
     * @brief Initialize TPM device
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if TPM device is available
     * @return true if available, false otherwise
     */
    bool isAvailable() const { return m_status == TPMStatus::INITIALIZED; }

    /**
     * @brief Get TPM device information
     * @return Device information
     */
    TPMDeviceInfo getDeviceInfo() const { return m_deviceInfo; }

    /**
     * @brief Get PCR values
     * @param pcrIndices List of PCR indices to read
     * @return List of PCR values
     */
    std::vector<PCRValue> getPCRValues(const std::vector<uint32_t>& pcrIndices);

    /**
     * @brief Get boot measurement (PCR 0-7 typically)
     * @return List of PCR values for boot measurement
     */
    std::vector<PCRValue> getBootMeasurement();

    /**
     * @brief Get TPM status
     * @return Status string
     */
    std::string getStatus() const;

private:
    TPMStatus m_status;
    TPMDeviceInfo m_deviceInfo;
    void* m_tpmContext;  // TSS2_ESYS_CONTEXT pointer

    /**
     * @brief Detect TPM device
     * @return true if device found, false otherwise
     */
    bool detectDevice();

    /**
     * @brief Initialize TSS2 context
     * @return true if successful, false otherwise
     */
    bool initializeTSS2();

    /**
     * @brief Cleanup TSS2 context
     */
    void cleanupTSS2();
};

#endif // TPM_DEVICE_H

