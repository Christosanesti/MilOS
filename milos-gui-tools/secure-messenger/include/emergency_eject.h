#ifndef EMERGENCY_EJECT_H
#define EMERGENCY_EJECT_H

#include <QObject>
#include <QString>
#include <QDateTime>

/**
 * @brief Emergency Eject
 * 
 * Provides emergency eject functionality to immediately dismantle and delete all data.
 */
class EmergencyEject : public QObject {
    Q_OBJECT

public:
    explicit EmergencyEject(QObject* parent = nullptr);
    ~EmergencyEject();

    /**
     * @brief Initialize emergency eject
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Execute emergency eject
     * @param confirmationCode Confirmation code
     * @return true if eject successful, false otherwise
     */
    Q_INVOKABLE bool executeEject(const QString& confirmationCode);

    /**
     * @brief Check if eject is in progress
     * @return true if ejecting, false otherwise
     */
    Q_INVOKABLE bool isEjecting() const;

Q_SIGNALS:
    /**
     * @brief Emitted when eject is initiated
     */
    void ejectInitiated();

    /**
     * @brief Emitted when eject is completed
     */
    void ejectCompleted();

    /**
     * @brief Emitted when eject fails
     */
    void ejectFailed(const QString& error);

    /**
     * @brief Set confirmation code (requires authentication)
     * @param currentCode Current confirmation code (if setting for first time, use system default)
     * @param newCode New confirmation code
     * @return true if code set successfully, false otherwise
     */
    Q_INVOKABLE bool setConfirmationCode(const QString& currentCode, const QString& newCode);

private:
    bool m_ejecting;
    QString m_confirmationHash;  // SHA-256 hash of confirmation code + salt
    
    bool deleteAllData();
    bool dismantleSystem();
    bool verifyDeletion();
    QString getSystemSalt() const;
    void generateInitialConfirmationHash();
};

#endif // EMERGENCY_EJECT_H


    bool deleteAllData();
    bool dismantleSystem();
    bool verifyDeletion();
    QString getSystemSalt() const;
    void generateInitialConfirmationHash();
};

#endif // EMERGENCY_EJECT_H

