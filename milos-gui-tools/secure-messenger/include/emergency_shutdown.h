#ifndef EMERGENCY_SHUTDOWN_H
#define EMERGENCY_SHUTDOWN_H

#include <QObject>
#include <QString>
#include <QDateTime>

/**
 * @brief Shutdown Reason
 */
enum class ShutdownReason {
    Emergency,
    Compromise,
    Maintenance,
    UserRequest
};

/**
 * @brief Emergency Shutdown
 * 
 * Provides emergency shutdown protocols for compromised scenarios.
 */
class EmergencyShutdown : public QObject {
    Q_OBJECT

public:
    explicit EmergencyShutdown(QObject* parent = nullptr);
    ~EmergencyShutdown();

    /**
     * @brief Initialize emergency shutdown
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Execute emergency shutdown
     * @param reason Shutdown reason
     * @param confirmationCode Confirmation code
     * @return true if shutdown successful, false otherwise
     */
    bool executeShutdown(ShutdownReason reason, const QString& confirmationCode);

    /**
     * @brief Check if shutdown is in progress
     * @return true if shutting down, false otherwise
     */
    bool isShuttingDown() const;

Q_SIGNALS:
    /**
     * @brief Emitted when shutdown is initiated
     */
    void shutdownInitiated(ShutdownReason reason);

    /**
     * @brief Emitted when shutdown is completed
     */
    void shutdownCompleted();

    /**
     * @brief Emitted when shutdown fails
     */
    void shutdownFailed(const QString& error);

private:
    bool m_shuttingDown;
    
    bool secureShutdown();
    bool notifyParticipants();
    bool saveState();
};

#endif // EMERGENCY_SHUTDOWN_H

