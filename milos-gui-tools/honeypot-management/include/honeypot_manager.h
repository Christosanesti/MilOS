#ifndef HONEYPOT_MANAGER_H
#define HONEYPOT_MANAGER_H

#include "honeypot.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

/**
 * @brief Honeypot Manager
 * 
 * Manages multiple honeypot instances.
 */
class HoneypotManager : public QObject {
    Q_OBJECT

public:
    explicit HoneypotManager(QObject* parent = nullptr);
    ~HoneypotManager();

    /**
     * @brief Initialize honeypot manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Deploy honeypot
     * @param config Honeypot configuration
     * @return Honeypot ID or empty if failed
     */
    QString deployHoneypot(const HoneypotConfig& config);

    /**
     * @brief Remove honeypot
     * @param honeypotId Honeypot ID
     * @return true if remove successful, false otherwise
     */
    bool removeHoneypot(const QString& honeypotId);

    /**
     * @brief Get honeypot
     * @param honeypotId Honeypot ID
     * @return Honeypot or nullptr if not found
     */
    Honeypot* getHoneypot(const QString& honeypotId) const;

    /**
     * @brief Get all honeypots
     * @return List of honeypot IDs
     */
    QStringList getAllHoneypots() const;

    /**
     * @brief Get threat intelligence
     * @return Aggregated threat data
     */
    QVariantMap getThreatIntelligence() const;

Q_SIGNALS:
    /**
     * @brief Emitted when honeypot is deployed
     */
    void honeypotDeployed(const QString& honeypotId);

    /**
     * @brief Emitted when threat is detected
     */
    void threatDetected(const QString& honeypotId, const QVariantMap& threatData);

private:
    QMap<QString, Honeypot*> m_honeypots;
    
    QString generateHoneypotId() const;
};

#endif // HONEYPOT_MANAGER_H





