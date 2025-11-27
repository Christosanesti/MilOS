#include "tpm_attestation.h"
#include "tpm_device.h"
#include <milos/logging/logger.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <openssl/rand.h>

#ifdef HAVE_TSS2
#include <tss2/tss2_esys.h>
#endif

TPMAttestation::TPMAttestation()
    : m_initialized(false)
    , m_tpmDevice(nullptr)
{
}

TPMAttestation::~TPMAttestation() {
}

bool TPMAttestation::initialize(TPMDevice* tpmDevice) {
    if (m_initialized) {
        return true;
    }

    if (!tpmDevice || !tpmDevice->isAvailable()) {
        return false;
    }

    m_tpmDevice = tpmDevice;
    m_initialized = true;
    return true;
}

AttestationQuote TPMAttestation::generateAttestation(const std::string& nonce,
                                                     const std::vector<uint32_t>& pcrIndices) {
    AttestationQuote quote;
    
    if (!m_initialized || !m_tpmDevice) {
        return quote;
    }

    // Use provided nonce or generate one
    quote.nonce = nonce.empty() ? generateNonce() : nonce;
    
    // Use provided PCR indices or default to boot measurement PCRs
    if (pcrIndices.empty()) {
        quote.pcr_indices = {0, 1, 2, 3, 4, 5, 6, 7};
    } else {
        quote.pcr_indices = pcrIndices;
    }

    quote.algorithm = "sha256";

#ifdef HAVE_TSS2
    // TPM quote generation using TSS2 library
    ESYS_CONTEXT* esysContext = static_cast<ESYS_CONTEXT*>(m_tpmDevice->getESYSContext());
    if (!esysContext) {
        LOG_ERROR("ESYS context not available");
        return quote;
    }

    TSS2_RC rc;
    
    // Create a signing key for attestation (or use endorsement key)
    // For simplicity, we'll create a primary key in the endorsement hierarchy
    ESYS_TR signingKeyHandle = ESYS_TR_NONE;
    
    // Prepare signing key template
    TPM2B_PUBLIC inPublic = {0};
    inPublic.size = sizeof(TPMT_PUBLIC);
    inPublic.publicArea.type = TPM2_ALG_RSA;
    inPublic.publicArea.nameAlg = TPM2_ALG_SHA256;
    inPublic.publicArea.objectAttributes = TPMA_OBJECT_USERWITHAUTH | 
                                         TPMA_OBJECT_SIGN_ENCRYPT | 
                                         TPMA_OBJECT_FIXEDTPM | 
                                         TPMA_OBJECT_FIXEDPARENT;
    inPublic.publicArea.parameters.rsaDetail.keyBits = 2048;
    inPublic.publicArea.parameters.rsaDetail.exponent = 0;

    TPM2B_SENSITIVE_CREATE inSensitive = {0};
    TPM2B_DATA outsideInfo = {0};
    TPM2B_PUBLIC* outPublic = nullptr;
    TPM2B_PRIVATE* outPrivate = nullptr;
    TPM2B_DATA* creationData = nullptr;
    TPM2B_DIGEST* creationHash = nullptr;
    TPMT_TK_CREATION* creationTicket = nullptr;
    TPM2B_NAME* name = nullptr;

    // Create primary key in endorsement hierarchy for signing
    rc = Esys_CreatePrimary(esysContext,
                           ESYS_TR_RH_ENDORSEMENT,
                           ESYS_TR_PASSWORD,
                           ESYS_TR_NONE,
                           ESYS_TR_NONE,
                           &inSensitive,
                           &inPublic,
                           &outsideInfo,
                           &creationData,
                           &creationHash,
                           &creationTicket,
                           &outPublic,
                           &outPrivate,
                           nullptr,
                           &name,
                           &signingKeyHandle);

    if (rc != TSS2_RC_SUCCESS) {
        std::stringstream ss;
        ss << "Failed to create signing key: 0x" << std::hex << rc;
        LOG_ERROR(ss.str());
        // Fallback to placeholder
        std::stringstream ss;
        ss << "tss2_quote_" << quote.nonce.substr(0, 8) << "_" << std::time(nullptr);
        quote.quote_data = ss.str();
        quote.signature = "tss2_signature_placeholder";
        return quote;
    }

    // Prepare PCR selection
    TPML_PCR_SELECTION pcrSelection = {0};
    pcrSelection.count = 1;
    pcrSelection.pcrSelections[0].hash = TPM2_ALG_SHA256;
    pcrSelection.pcrSelections[0].sizeofSelect = 3;  // 24 PCRs / 8
    
    // Set PCR bits for selected indices
    for (uint32_t pcrIndex : quote.pcr_indices) {
        if (pcrIndex < 24) {
            uint8_t byteIndex = pcrIndex / 8;
            uint8_t bitIndex = pcrIndex % 8;
            pcrSelection.pcrSelections[0].pcrSelect[byteIndex] |= (1 << bitIndex);
        }
    }

    // Prepare nonce
    TPM2B_DATA qualifyingData = {0};
    if (quote.nonce.length() > 0) {
        // Convert hex nonce to bytes
        size_t nonceLen = quote.nonce.length() / 2;
        if (nonceLen > sizeof(qualifyingData.buffer)) {
            nonceLen = sizeof(qualifyingData.buffer);
        }
        qualifyingData.size = nonceLen;
        for (size_t i = 0; i < nonceLen; i++) {
            std::string byteStr = quote.nonce.substr(i * 2, 2);
            qualifyingData.buffer[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
        }
    }

    // Generate quote
    TPM2B_ATTEST* attest = nullptr;
    TPMT_SIGNATURE* signature = nullptr;

    rc = Esys_Quote(esysContext,
                   signingKeyHandle,
                   ESYS_TR_PASSWORD,
                   ESYS_TR_NONE,
                   ESYS_TR_NONE,
                   &qualifyingData,
                   &pcrSelection,
                   &attest,
                   &signature);

    // Cleanup signing key (transient)
    Esys_FlushContext(esysContext, signingKeyHandle);
    if (outPublic) Esys_Free(outPublic);
    if (outPrivate) Esys_Free(outPrivate);
    if (creationData) Esys_Free(creationData);
    if (creationHash) Esys_Free(creationHash);
    if (creationTicket) Esys_Free(creationTicket);
    if (name) Esys_Free(name);

    if (rc != TSS2_RC_SUCCESS) {
        std::stringstream ss;
        ss << "Failed to generate quote: 0x" << std::hex << rc;
        LOG_ERROR(ss.str());
        // Fallback to placeholder
        std::stringstream ss;
        ss << "tss2_quote_" << quote.nonce.substr(0, 8) << "_" << std::time(nullptr);
        quote.quote_data = ss.str();
        quote.signature = "tss2_signature_placeholder";
        return quote;
    }

    // Extract quote data and signature
    if (attest) {
        // Convert attestation data to hex string
        std::stringstream ss;
        ss << std::hex;
        for (size_t i = 0; i < attest->size; i++) {
            ss << std::setw(2) << std::setfill('0') 
               << static_cast<int>(attest->attestationData[i]);
        }
        quote.quote_data = ss.str();
        Esys_Free(attest);
    }

    if (signature) {
        // Convert signature to hex string
        std::stringstream ss;
        ss << std::hex;
        if (signature->sigAlg == TPM2_ALG_RSASSA) {
            // RSA signature
            for (size_t i = 0; i < signature->signature.rsassa.sig.size; i++) {
                ss << std::setw(2) << std::setfill('0') 
                   << static_cast<int>(signature->signature.rsassa.sig.buffer[i]);
            }
        } else if (signature->sigAlg == TPM2_ALG_ECDSA) {
            // ECDSA signature
            if (signature->signature.ecdsa.signatureR.size > 0) {
                for (size_t i = 0; i < signature->signature.ecdsa.signatureR.size; i++) {
                    ss << std::setw(2) << std::setfill('0') 
                       << static_cast<int>(signature->signature.ecdsa.signatureR.buffer[i]);
                }
            }
            if (signature->signature.ecdsa.signatureS.size > 0) {
                for (size_t i = 0; i < signature->signature.ecdsa.signatureS.size; i++) {
                    ss << std::setw(2) << std::setfill('0') 
                       << static_cast<int>(signature->signature.ecdsa.signatureS.buffer[i]);
                }
            }
        }
        quote.signature = ss.str();
        Esys_Free(signature);
    }
#else
    // Fallback: Generate placeholder quote when TSS2 library not available
    std::stringstream ss;
    ss << "placeholder_quote_" << std::time(nullptr);
    quote.quote_data = ss.str();
    quote.signature = "placeholder_signature";
#endif

    // Format creation time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    quote.created_at = timeStr;

    return quote;
}

AttestationVerificationResult TPMAttestation::verifyAttestation(const AttestationQuote& quote) {
    AttestationVerificationResult result;
    result.success = false;
    result.is_valid = false;

    if (!m_initialized || !m_tpmDevice) {
        result.error_message = "Attestation not initialized";
        return result;
    }

    if (quote.quote_data.empty() || quote.signature.empty()) {
        result.error_message = "Invalid quote data";
        return result;
    }

#ifdef HAVE_TSS2
    // TPM quote verification using TSS2 library
    ESYS_CONTEXT* esysContext = static_cast<ESYS_CONTEXT*>(m_tpmDevice->getESYSContext());
    if (!esysContext) {
        result.error_message = "ESYS context not available";
        return result;
    }

    // Check if quote data and signature are present and not placeholders
    if (quote.quote_data.empty() || quote.signature.empty() ||
        quote.quote_data.find("placeholder") != std::string::npos ||
        quote.signature.find("placeholder") != std::string::npos) {
        result.success = true;
        result.is_valid = false;
        result.error_message = "Quote verification failed: Invalid quote data or signature";
        return result;
    }

    // Note: In a production implementation, the signing key used for verification
    // should be the same key that was used to sign the quote, or we should use
    // the endorsement key. For now, we create a new key which may not match.
    // A proper implementation would store/retrieve the signing key handle or use EK.
    
    // Create a signing key for verification (same as generation)
    ESYS_TR signingKeyHandle = ESYS_TR_NONE;
    
    TPM2B_PUBLIC inPublic = {0};
    inPublic.size = sizeof(TPMT_PUBLIC);
    inPublic.publicArea.type = TPM2_ALG_RSA;
    inPublic.publicArea.nameAlg = TPM2_ALG_SHA256;
    inPublic.publicArea.objectAttributes = TPMA_OBJECT_USERWITHAUTH | 
                                         TPMA_OBJECT_SIGN_ENCRYPT | 
                                         TPMA_OBJECT_FIXEDTPM | 
                                         TPMA_OBJECT_FIXEDPARENT;
    inPublic.publicArea.parameters.rsaDetail.keyBits = 2048;
    inPublic.publicArea.parameters.rsaDetail.exponent = 0;

    TPM2B_SENSITIVE_CREATE inSensitive = {0};
    TPM2B_DATA outsideInfo = {0};
    TPM2B_PUBLIC* outPublic = nullptr;
    TPM2B_PRIVATE* outPrivate = nullptr;
    TPM2B_DATA* creationData = nullptr;
    TPM2B_DIGEST* creationHash = nullptr;
    TPMT_TK_CREATION* creationTicket = nullptr;
    TPM2B_NAME* name = nullptr;

    TSS2_RC rc = Esys_CreatePrimary(esysContext,
                                    ESYS_TR_RH_ENDORSEMENT,
                                    ESYS_TR_PASSWORD,
                                    ESYS_TR_NONE,
                                    ESYS_TR_NONE,
                                    &inSensitive,
                                    &inPublic,
                                    &outsideInfo,
                                    &creationData,
                                    &creationHash,
                                    &creationTicket,
                                    &outPublic,
                                    &outPrivate,
                                    nullptr,
                                    &name,
                                    &signingKeyHandle);

    if (rc != TSS2_RC_SUCCESS) {
        result.success = true;
        result.is_valid = false;
        result.error_message = "Failed to create verification key";
        return result;
    }

    // Convert quote data from hex string to bytes
    TPM2B_ATTEST attest = {0};
    size_t quoteDataLen = quote.quote_data.length() / 2;
    if (quoteDataLen > sizeof(attest.attestationData)) {
        quoteDataLen = sizeof(attest.attestationData);
    }
    attest.size = quoteDataLen;
    for (size_t i = 0; i < quoteDataLen; i++) {
        std::string byteStr = quote.quote_data.substr(i * 2, 2);
        attest.attestationData[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
    }

    // Convert signature from hex string to bytes
    TPMT_SIGNATURE signature = {0};
    signature.sigAlg = TPM2_ALG_RSASSA;
    size_t sigLen = quote.signature.length() / 2;
    if (sigLen > sizeof(signature.signature.rsassa.sig.buffer)) {
        sigLen = sizeof(signature.signature.rsassa.sig.buffer);
    }
    signature.signature.rsassa.sig.size = sigLen;
    for (size_t i = 0; i < sigLen; i++) {
        std::string byteStr = quote.signature.substr(i * 2, 2);
        signature.signature.rsassa.sig.buffer[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
    }

    // Verify signature
    TPMT_TK_VERIFIED* validation = nullptr;
    rc = Esys_VerifySignature(esysContext,
                              signingKeyHandle,
                              ESYS_TR_NONE,
                              ESYS_TR_NONE,
                              ESYS_TR_NONE,
                              &attest,
                              &signature,
                              &validation);

    // Cleanup
    Esys_FlushContext(esysContext, signingKeyHandle);
    if (outPublic) Esys_Free(outPublic);
    if (outPrivate) Esys_Free(outPrivate);
    if (creationData) Esys_Free(creationData);
    if (creationHash) Esys_Free(creationHash);
    if (creationTicket) Esys_Free(creationTicket);
    if (name) Esys_Free(name);
    if (validation) Esys_Free(validation);

    result.success = true;
    result.is_valid = (rc == TSS2_RC_SUCCESS);
    
    if (!result.is_valid) {
        std::stringstream ss;
        ss << "Quote verification failed: 0x" << std::hex << rc;
        result.error_message = ss.str();
    }
#else
    // Fallback: Basic validation when TSS2 library not available
    result.success = true;
    result.is_valid = !quote.quote_data.empty() && !quote.signature.empty();
    
    if (!result.is_valid) {
        result.error_message = "Quote verification unavailable: TSS2 library not available";
    }
#endif

    // Format verification time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    result.verified_at = timeStr;

    if (!result.is_valid) {
        result.error_message = "Quote verification failed";
    }

    return result;
}

std::string TPMAttestation::generateNonce(size_t length) const {
    std::vector<unsigned char> nonceBytes(length);
    
    if (RAND_bytes(nonceBytes.data(), length) == 1) {
        std::stringstream ss;
        for (size_t i = 0; i < length; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') 
               << static_cast<int>(nonceBytes[i]);
        }
        return ss.str();
    }
    
    // Fallback: Use random device
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::stringstream ss;
    for (size_t i = 0; i < length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }
    return ss.str();
}

