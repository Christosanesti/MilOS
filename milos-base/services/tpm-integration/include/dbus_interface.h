#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class TPMService;
class TPMDevice;
class TPMKeyManager;
class TPMAttestation;
class SecureBootManager;

/**
 * @brief D-Bus Interface for TPM Integration Service
 * 
 * Implements org.milos.TPM D-Bus interface.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.TPM")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Set TPM service instance
     */
    void setTPMService(TPMService* service);

    /**
     * @brief Set TPM device instance
     */
    void setTPMDevice(TPMDevice* device);

    /**
     * @brief Set key manager instance
     */
    void setKeyManager(TPMKeyManager* manager);

    /**
     * @brief Set attestation instance
     */
    void setAttestation(TPMAttestation* attestation);

    /**
     * @brief Set secure boot manager instance
     */
    void setSecureBootManager(SecureBootManager* secureBootManager);

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Get TPM status
     * @return Status information (JSON string)
     */
    QString GetTPMStatus();

    /**
     * @brief Generate key in TPM
     * @param keyParams Key generation parameters (JSON string)
     * @return Key information (JSON string)
     */
    QString GenerateKey(const QString& keyParams);

    /**
     * @brief Get boot measurement
     * @return Boot measurement (PCR values as JSON string)
     */
    QString GetBootMeasurement();

    /**
     * @brief Generate attestation quote
     * @param nonce Nonce for quote (optional)
     * @return Attestation quote (JSON string)
     */
    QString GenerateAttestation(const QString& nonce = "");

    /**
     * @brief Verify attestation quote
     * @param quote Attestation quote (JSON string)
     * @return Verification result (JSON string)
     */
    QString VerifyAttestation(const QString& quote);

    /**
     * @brief Verify boot integrity
     * @param policyId Policy ID (optional, uses default if empty)
     * @return Boot integrity verification result (JSON string)
     */
    QString VerifyBootIntegrity(const QString& policyId = "");

    /**
     * @brief Get secure boot status
     * @return Secure boot status (JSON string)
     */
    QString GetSecureBootStatus();

    /**
     * @brief Store boot baseline
     * @return Success status (JSON string)
     */
    QString StoreBootBaseline();

Q_SIGNALS:
    /**
     * @brief Emitted when TPM status changes
     * @param status Status string
     */
    void TPMStatusChanged(const QString& status);

private:
    bool m_initialized;
    TPMService* m_tpmService;
    TPMDevice* m_tpmDevice;
    TPMKeyManager* m_keyManager;
    TPMAttestation* m_attestation;
    SecureBootManager* m_secureBootManager;
};

#endif // DBUS_INTERFACE_H

