#include "secure_boot_manager.h"
#include "tpm_device.h"
#include "tpm_attestation.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <iostream>

SecureBootManager::SecureBootManager()
    : m_initialized(false)
    , m_tpmDevice(nullptr)
    , m_attestation(nullptr)
{
    initializeDefaultPolicy();
}

SecureBootManager::~SecureBootManager() {
}

bool SecureBootManager::initialize(TPMDevice* tpmDevice, TPMAttestation* attestation) {
    if (m_initialized) {
        return true;
    }

    if (!tpmDevice || !tpmDevice->isAvailable()) {
        return false;
    }

    m_tpmDevice = tpmDevice;
    m_attestation = attestation;

    // Collect initial boot measurement
    m_currentMeasurement = collectBootMeasurement();

    m_initialized = true;
    return true;
}

void SecureBootManager::initializeDefaultPolicy() {
    m_defaultPolicy.policy_id = "default";
    m_defaultPolicy.required_pcrs = {0, 1, 2, 3, 4, 5, 6, 7};  // Boot measurement PCRs
    m_defaultPolicy.enforce_policy = true;
    m_defaultPolicy.allow_pcr_changes = false;
    m_defaultPolicy.description = "Default secure boot policy";
    m_policies["default"] = m_defaultPolicy;
}

BootMeasurement SecureBootManager::collectBootMeasurement(const std::vector<uint32_t>& pcrIndices) {
    BootMeasurement measurement;
    measurement.is_valid = false;

    if (!m_tpmDevice || !m_tpmDevice->isAvailable()) {
        return measurement;
    }

    // Use provided PCR indices or default boot PCRs
    std::vector<uint32_t> pcrs = pcrIndices.empty() ? std::vector<uint32_t>{0, 1, 2, 3, 4, 5, 6, 7} : pcrIndices;
    measurement.pcr_indices = pcrs;

    // Get PCR values from TPM device
    auto pcrValues = m_tpmDevice->getPCRValues(pcrs);
    
    for (const auto& pcr : pcrValues) {
        measurement.pcr_values[pcr.pcr_index] = pcr.value;
        if (measurement.algorithm.empty()) {
            measurement.algorithm = pcr.algorithm;
        }
    }

    // Format measurement time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    measurement.measured_at = timeStr;

    measurement.is_valid = !measurement.pcr_values.empty();
    
    // Update current measurement
    m_currentMeasurement = measurement;

    return measurement;
}

std::string SecureBootManager::generateBootAttestation(const std::string& nonce) {
    if (!m_attestation || !m_initialized) {
        return "";
    }

    // Use current boot measurement PCRs
    std::vector<uint32_t> pcrIndices = m_currentMeasurement.pcr_indices;
    if (pcrIndices.empty()) {
        pcrIndices = {0, 1, 2, 3, 4, 5, 6, 7};
    }

    // Generate attestation quote
    auto quote = m_attestation->generateAttestation(nonce, pcrIndices);
    
    // Format attestation as JSON (simplified)
    std::ostringstream oss;
    oss << "{\"quote_data\":\"" << quote.quote_data
        << "\",\"signature\":\"" << quote.signature
        << "\",\"nonce\":\"" << quote.nonce
        << "\",\"created_at\":\"" << quote.created_at << "\"}";
    
    return oss.str();
}

BootIntegrityResult SecureBootManager::verifyBootIntegrity(const SecureBootPolicy& policy) {
    BootIntegrityResult result;
    result.is_valid = false;
    result.policy_compliant = false;

    if (!m_initialized || !m_tpmDevice || !m_tpmDevice->isAvailable()) {
        result.error_message = "TPM device not available";
        return result;
    }

    // Collect current boot measurement
    BootMeasurement currentMeasurement = collectBootMeasurement(policy.required_pcrs);
    if (!currentMeasurement.is_valid) {
        result.error_message = "Failed to collect boot measurement";
        return result;
    }

    // Compare with expected PCR values if policy has them
    if (!policy.expected_pcr_values.empty()) {
        result.mismatched_pcrs = comparePCRValues(policy.expected_pcr_values, currentMeasurement.pcr_values);
        
        if (!result.mismatched_pcrs.empty() && !policy.allow_pcr_changes) {
            result.error_message = "PCR values do not match policy";
            result.is_valid = false;
            result.policy_compliant = false;
            
            // Format verification time
            char timeStr[64];
            std::time_t now = std::time(nullptr);
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            result.verification_time = timeStr;
            
            return result;
        }
    }

    // If we have a baseline, compare with it
    if (m_baselineMeasurement.is_valid) {
        result.mismatched_pcrs = comparePCRValues(m_baselineMeasurement.pcr_values, currentMeasurement.pcr_values);
        
        if (!result.mismatched_pcrs.empty() && !policy.allow_pcr_changes) {
            result.error_message = "Boot measurement differs from baseline";
            result.is_valid = false;
            result.policy_compliant = false;
            
            char timeStr[64];
            std::time_t now = std::time(nullptr);
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            result.verification_time = timeStr;
            
            return result;
        }
    }

    // Verification passed
    result.is_valid = true;
    result.policy_compliant = true;
    
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    result.verification_time = timeStr;

    return result;
}

