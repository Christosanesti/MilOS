#include "dbus_interface.h"
#include "tpm_service.h"
#include "tpm_device.h"
#include "tpm_key_manager.h"
#include "tpm_attestation.h"
#include "secure_boot_manager.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_tpmService(nullptr)
    , m_tpmDevice(nullptr)
    , m_keyManager(nullptr)
    , m_attestation(nullptr)
    , m_secureBootManager(nullptr)
{
}

DBusInterface::~DBusInterface() {
}

bool DBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    // Register D-Bus service
    if (!QDBusConnection::systemBus().registerService("org.milos.TPM")) {
        std::cerr << "Failed to register D-Bus service" << std::endl;
        return false;
    }

    // Register D-Bus object
    if (!QDBusConnection::systemBus().registerObject("/org/milos/TPM", this,
                                                      QDBusConnection::ExportAllContents)) {
        std::cerr << "Failed to register D-Bus object" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void DBusInterface::setTPMService(TPMService* service) {
    m_tpmService = service;
}

void DBusInterface::setTPMDevice(TPMDevice* device) {
    m_tpmDevice = device;
}

void DBusInterface::setKeyManager(TPMKeyManager* manager) {
    m_keyManager = manager;
}

void DBusInterface::setAttestation(TPMAttestation* attestation) {
    m_attestation = attestation;
}

void DBusInterface::setSecureBootManager(SecureBootManager* secureBootManager) {
    m_secureBootManager = secureBootManager;
}

QString DBusInterface::GetTPMStatus() {
    QJsonObject status;

    if (!m_tpmDevice) {
        status["available"] = false;
        status["error"] = "TPM device not initialized";
        return QJsonDocument(status).toJson();
    }

    auto deviceInfo = m_tpmDevice->getDeviceInfo();
    status["available"] = m_tpmDevice->isAvailable();
    status["device_path"] = QString::fromStdString(deviceInfo.device_path);
    status["tpm_version"] = QString::fromStdString(deviceInfo.tpm_version);
    status["status"] = QString::fromStdString(m_tpmDevice->getStatus());
    status["secure_boot_supported"] = deviceInfo.secure_boot_supported;

    if (m_tpmService && m_tpmService->getHealthMonitor()) {
        auto healthStatus = m_tpmService->getHealthMonitor()->getHealthStatus();
        status["health"] = healthStatus.is_healthy;
        status["health_message"] = QString::fromStdString(healthStatus.status_message);
    }

    return QJsonDocument(status).toJson();
}

QString DBusInterface::GenerateKey(const QString& keyParams) {
    QJsonObject result;

    if (!m_keyManager) {
        result["success"] = false;
        result["error"] = "Key manager not initialized";
        return QJsonDocument(result).toJson();
    }

    // Parse key parameters
    QJsonDocument doc = QJsonDocument::fromJson(keyParams.toUtf8());
    if (!doc.isObject()) {
        result["success"] = false;
        result["error"] = "Invalid key parameters";
        return QJsonDocument(result).toJson();
    }

    QJsonObject params = doc.object();
    KeyGenerationParams keyGenParams;
    keyGenParams.key_type = params["key_type"].toString("RSA2048").toStdString();
    keyGenParams.key_usage = params["key_usage"].toString("signing").toStdString();
    keyGenParams.persistent = params["persistent"].toBool(false);
    keyGenParams.key_auth = params["key_auth"].toString().toStdString();

    auto keyInfo = m_keyManager->generateKey(keyGenParams);

    if (keyInfo.key_id.empty()) {
        result["success"] = false;
        result["error"] = "Key generation failed";
        return QJsonDocument(result).toJson();
    }

    result["success"] = true;
    result["key_id"] = QString::fromStdString(keyInfo.key_id);
    result["key_type"] = QString::fromStdString(keyInfo.key_type);
    result["persistent_handle"] = static_cast<qint64>(keyInfo.persistent_handle);
    result["created_at"] = QString::fromStdString(keyInfo.created_at);

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetBootMeasurement() {
    QJsonObject result;

    if (!m_tpmDevice) {
        result["success"] = false;
        result["error"] = "TPM device not initialized";
        return QJsonDocument(result).toJson();
    }

    auto pcrValues = m_tpmDevice->getBootMeasurement();

    QJsonArray pcrArray;
    for (const auto& pcr : pcrValues) {
        QJsonObject pcrObj;
        pcrObj["pcr_index"] = static_cast<int>(pcr.pcr_index);
        pcrObj["algorithm"] = QString::fromStdString(pcr.algorithm);
        
        // Convert value to hex string
        QString hexValue;
        for (uint8_t byte : pcr.value) {
            hexValue += QString::number(byte, 16).rightJustified(2, '0');
        }
        pcrObj["value"] = hexValue;
        
        pcrArray.append(pcrObj);
    }

    result["success"] = true;
    result["pcr_values"] = pcrArray;

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GenerateAttestation(const QString& nonce) {
    QJsonObject result;

    if (!m_attestation) {
        result["success"] = false;
        result["error"] = "Attestation not initialized";
        return QJsonDocument(result).toJson();
    }

    std::string nonceStr = nonce.isEmpty() ? "" : nonce.toStdString();
    auto quote = m_attestation->generateAttestation(nonceStr);

    result["success"] = true;
    result["quote_data"] = QString::fromStdString(quote.quote_data);
    result["signature"] = QString::fromStdString(quote.signature);
    result["nonce"] = QString::fromStdString(quote.nonce);
    result["algorithm"] = QString::fromStdString(quote.algorithm);
    result["created_at"] = QString::fromStdString(quote.created_at);

    QJsonArray pcrArray;
    for (uint32_t pcrIndex : quote.pcr_indices) {
        pcrArray.append(static_cast<int>(pcrIndex));
    }
    result["pcr_indices"] = pcrArray;

    return QJsonDocument(result).toJson();
}

QString DBusInterface::VerifyAttestation(const QString& quote) {
    QJsonObject result;

    if (!m_attestation) {
        result["success"] = false;
        result["error"] = "Attestation not initialized";
        return QJsonDocument(result).toJson();
    }

    // Parse quote
    QJsonDocument doc = QJsonDocument::fromJson(quote.toUtf8());
    if (!doc.isObject()) {
        result["success"] = false;
        result["error"] = "Invalid quote format";
        return QJsonDocument(result).toJson();
    }

    QJsonObject quoteObj = doc.object();
    AttestationQuote attestationQuote;
    attestationQuote.quote_data = quoteObj["quote_data"].toString().toStdString();
    attestationQuote.signature = quoteObj["signature"].toString().toStdString();
    attestationQuote.nonce = quoteObj["nonce"].toString().toStdString();
    attestationQuote.algorithm = quoteObj["algorithm"].toString("sha256").toStdString();

    QJsonArray pcrArray = quoteObj["pcr_indices"].toArray();
    for (const QJsonValue& value : pcrArray) {
        attestationQuote.pcr_indices.push_back(value.toInt());
    }

    auto verificationResult = m_attestation->verifyAttestation(attestationQuote);

    result["success"] = verificationResult.success;
    result["is_valid"] = verificationResult.is_valid;
    result["error_message"] = QString::fromStdString(verificationResult.error_message);
    result["verified_at"] = QString::fromStdString(verificationResult.verified_at);

    return QJsonDocument(result).toJson();
}

QString DBusInterface::VerifyBootIntegrity(const QString& policyId) {
    QJsonObject result;

    if (!m_secureBootManager) {
        result["success"] = false;
        result["error"] = "Secure boot manager not initialized";
        return QJsonDocument(result).toJson();
    }

    std::string policyIdStr = policyId.isEmpty() ? "" : policyId.toStdString();
    auto integrityResult = m_secureBootManager->verifyBootIntegrity(
        m_secureBootManager->getSecureBootPolicy(policyIdStr));

    result["success"] = true;
    result["is_valid"] = integrityResult.is_valid;
    result["policy_compliant"] = integrityResult.policy_compliant;
    result["verification_time"] = QString::fromStdString(integrityResult.verification_time);
    result["error_message"] = QString::fromStdString(integrityResult.error_message);

    QJsonArray mismatchedArray;
    for (uint32_t pcrIndex : integrityResult.mismatched_pcrs) {
        mismatchedArray.append(static_cast<int>(pcrIndex));
    }
    result["mismatched_pcrs"] = mismatchedArray;

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetSecureBootStatus() {
    QJsonObject result;

    if (!m_secureBootManager) {
        result["success"] = false;
        result["error"] = "Secure boot manager not initialized";
        return QJsonDocument(result).toJson();
    }

    result["success"] = true;
    result["secure_boot_enabled"] = m_secureBootManager->isSecureBootEnabled();
    
    auto currentMeasurement = m_secureBootManager->getCurrentBootMeasurement();
    result["measurement_valid"] = currentMeasurement.is_valid;
    result["measured_at"] = QString::fromStdString(currentMeasurement.measured_at);
    result["algorithm"] = QString::fromStdString(currentMeasurement.algorithm);

    QJsonArray pcrArray;
    for (const auto& [pcrIndex, value] : currentMeasurement.pcr_values) {
        QJsonObject pcrObj;
        pcrObj["pcr_index"] = static_cast<int>(pcrIndex);
        
        QString hexValue;
        for (uint8_t byte : value) {
            hexValue += QString::number(byte, 16).rightJustified(2, '0');
        }
        pcrObj["value"] = hexValue;
        pcrArray.append(pcrObj);
    }
    result["pcr_values"] = pcrArray;

    auto baseline = m_secureBootManager->getBootBaseline();
    result["baseline_stored"] = baseline.is_valid;

    return QJsonDocument(result).toJson();
}

QString DBusInterface::StoreBootBaseline() {
    QJsonObject result;

    if (!m_secureBootManager) {
        result["success"] = false;
        result["error"] = "Secure boot manager not initialized";
        return QJsonDocument(result).toJson();
    }

    auto measurement = m_secureBootManager->getCurrentBootMeasurement();
    if (!measurement.is_valid) {
        result["success"] = false;
        result["error"] = "Current boot measurement is not valid";
        return QJsonDocument(result).toJson();
    }

    bool stored = m_secureBootManager->storeBootBaseline(measurement);
    result["success"] = stored;
    if (!stored) {
        result["error"] = "Failed to store boot baseline";
    } else {
        result["message"] = "Boot baseline stored successfully";
        result["measured_at"] = QString::fromStdString(measurement.measured_at);
    }

    return QJsonDocument(result).toJson();
}

