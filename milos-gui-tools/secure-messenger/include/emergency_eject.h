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
    bool executeEject(const QString& confirmationCode);

    /**
     * @brief Check if eject is in progress
     * @return true if ejecting, false otherwise
     */
    bool isEjecting() const;

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

private:
    bool m_ejecting;
    QString m_confirmationCode;
    
    bool deleteAllData();
    bool dismantleSystem();
    bool verifyDeletion();
};

#endif // EMERGENCY_EJECT_H