BootIntegrityResult SecureBootManager::verifyBootIntegrity() {
    return verifyBootIntegrity(m_defaultPolicy);
}

void SecureBootManager::setSecureBootPolicy(const SecureBootPolicy& policy) {
    m_policies[policy.policy_id] = policy;
    if (policy.policy_id == "default") {
        m_defaultPolicy = policy;
    }
}

SecureBootPolicy SecureBootManager::getSecureBootPolicy(const std::string& policyId) const {
    if (policyId.empty()) {
        return m_defaultPolicy;
    }

    auto it = m_policies.find(policyId);
    if (it != m_policies.end()) {
        return it->second;
    }

    return SecureBootPolicy();  // Return empty policy if not found
}

bool SecureBootManager::isSecureBootEnabled() const {
    // Check UEFI secure boot status
    return checkUEFISecureBoot();
}

BootMeasurement SecureBootManager::getCurrentBootMeasurement() const {
    return m_currentMeasurement;
}

bool SecureBootManager::storeBootBaseline(const BootMeasurement& measurement) {
    if (!measurement.is_valid) {
        return false;
    }

    m_baselineMeasurement = measurement;

    // Store baseline to file (in production, use secure storage)
    std::string baselinePath = "/var/lib/milos/tpm/boot_baseline.json";
    std::filesystem::create_directories(std::filesystem::path(baselinePath).parent_path());

    std::ofstream file(baselinePath);
    if (!file.is_open()) {
        return false;
    }

    // Write baseline as JSON (simplified)
    file << "{\"measured_at\":\"" << measurement.measured_at
         << "\",\"algorithm\":\"" << measurement.algorithm << "\",\"pcr_values\":{";
    
    bool first = true;
    for (const auto& [pcrIndex, value] : measurement.pcr_values) {
        if (!first) file << ",";
        file << "\"" << pcrIndex << "\":\"";
        for (uint8_t byte : value) {
            file << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        file << "\"";
        first = false;
    }
    
    file << "}}";
    file.close();

    return true;
}

BootMeasurement SecureBootManager::getBootBaseline() const {
    return m_baselineMeasurement;
}

bool SecureBootManager::checkUEFISecureBoot() const {
    // Check if UEFI secure boot is enabled
    // This is typically done by checking /sys/firmware/efi/efivars/SecureBoot-*
    // or using efivar library
    
    // Check for EFI variables
    std::string efiVarsPath = "/sys/firmware/efi/efivars";
    if (!std::filesystem::exists(efiVarsPath)) {
        return false;  // Not UEFI system
    }

    // Try to find SecureBoot variable
    // In production, use efivar library to read SecureBoot-8be4df61-93ca-11d2-aa0d-00e098032b8c
    // For now, check if the directory exists
    std::string secureBootVar = efiVarsPath + "/SecureBoot-8be4df61-93ca-11d2-aa0d-00e098032b8c";
    if (std::filesystem::exists(secureBootVar)) {
        // Read the value (first 4 bytes are attributes, 5th byte is the value)
        std::ifstream file(secureBootVar, std::ios::binary);
        if (file.is_open()) {
            file.seekg(4);  // Skip attributes
            uint8_t value = 0;
            file.read(reinterpret_cast<char*>(&value), 1);
            file.close();
            return value == 1;
        }
    }

    return false;
}

std::vector<uint32_t> SecureBootManager::comparePCRValues(
    const std::map<uint32_t, std::vector<uint8_t>>& expected,
    const std::map<uint32_t, std::vector<uint8_t>>& actual) const {
    
    std::vector<uint32_t> mismatched;

    for (const auto& [pcrIndex, expectedValue] : expected) {
        auto it = actual.find(pcrIndex);
        if (it == actual.end()) {
            mismatched.push_back(pcrIndex);
            continue;
        }

        if (expectedValue != it->second) {
            mismatched.push_back(pcrIndex);
        }
    }

    return mismatched;
}

