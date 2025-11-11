#ifndef SIGNATURE_VERIFIER_H
#define SIGNATURE_VERIFIER_H

#include <string>
#include <vector>
#include <memory>

class ConfigParser;

/**
 * @brief Signature Verification Result
 */
struct SignatureResult {
    bool verified;
    std::string key_id;
    std::string error_message;
};

/**
 * @brief Signature Verifier
 * 
 * Verifies GPG signatures for packages before installation.
 */
class SignatureVerifier {
public:
    SignatureVerifier();
    ~SignatureVerifier();

    /**
     * @brief Initialize signature verifier
     * @param configParser Configuration parser instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser);

    /**
     * @brief Verify package signature
     * @param packagePath Path to package file
     * @return Signature verification result
     */
    SignatureResult verifyPackage(const std::string& packagePath);

    /**
     * @brief Verify package signature by name (downloads and verifies)
     * @param packageName Package name
     * @return Signature verification result
     */
    SignatureResult verifyPackageByName(const std::string& packageName);

    /**
     * @brief Import GPG key
     * @param keyPath Path to GPG key file
     * @return true if import successful, false otherwise
     */
    bool importKey(const std::string& keyPath);

    /**
     * @brief Check if signature verification is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return m_enabled; }

    /**
     * @brief Check if signature verification is required
     * @return true if required, false otherwise
     */
    bool isRequired() const { return m_requireVerification; }

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    bool m_enabled;
    bool m_requireVerification;
    std::string m_keyringPath;
    std::vector<std::string> m_repositoryKeys;

    /**
     * @brief Verify signature using GPG
     * @param packagePath Path to package file
     * @param signaturePath Path to signature file (optional)
     * @return Signature verification result
     */
    SignatureResult verifyWithGPG(const std::string& packagePath, const std::string& signaturePath = "");

    /**
     * @brief Find signature file for package
     * @param packagePath Path to package file
     * @return Path to signature file, empty if not found
     */
    std::string findSignatureFile(const std::string& packagePath);
};

#endif // SIGNATURE_VERIFIER_H

