#ifndef MFA_MANAGER_H
#define MFA_MANAGER_H

#include "biometric_authenticator.h"
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief MFA Factor Type
 */
enum class MFAFactorType {
    Biometric,
    HardwareToken,
    Password,
    PIN
};

/**
 * @brief MFA Manager
 * 
 * Manages multi-factor authentication.
 */
class MFAManager : public QObject {
    Q_OBJECT

public:
    explicit MFAManager(QObject* parent = nullptr);
    ~MFAManager();

    /**
     * @brief Initialize MFA manager
     * @param authenticator Biometric authenticator
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BiometricAuthenticator* authenticator);

    /**
     * @brief Authenticate with MFA
     * @param userId User ID
     * @param factors List of factor types to use
     * @return true if authentication successful, false otherwise
     */
    bool authenticate(const QString& userId, const QList<MFAFactorType>& factors);

    /**
     * @brief Add hardware token
     * @param userId User ID
     * @param tokenId Token ID
     * @return true if add successful, false otherwise
     */
    bool addHardwareToken(const QString& userId, const QString& tokenId);

    /**
     * @brief Verify hardware token
     * @param userId User ID
     * @param tokenCode Token code
     * @return true if verify successful, false otherwise
     */
    bool verifyHardwareToken(const QString& userId, const QString& tokenCode);

Q_SIGNALS:
    /**
     * @brief Emitted when MFA authentication completes
     */
    void mfaAuthenticationCompleted(const QString& userId, bool success);

private:
    BiometricAuthenticator* m_authenticator;
    QMap<QString, QStringList> m_userTokens;
    
    bool verifyPassword(const QString& userId, const QString& password) const;
    bool verifyPIN(const QString& userId, const QString& pin) const;
};

Q_DECLARE_METATYPE(MFAFactorType)

#endif // MFA_MANAGER_H

