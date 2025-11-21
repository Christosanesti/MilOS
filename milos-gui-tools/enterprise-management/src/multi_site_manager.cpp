#include "multi_site_manager.h"
#include <QTimer>
#include <QUuid>
#include <QDebug>

MultiSiteManager::MultiSiteManager(QObject* parent)
    : QObject(parent)
    , m_connectivityTimer(new QTimer(this))
{
    connect(m_connectivityTimer, &QTimer::timeout, this, &MultiSiteManager::onConnectivityCheck);
}

MultiSiteManager::~MultiSiteManager() {
}

bool MultiSiteManager::initialize() {
    m_connectivityTimer->setInterval(30000);  // Check every 30 seconds
    m_connectivityTimer->start();
    return true;
}

bool MultiSiteManager::addSite(const SiteInfo& siteInfo) {
    m_sites[siteInfo.id] = siteInfo;
    return true;
}

bool MultiSiteManager::removeSite(const QString& siteId) {
    if (!m_sites.contains(siteId)) {
        return false;
    }
    
    m_sites.remove(siteId);
    return true;
}

SiteInfo MultiSiteManager::getSite(const QString& siteId) const {
    return m_sites.value(siteId, SiteInfo());
}

QStringList MultiSiteManager::getAllSites() const {
    return m_sites.keys();
}

bool MultiSiteManager::checkSiteConnectivity(const QString& siteId) {
    if (!m_sites.contains(siteId)) {
        return false;
    }
    
    SiteInfo& site = m_sites[siteId];
    
    // In production, would ping site or check network connectivity
    // For now, simulate connectivity check
    SiteStatus oldStatus = site.status;
    site.status = SiteStatus::Online;  // Simulate online
    
    if (oldStatus != site.status) {
        emit siteStatusChanged(siteId, site.status);
    }
    
    return (site.status == SiteStatus::Online);
}

QVariantMap MultiSiteManager::getSiteStatistics(const QString& siteId) const {
    QVariantMap stats;
    
    if (!m_sites.contains(siteId)) {
        return stats;
    }
    
    const SiteInfo& site = m_sites[siteId];
    stats["id"] = site.id;
    stats["name"] = site.name;
    stats["address"] = site.address;
    stats["status"] = static_cast<int>(site.status);
    stats["metadata"] = site.metadata;
    
    return stats;
}

void MultiSiteManager::checkAllSitesConnectivity() {
    for (auto it = m_sites.begin(); it != m_sites.end(); ++it) {
        checkSiteConnectivity(it.key());
    }
}

void MultiSiteManager::onConnectivityCheck() {
    checkAllSitesConnectivity();
}

