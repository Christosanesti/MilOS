#ifndef THREAT_BLOCKER_H
#define THREAT_BLOCKER_H

#include "threat_feed.h"
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief Threat Blocker
 * 
 * Automatically blocks threats based on intelligence feeds.
 */
class ThreatBlocker : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool autoBlockingEnabled READ autoBlockingEnabled WRITE setAutoBlockingEnabled NOTIFY autoBlockingEnabledChanged)

public:
    explicit ThreatBlocker(QObject* parent = nullptr);
    ~ThreatBlocker();

    /**
     * @brief Initialize threat blocker
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if auto-blocking is enabled
     * @return true if enabled, false otherwise
     */
    bool autoBlockingEnabled() const { return m_autoBlockingEnabled; }

    /**
     * @brief Set auto-blocking enabled
     * @param enabled Enabled state
     */
    void setAutoBlockingEnabled(bool enabled);

    /**
     * @brief Check if indicator should be blocked
     * @param indicator Threat indicator
     * @return true if should block, false otherwise
     */
    bool shouldBlock(const ThreatIndicator& indicator) const;

    /**
     * @brief Block threat indicator
     * @param indicator Threat indicator
     * @return true if block successful, false otherwise
     */
    bool blockIndicator(const ThreatIndicator& indicator);

    /**
     * @brief Get blocked indicators
     * @return List of blocked indicator IDs
     */
    QStringList getBlockedIndicators() const;

    /**
     * @brief Unblock indicator
     * @param indicatorId Indicator ID
     * @return true if unblock successful, false otherwise
     */
    bool unblockIndicator(const QString& indicatorId);

Q_SIGNALS:
    /**
     * @brief Emitted when auto-blocking state changes
     */
    void autoBlockingEnabledChanged(bool enabled);

    /**
     * @brief Emitted when indicator is blocked
     */
    void indicatorBlocked(const ThreatIndicator& indicator);

    /**
     * @brief Emitted when indicator is unblocked
     */
    void indicatorUnblocked(const QString& indicatorId);

private:
    bool m_autoBlockingEnabled;
    QStringList m_blockedIndicators;
    
    bool isCriticalSeverity(const QString& severity) const;
};

#endif // THREAT_BLOCKER_H

