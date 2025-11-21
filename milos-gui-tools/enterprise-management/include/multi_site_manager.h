#ifndef MULTI_SITE_MANAGER_H
#define MULTI_SITE_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QTimer>

/**
 * @brief Site Status
 */
enum class SiteStatus {
    Online,
    Offline,
    Degraded,
    Unknown
};

/**
 * @brief Site Information
 */
struct SiteInfo {
    QString id;
    QString name;
    QString address;
    SiteStatus status;
    QVariantMap metadata;
};

/**
 * @brief Multi-Site Manager
 * 
 * Manages multiple deployment sites.
 */
class MultiSiteManager : public QObject {
    Q_OBJECT

public:
    explicit MultiSiteManager(QObject* parent = nullptr);
    ~MultiSiteManager();

    /**
     * @brief Initialize multi-site manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add site
     * @param siteInfo Site information
     * @return true if add successful, false otherwise
     */
    bool addSite(const SiteInfo& siteInfo);

    /**
     * @brief Remove site
     * @param siteId Site ID
     * @return true if remove successful, false otherwise
     */
    bool removeSite(const QString& siteId);

    /**
     * @brief Get site
     * @param siteId Site ID
     * @return Site information or empty if not found
     */
    SiteInfo getSite(const QString& siteId) const;

    /**
     * @brief Get all sites
     * @return List of site IDs
     */
    QStringList getAllSites() const;

    /**
     * @brief Check site connectivity
     * @param siteId Site ID
     * @return true if connected, false otherwise
     */
    bool checkSiteConnectivity(const QString& siteId);

    /**
     * @brief Get site statistics
     * @param siteId Site ID
     * @return Statistics map
     */
    QVariantMap getSiteStatistics(const QString& siteId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when site status changes
     */
    void siteStatusChanged(const QString& siteId, SiteStatus status);

private:
    QMap<QString, SiteInfo> m_sites;
    QTimer* m_connectivityTimer;
    
    void checkAllSitesConnectivity();

private slots:
    void onConnectivityCheck();
};

Q_DECLARE_METATYPE(SiteStatus)
Q_DECLARE_METATYPE(SiteInfo)

#endif // MULTI_SITE_MANAGER_H

