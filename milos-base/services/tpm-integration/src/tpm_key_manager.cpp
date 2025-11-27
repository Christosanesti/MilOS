#include "tpm_key_manager.h"
#include "tpm_device.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <iostream>
#include <algorithm>

#ifdef HAVE_TSS2
#include <tss2/tss2_esys.h>
#endif

TPMKeyManager::TPMKeyManager()
    : m_initialized(false)
    , m_tpmDevice(nullptr)
{
}

TPMKeyManager::~TPMKeyManager() {
}

bool TPMKeyManager::initialize(TPMDevice* tpmDevice) {
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

TPMKeyInfo TPMKeyManager::generateKey(const KeyGenerationParams& params) {
    TPMKeyInfo keyInfo;
    keyInfo.key_type = params.key_type;
    keyInfo.is_persistent = params.persistent;

    if (!m_initialized || !m_tpmDevice) {
        return keyInfo;
    }

#ifdef HAVE_TSS2
    // TPM key generation using TSS2 library
    ESYS_CONTEXT* esysContext = static_cast<ESYS_CONTEXT*>(m_tpmDevice->getESYSContext());
    if (!esysContext) {
        std::cerr << "ESYS context not available" << std::endl;
        return keyInfo;
    }

    TSS2_RC rc;
    ESYS_TR keyHandle = ESYS_TR_NONE;
    
    // Determine key attributes based on key type and usage
    TPMA_OBJECT objectAttributes = TPMA_OBJECT_USERWITHAUTH | 
                                    TPMA_OBJECT_RESTRICTED | 
                                    TPMA_OBJECT_DECRYPT;
    
    if (params.key_usage == "signing") {
        objectAttributes = TPMA_OBJECT_USERWITHAUTH | 
                          TPMA_OBJECT_SIGN_ENCRYPT | 
                          TPMA_OBJECT_FIXEDTPM | 
                          TPMA_OBJECT_FIXEDPARENT;
    } else if (params.key_usage == "storage") {
        objectAttributes = TPMA_OBJECT_USERWITHAUTH | 
                          TPMA_OBJECT_RESTRICTED | 
                          TPMA_OBJECT_DECRYPT | 
                          TPMA_OBJECT_FIXEDTPM | 
                          TPMA_OBJECT_FIXEDPARENT;
    }

    // Prepare key parameters
    TPM2B_PUBLIC inPublic = {0};
    inPublic.size = sizeof(TPMT_PUBLIC);
    inPublic.publicArea.type = TPM2_ALG_RSA;
    inPublic.publicArea.nameAlg = TPM2_ALG_SHA256;
    inPublic.publicArea.objectAttributes = objectAttributes;
    
    // Set key size based on key type
    if (params.key_type == "RSA2048") {
        inPublic.publicArea.parameters.rsaDetail.keyBits = 2048;
        inPublic.publicArea.parameters.rsaDetail.exponent = 0;  // Default exponent
    } else if (params.key_type == "ECC256") {
        inPublic.publicArea.type = TPM2_ALG_ECC;
        inPublic.publicArea.parameters.eccDetail.curveID = TPM2_ECC_NIST_P256;
        inPublic.publicArea.parameters.eccDetail.kdf.scheme = TPM2_ALG_NULL;
        inPublic.publicArea.parameters.eccDetail.scheme.scheme = TPM2_ALG_NULL;
    } else {
        // Default to RSA 2048
        inPublic.publicArea.parameters.rsaDetail.keyBits = 2048;
    }

    TPM2B_DATA outsideInfo = {0};
    TPM2B_SENSITIVE_CREATE inSensitive = {0};
    TPM2B_PUBLIC* outPublic = nullptr;
    TPM2B_PRIVATE* outPrivate = nullptr;
    TPM2B_DATA* creationData = nullptr;
    TPM2B_DIGEST* creationHash = nullptr;
    TPMT_TK_CREATION* creationTicket = nullptr;
    TPM2B_NAME* name = nullptr;

    // Create primary key in owner hierarchy
    rc = Esys_CreatePrimary(esysContext,
                           ESYS_TR_RH_OWNER,
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
                           &keyHandle);

    if (rc != TSS2_RC_SUCCESS) {
        std::cerr << "Failed to create primary key: 0x" << std::hex << rc << std::endl;
        return keyInfo;
    }

    // Extract public key data
    if (outPublic) {
        std::stringstream ss;
        ss << std::hex;
        
        if (outPublic->publicArea.type == TPM2_ALG_RSA && 
            outPublic->publicArea.unique.rsa.size > 0) {
            // RSA public key
            for (size_t i = 0; i < outPublic->publicArea.unique.rsa.size; i++) {
                ss << std::setw(2) << std::setfill('0') 
                   << static_cast<int>(outPublic->publicArea.unique.rsa.buffer[i]);
            }
        } else if (outPublic->publicArea.type == TPM2_ALG_ECC && 
                   outPublic->publicArea.unique.ecc.x.size > 0) {
            // ECC public key (x coordinate)
            for (size_t i = 0; i < outPublic->publicArea.unique.ecc.x.size; i++) {
                ss << std::setw(2) << std::setfill('0') 
                   << static_cast<int>(outPublic->publicArea.unique.ecc.x.buffer[i]);
            }
            // Add y coordinate if present
            if (outPublic->publicArea.unique.ecc.y.size > 0) {
                for (size_t i = 0; i < outPublic->publicArea.unique.ecc.y.size; i++) {
                    ss << std::setw(2) << std::setfill('0') 
                       << static_cast<int>(outPublic->publicArea.unique.ecc.y.buffer[i]);
                }
            }
        }
        
        keyInfo.public_key = ss.str();
    }

    // Generate key ID
    std::stringstream keyIdStream;
    keyIdStream << "tss2_key_" << params.key_type << "_" << std::hex << std::time(nullptr);
    keyInfo.key_id = keyIdStream.str();

    // Make key persistent if requested
    if (params.persistent) {
        uint32_t persistentHandle = generatePersistentHandle();
        
        rc = Esys_EvictControl(esysContext,
                              ESYS_TR_RH_OWNER,
                              keyHandle,
                              ESYS_TR_PASSWORD,
                              ESYS_TR_NONE,
                              ESYS_TR_NONE,
                              persistentHandle,
                              nullptr);
        
        if (rc == TSS2_RC_SUCCESS) {
            keyInfo.persistent_handle = persistentHandle;
            // Flush transient handle since we now have persistent handle
            Esys_FlushContext(esysContext, keyHandle);
        } else {
            std::cerr << "Failed to make key persistent: 0x" << std::hex << rc << std::endl;
            // Keep transient handle
        }
    }

    // Cleanup
    if (outPublic) Esys_Free(outPublic);
    if (outPrivate) Esys_Free(outPrivate);
    if (creationData) Esys_Free(creationData);
    if (creationTicket) Esys_Free(creationTicket);
    if (creationHash) Esys_Free(creationHash);
    if (name) Esys_Free(name);
#else
    // Fallback: Generate key ID without TPM
    std::stringstream ss;
    ss << std::hex << std::time(nullptr) << "-" << std::rand();
    keyInfo.key_id = ss.str();
#endif

    // Format creation time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    keyInfo.created_at = timeStr;

    // Store key info
    m_keys.push_back(keyInfo);

    return keyInfo;
}

TPMKeyInfo TPMKeyManager::getKey(const std::string& keyId) const {
    for (const auto& key : m_keys) {
        if (key.key_id == keyId) {
            return key;
        }
    }
    return TPMKeyInfo();  // Return empty key
}

std::vector<TPMKeyInfo> TPMKeyManager::listKeys() const {
    return m_keys;
}

bool TPMKeyManager::deleteKey(const std::string& keyId) {
    auto it = std::find_if(m_keys.begin(), m_keys.end(),
                          [&keyId](const TPMKeyInfo& key) { return key.key_id == keyId; });
    
    if (it != m_keys.end()) {
#ifdef HAVE_TSS2
        // TPM key deletion using TSS2 library
        ESYS_CONTEXT* esysContext = static_cast<ESYS_CONTEXT*>(m_tpmDevice->getESYSContext());
        if (esysContext) {
            if (it->is_persistent && it->persistent_handle != 0) {
                // Evict persistent handle from TPM
                ESYS_TR persistentHandle = ESYS_TR_PERSISTENT | it->persistent_handle;
                TSS2_RC rc = Esys_EvictControl(esysContext,
                                              ESYS_TR_RH_OWNER,
                                              persistentHandle,
                                              ESYS_TR_PASSWORD,
                                              ESYS_TR_NONE,
                                              ESYS_TR_NONE,
                                              it->persistent_handle,
                                              nullptr);
                if (rc != TSS2_RC_SUCCESS) {
                    std::cerr << "Failed to evict persistent key: 0x" << std::hex << rc << std::endl;
                }
            }
        }
#endif
        m_keys.erase(it);
        return true;
    }
    
    return false;
}

uint32_t TPMKeyManager::generatePersistentHandle() const {
    // Generate a persistent handle in the persistent range (0x81000000 - 0x817FFFFF)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0x81000000, 0x817FFFFF);
    return dis(gen);
}

