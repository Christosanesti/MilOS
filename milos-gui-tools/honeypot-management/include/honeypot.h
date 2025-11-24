#ifndef HONEYPOT_H
#define HONEYPOT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Honeypot Type
 */
enum class HoneypotType {
    WebServer,
    SSH,
    FTP,
    Database,
    Email,
    Custom
};

/**
 * @brief Honeypot Status
 */
enum class HoneypotStatus {
    Stopped,
    Starting,
    Running,
    Stopping,
    Error
};

/**
 * @brief Honeypot Configuration
 */
struct HoneypotConfig {
    QString id;
    QString name;
    HoneypotType type;
    QString host;
    int port;
    QVariantMap parameters;
    bool enabled;
};

/**
 * @brief Honeypot
 * 
 * Represents a single honeypot instance.
 */
class Honeypot : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(HoneypotStatus status READ status NOTIFY statusChanged)

public:
    explicit Honeypot(QObject* parent = nullptr);
    ~Honeypot();

    /**
     * @brief Initialize honeypot
     * @param config Honeypot configuration
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const HoneypotConfig& config);

    /**
     * @brief Get honeypot ID
     * @return Honeypot ID
     */
    QString id() const { return m_config.id; }

    /**
     * @brief Get honeypot status
     * @return Honeypot status
     */
    HoneypotStatus status() const { return m_status; }

    /**
     * @brief Start honeypot
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop honeypot
     */
    void stop();

    /**
     * @brief Get interaction count
     * @return Number of interactions
     */
    int getInteractionCount() const { return m_interactionCount; }

    /**
     * @brief Get threat data
     * @return Threat data
     */
    QVariantMap getThreatData() const;

Q_SIGNALS:
    /**
     * @brief Emitted when status changes
     */
    void statusChanged(HoneypotStatus status);

    /**
     * @brief Emitted when interaction detected
     */
    void interactionDetected(const QVariantMap& interactionData);

private:
    HoneypotConfig m_config;
    HoneypotStatus m_status;
    int m_interactionCount;
    
    void updateStatus(HoneypotStatus status);
    void simulateInteraction();
};

Q_DECLARE_METATYPE(HoneypotType)
Q_DECLARE_METATYPE(HoneypotStatus)
Q_DECLARE_METATYPE(HoneypotConfig)

#endif // HONEYPOT_H






