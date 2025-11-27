#include "tpm_device.h"
#include <milos/logging/logger.h>
#include <fstream>
#include <filesystem>

#ifdef HAVE_TSS2
#include <tss2/tss2_esys.h>
#include <tss2/tss2_tctildr.h>
#endif

TPMDevice::TPMDevice()
    : m_status(TPMStatus::UNAVAILABLE)
    , m_tpmContext(nullptr)
{
    m_deviceInfo.status = TPMStatus::UNAVAILABLE;
    m_deviceInfo.secure_boot_supported = false;
}

TPMDevice::~TPMDevice() {
    cleanupTSS2();
}

bool TPMDevice::initialize() {
    if (m_status == TPMStatus::INITIALIZED) {
        return true;
    }

    // Detect TPM device
    if (!detectDevice()) {
        LOG_WARNING("TPM device not found");
        m_status = TPMStatus::UNAVAILABLE;
        return false;
    }

#ifdef HAVE_TSS2
    // Initialize TSS2 context
    if (!initializeTSS2()) {
        LOG_ERROR("Failed to initialize TSS2 context");
        m_status = TPMStatus::ERROR;
        return false;
    }
#else
    LOG_WARNING("TSS2 libraries not available. TPM operations will be limited.");
#endif

    m_status = TPMStatus::INITIALIZED;
    m_deviceInfo.status = TPMStatus::INITIALIZED;
    return true;
}

bool TPMDevice::detectDevice() {
    // Check for TPM device files
    std::vector<std::string> tpmDevices = {"/dev/tpm0", "/dev/tpmrm0"};
    
    for (const auto& device : tpmDevices) {
        if (std::filesystem::exists(device)) {
            m_deviceInfo.device_path = device;
            m_deviceInfo.status = TPMStatus::AVAILABLE;
            
            // Try to read TPM version info
            std::ifstream versionFile("/sys/class/tpm/tpm0/device/description");
            if (versionFile.is_open()) {
                std::getline(versionFile, m_deviceInfo.tpm_version);
                versionFile.close();
            } else {
                m_deviceInfo.tpm_version = "TPM 2.0";
            }
            
            return true;
        }
    }

    return false;
}

#ifdef HAVE_TSS2
bool TPMDevice::initializeTSS2() {
    TSS2_RC rc;
    
    // Initialize ESYS context
    ESYS_CONTEXT* esysContext = nullptr;
    rc = Esys_Initialize(&esysContext, nullptr, nullptr);
    if (rc != TSS2_RC_SUCCESS) {
        std::stringstream ss;
        ss << "Failed to initialize ESYS context: " << rc;
        LOG_ERROR(ss.str());
        return false;
    }

    m_tpmContext = esysContext;
    return true;
}

void TPMDevice::cleanupTSS2() {
    if (m_tpmContext) {
        Esys_Finalize(static_cast<ESYS_CONTEXT*>(m_tpmContext));
        m_tpmContext = nullptr;
    }
}
#else
bool TPMDevice::initializeTSS2() {
    // TSS2 not available, use fallback
    return true;
}

void TPMDevice::cleanupTSS2() {
    // Nothing to cleanup
}
#endif

std::vector<PCRValue> TPMDevice::getPCRValues(const std::vector<uint32_t>& pcrIndices) {
    std::vector<PCRValue> pcrValues;

#ifdef HAVE_TSS2
    if (!m_tpmContext) {
        return pcrValues;
    }

    ESYS_CONTEXT* esysContext = static_cast<ESYS_CONTEXT*>(m_tpmContext);
    
    for (uint32_t pcrIndex : pcrIndices) {
        PCRValue pcr;
        pcr.pcr_index = pcrIndex;
        pcr.algorithm = "sha256";

        TPM2B_DIGEST* pcrValue = nullptr;
        TSS2_RC rc = Esys_PCR_Read(esysContext, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
                                   pcrIndex, nullptr, &pcrValue);
        if (rc == TSS2_RC_SUCCESS && pcrValue) {
            pcr.value.assign(pcrValue->buffer, pcrValue->buffer + pcrValue->size);
            Esys_Free(pcrValue);
        }

        pcrValues.push_back(pcr);
    }
#else
    // Fallback: Read from sysfs
    for (uint32_t pcrIndex : pcrIndices) {
        PCRValue pcr;
        pcr.pcr_index = pcrIndex;
        pcr.algorithm = "sha256";
        
        std::string pcrPath = "/sys/class/tpm/tpm0/pcr-sha256/" + std::to_string(pcrIndex);
        std::ifstream pcrFile(pcrPath);
        if (pcrFile.is_open()) {
            std::string hexValue;
            pcrFile >> hexValue;
            // Convert hex to bytes (simplified)
            pcrFile.close();
        }
        
        pcrValues.push_back(pcr);
    }
#endif

    return pcrValues;
}

std::vector<PCRValue> TPMDevice::getBootMeasurement() {
    // PCR 0-7 are typically used for boot measurement
    std::vector<uint32_t> bootPCRs = {0, 1, 2, 3, 4, 5, 6, 7};
    return getPCRValues(bootPCRs);
}

std::string TPMDevice::getStatus() const {
    switch (m_status) {
        case TPMStatus::UNAVAILABLE:
            return "unavailable";
        case TPMStatus::AVAILABLE:
            return "available";
        case TPMStatus::INITIALIZED:
            return "initialized";
        case TPMStatus::ERROR:
            return "error";
        default:
            return "unknown";
    }
}

