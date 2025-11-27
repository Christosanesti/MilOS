#include "milos/logging/cve_client.h"
#include "milos/logging/logger.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QNetworkReply>

CVEClient::CVEClient(QObject* parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_nvdEnabled(false)
    , m_osvEnabled(false)
    , m_githubEnabled(false)
{
}

CVEClient::~CVEClient() {
    if (m_networkManager) {
        delete m_networkManager;
    }
}

bool CVEClient::initialize(bool enableNVD, bool enableOSV, bool enableGitHub) {
    m_networkManager = new QNetworkAccessManager(this);
    m_nvdEnabled = enableNVD;
    m_osvEnabled = enableOSV;
    m_githubEnabled = enableGitHub;

    // Connect network reply signals - use direct signal connection
    // Note: QNetworkAccessManager::finished signal will be handled by reply-specific connections

    // Load API key from environment or config
    m_nvdApiKey = qgetenv("NVD_API_KEY");

    LOG_INFO("CVE Client initialized");
    return true;
}

QList<CVEClient::Vulnerability> CVEClient::checkPackage(const QString& packageName,
                                                        const QString& packageVersion,
                                                        const QString& ecosystem) {
    QList<Vulnerability> vulnerabilities;

    if (m_nvdEnabled) {
        QList<Vulnerability> nvdVulns = queryNVD(packageName, packageVersion);
        vulnerabilities.append(nvdVulns);
    }

    if (m_osvEnabled) {
        QList<Vulnerability> osvVulns = queryOSV(packageName, packageVersion, ecosystem);
        vulnerabilities.append(osvVulns);
    }

    if (m_githubEnabled) {
        QList<Vulnerability> githubVulns = queryGitHub(packageName, packageVersion);
        vulnerabilities.append(githubVulns);
    }

    return vulnerabilities;
}

QVariantMap CVEClient::checkPackages(const QList<QVariantMap>& packages) {
    QVariantMap results;

    for (const QVariantMap& package : packages) {
        QString name = package.value("name").toString();
        QString version = package.value("version").toString();
        QString ecosystem = package.value("ecosystem").toString();

        QList<Vulnerability> vulns = checkPackage(name, version, ecosystem);
        
        QVariantList vulnList;
        for (const Vulnerability& vuln : vulns) {
            QVariantMap vulnMap;
            vulnMap["cve_id"] = vuln.cveId;
            vulnMap["description"] = vuln.description;
            vulnMap["severity"] = vuln.severity;
            vulnMap["published_date"] = vuln.publishedDate;
            vulnMap["affected_versions"] = vuln.affectedVersions;
            vulnMap["fixed_versions"] = vuln.fixedVersions;
            vulnMap["source"] = vuln.source;
            vulnList.append(vulnMap);
        }
        
        results[name] = vulnList;
    }

    return results;
}

CVEClient::Vulnerability CVEClient::getCVEDetails(const QString& cveId) {
    Vulnerability vuln;
    vuln.cveId = cveId;

    if (m_nvdEnabled) {
        // Query NVD for CVE details
        QUrl url(QString("https://services.nvd.nist.gov/rest/json/cves/2.0?cveId=%1").arg(cveId));
        QNetworkRequest request(url);
        
        if (!m_nvdApiKey.isEmpty()) {
            request.setRawHeader("apiKey", m_nvdApiKey.toUtf8());
        }
        
        QNetworkReply* reply = m_networkManager->get(request);
        m_pendingRequests[reply] = qMakePair(cveId, "NVD");
        
        // Note: In production, would use async/await or signals
        // For now, this is a placeholder structure
    }

    return vuln;
}

void CVEClient::onNVDReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    QPair<QString, QString> requestInfo = m_pendingRequests.take(reply);
    QString packageName = requestInfo.first;
    QString packageVersion = requestInfo.second;
    
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("NVD API request failed: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isObject()) {
        LOG_WARNING("Invalid NVD API response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray vulnerabilities = root.value("vulnerabilities").toArray();
    
    for (const QJsonValue& vulnValue : vulnerabilities) {
        QJsonObject vulnObj = vulnValue.toObject();
        QJsonObject cve = vulnObj.value("cve").toObject();
        
        Vulnerability vuln = parseNVDResponse(vulnObj);
        if (!vuln.cveId.isEmpty()) {
            emit vulnerabilityFound(vuln);
        }
    }
    
    reply->deleteLater();
}

void CVEClient::onOSVReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    QPair<QString, QString> requestInfo = m_pendingRequests.take(reply);
    QString packageName = requestInfo.first;
    QString packageVersion = requestInfo.second;
    
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("OSV API request failed: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isObject()) {
        LOG_WARNING("Invalid OSV API response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray vulns = root.value("vulns").toArray();
    
    for (const QJsonValue& vulnValue : vulns) {
        QJsonObject vulnObj = vulnValue.toObject();
        Vulnerability vuln = parseOSVResponse(vulnObj);
        if (!vuln.cveId.isEmpty()) {
            emit vulnerabilityFound(vuln);
        }
    }
    
    reply->deleteLater();
}

void CVEClient::onGitHubReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    QPair<QString, QString> requestInfo = m_pendingRequests.take(reply);
    QString packageName = requestInfo.first;
    QString packageVersion = requestInfo.second;
    
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("GitHub API request failed: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isArray()) {
        LOG_WARNING("Invalid GitHub API response");
        reply->deleteLater();
        return;
    }
    
    QJsonArray advisories = doc.array();
    
    for (const QJsonValue& advisoryValue : advisories) {
        QJsonObject advisoryObj = advisoryValue.toObject();
        Vulnerability vuln = parseGitHubResponse(advisoryObj);
        if (!vuln.cveId.isEmpty()) {
            emit vulnerabilityFound(vuln);
        }
    }
    
    reply->deleteLater();
}

QList<CVEClient::Vulnerability> CVEClient::queryNVD(const QString& packageName, const QString& packageVersion) {
    QList<Vulnerability> vulnerabilities;

    // NVD API query
    // Format: https://services.nvd.nist.gov/rest/json/cves/2.0?keywordSearch=<package>
    QUrl url(QString("https://services.nvd.nist.gov/rest/json/cves/2.0?keywordSearch=%1").arg(packageName));
    QNetworkRequest request(url);
    
    if (!m_nvdApiKey.isEmpty()) {
        request.setRawHeader("apiKey", m_nvdApiKey.toUtf8());
    }
    
    QNetworkReply* reply = m_networkManager->get(request);
    m_pendingRequests[reply] = qMakePair(packageName, packageVersion);
    
    // Connect reply signals
    connect(reply, &QNetworkReply::finished, this, &CVEClient::onNVDReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply]() {
        LOG_ERROR(QString("NVD API network error: %1").arg(reply->errorString()));
    });

    // Return empty list - vulnerabilities will be emitted via signals
    return vulnerabilities;
}

QList<CVEClient::Vulnerability> CVEClient::queryOSV(const QString& packageName, 
                                                     const QString& packageVersion,
                                                     const QString& ecosystem) {
    QList<Vulnerability> vulnerabilities;

    // OSV API query
    // Format: POST https://api.osv.dev/v1/query
    QUrl url("https://api.osv.dev/v1/query");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject query;
    query["package"] = QJsonObject{
        {"name", packageName},
        {"ecosystem", ecosystem.isEmpty() ? "UNSPECIFIED" : ecosystem}
    };
    query["version"] = packageVersion;

    QJsonDocument doc(query);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    m_pendingRequests[reply] = qMakePair(packageName, packageVersion);
    
    // Connect reply signals
    connect(reply, &QNetworkReply::finished, this, &CVEClient::onOSVReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply]() {
        LOG_ERROR(QString("OSV API network error: %1").arg(reply->errorString()));
    });

    // Return empty list - vulnerabilities will be emitted via signals
    return vulnerabilities;
}

QList<CVEClient::Vulnerability> CVEClient::queryGitHub(const QString& packageName, const QString& packageVersion) {
    QList<Vulnerability> vulnerabilities;

    // GitHub Security Advisories API
    // Format: https://api.github.com/advisories?package=<package>
    QUrl url(QString("https://api.github.com/advisories?package=%1").arg(packageName));
    QNetworkRequest request(url);
    
    // GitHub requires authentication for higher rate limits
    QString githubToken = qgetenv("GITHUB_TOKEN");
    if (!githubToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("token %1").arg(githubToken).toUtf8());
    }

    QNetworkReply* reply = m_networkManager->get(request);
    m_pendingRequests[reply] = qMakePair(packageName, packageVersion);
    
    // Connect reply signals
    connect(reply, &QNetworkReply::finished, this, &CVEClient::onGitHubReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply]() {
        LOG_ERROR(QString("GitHub API network error: %1").arg(reply->errorString()));
    });

    // Return empty list - vulnerabilities will be emitted via signals
    return vulnerabilities;
}

CVEClient::Vulnerability CVEClient::parseNVDResponse(const QJsonObject& response) {
    Vulnerability vuln;
    
    QJsonObject cve = response.value("cve").toObject();
    QJsonObject cveMetadata = cve.value("id").toObject();
    vuln.cveId = cveMetadata.value("id").toString();
    
    QJsonArray descriptions = cve.value("descriptions").toArray();
    if (!descriptions.isEmpty()) {
        QJsonObject desc = descriptions.first().toObject();
        vuln.description = desc.value("value").toString();
    }
    
    QJsonObject metrics = cve.value("metrics").toObject();
    if (metrics.contains("cvssMetricV31")) {
        QJsonArray cvssV31 = metrics.value("cvssMetricV31").toArray();
        if (!cvssV31.isEmpty()) {
            QJsonObject cvss = cvssV31.first().toObject();
            QJsonObject cvssData = cvss.value("cvssData").toObject();
            double baseScore = cvssData.value("baseScore").toDouble();
            
            if (baseScore >= 9.0) vuln.severity = "CRITICAL";
            else if (baseScore >= 7.0) vuln.severity = "HIGH";
            else if (baseScore >= 4.0) vuln.severity = "MEDIUM";
            else vuln.severity = "LOW";
        }
    }
    
    QJsonObject published = cve.value("published").toObject();
    vuln.publishedDate = published.value("date").toString();
    
    vuln.source = "NVD";
    return vuln;
}

CVEClient::Vulnerability CVEClient::parseOSVResponse(const QJsonObject& response) {
    Vulnerability vuln;
    
    vuln.cveId = response.value("id").toString();
    vuln.description = response.value("summary").toString();
    
    QJsonObject databaseSpecific = response.value("database_specific").toObject();
    QString severity = databaseSpecific.value("severity").toString();
    if (severity.isEmpty()) {
        severity = "MEDIUM";  // Default
    }
    vuln.severity = severity.toUpper();
    
    vuln.publishedDate = response.value("published").toString();
    
    // Parse affected versions
    QJsonArray affected = response.value("affected").toArray();
    for (const QJsonValue& affectedValue : affected) {
        QJsonObject affectedObj = affectedValue.toObject();
        QJsonArray ranges = affectedObj.value("ranges").toArray();
        for (const QJsonValue& rangeValue : ranges) {
            QJsonObject range = rangeValue.toObject();
            QString rangeType = range.value("type").toString();
            if (rangeType == "SEMVER") {
                QJsonArray events = range.value("events").toArray();
                for (const QJsonValue& eventValue : events) {
                    QJsonObject event = eventValue.toObject();
                    if (event.contains("introduced")) {
                        vuln.affectedVersions.append(event.value("introduced").toString());
                    }
                    if (event.contains("fixed")) {
                        vuln.fixedVersions.append(event.value("fixed").toString());
                    }
                }
            }
        }
    }
    
    vuln.source = "OSV";
    return vuln;
}

CVEClient::Vulnerability CVEClient::parseGitHubResponse(const QJsonObject& response) {
    Vulnerability vuln;
    
    vuln.cveId = response.value("ghsa_id").toString();
    if (vuln.cveId.isEmpty()) {
        vuln.cveId = response.value("cve_id").toString();
    }
    
    vuln.description = response.value("summary").toString();
    vuln.publishedDate = response.value("published_at").toString();
    
    QJsonObject severity = response.value("severity").toObject();
    vuln.severity = severity.value("level").toString().toUpper();
    
    QJsonArray identifiers = response.value("identifiers").toArray();
    for (const QJsonValue& identValue : identifiers) {
        QJsonObject ident = identValue.toObject();
        if (ident.value("type").toString() == "CVE") {
            vuln.cveId = ident.value("value").toString();
            break;
        }
    }
    
    vuln.source = "GITHUB";
    return vuln;
}

bool CVEClient::isVersionAffected(const QString& version, const QStringList& affectedVersions) {
    // Check if version is in affected range
    // This would use semantic versioning comparison
    for (const QString& affected : affectedVersions) {
        if (version == affected) {
            return true;
        }
        // Handle version ranges (e.g., "<2.0.0", ">=1.0.0,<2.0.0")
        // Implementation would parse and compare semantic versions
    }
    return false;
}


#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QNetworkReply>

CVEClient::CVEClient(QObject* parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_nvdEnabled(false)
    , m_osvEnabled(false)
    , m_githubEnabled(false)
{
}

CVEClient::~CVEClient() {
    if (m_networkManager) {
        delete m_networkManager;
    }
}

bool CVEClient::initialize(bool enableNVD, bool enableOSV, bool enableGitHub) {
    m_networkManager = new QNetworkAccessManager(this);
    m_nvdEnabled = enableNVD;
    m_osvEnabled = enableOSV;
    m_githubEnabled = enableGitHub;

    // Connect network reply signals - use direct signal connection
    // Note: QNetworkAccessManager::finished signal will be handled by reply-specific connections

    // Load API key from environment or config
    m_nvdApiKey = qgetenv("NVD_API_KEY");

    LOG_INFO("CVE Client initialized");
    return true;
}

QList<CVEClient::Vulnerability> CVEClient::checkPackage(const QString& packageName,
                                                        const QString& packageVersion,
                                                        const QString& ecosystem) {
    QList<Vulnerability> vulnerabilities;

    if (m_nvdEnabled) {
        QList<Vulnerability> nvdVulns = queryNVD(packageName, packageVersion);
        vulnerabilities.append(nvdVulns);
    }

    if (m_osvEnabled) {
        QList<Vulnerability> osvVulns = queryOSV(packageName, packageVersion, ecosystem);
        vulnerabilities.append(osvVulns);
    }

    if (m_githubEnabled) {
        QList<Vulnerability> githubVulns = queryGitHub(packageName, packageVersion);
        vulnerabilities.append(githubVulns);
    }

    return vulnerabilities;
}

QVariantMap CVEClient::checkPackages(const QList<QVariantMap>& packages) {
    QVariantMap results;

    for (const QVariantMap& package : packages) {
        QString name = package.value("name").toString();
        QString version = package.value("version").toString();
        QString ecosystem = package.value("ecosystem").toString();

        QList<Vulnerability> vulns = checkPackage(name, version, ecosystem);
        
        QVariantList vulnList;
        for (const Vulnerability& vuln : vulns) {
            QVariantMap vulnMap;
            vulnMap["cve_id"] = vuln.cveId;
            vulnMap["description"] = vuln.description;
            vulnMap["severity"] = vuln.severity;
            vulnMap["published_date"] = vuln.publishedDate;
            vulnMap["affected_versions"] = vuln.affectedVersions;
            vulnMap["fixed_versions"] = vuln.fixedVersions;
            vulnMap["source"] = vuln.source;
            vulnList.append(vulnMap);
        }
        
        results[name] = vulnList;
    }

    return results;
}

CVEClient::Vulnerability CVEClient::getCVEDetails(const QString& cveId) {
    Vulnerability vuln;
    vuln.cveId = cveId;

    if (m_nvdEnabled) {
        // Query NVD for CVE details
        QUrl url(QString("https://services.nvd.nist.gov/rest/json/cves/2.0?cveId=%1").arg(cveId));
        QNetworkRequest request(url);
        
        if (!m_nvdApiKey.isEmpty()) {
            request.setRawHeader("apiKey", m_nvdApiKey.toUtf8());
        }
        
        QNetworkReply* reply = m_networkManager->get(request);
        m_pendingRequests[reply] = qMakePair(cveId, "NVD");
        
        // Note: In production, would use async/await or signals
        // For now, this is a placeholder structure
    }

    return vuln;
}

void CVEClient::onNVDReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    QPair<QString, QString> requestInfo = m_pendingRequests.take(reply);
    QString packageName = requestInfo.first;
    QString packageVersion = requestInfo.second;
    
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("NVD API request failed: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isObject()) {
        LOG_WARNING("Invalid NVD API response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray vulnerabilities = root.value("vulnerabilities").toArray();
    
    for (const QJsonValue& vulnValue : vulnerabilities) {
        QJsonObject vulnObj = vulnValue.toObject();
        QJsonObject cve = vulnObj.value("cve").toObject();
        
        Vulnerability vuln = parseNVDResponse(vulnObj);
        if (!vuln.cveId.isEmpty()) {
            emit vulnerabilityFound(vuln);
        }
    }
    
    reply->deleteLater();
}

void CVEClient::onOSVReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    QPair<QString, QString> requestInfo = m_pendingRequests.take(reply);
    QString packageName = requestInfo.first;
    QString packageVersion = requestInfo.second;
    
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("OSV API request failed: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isObject()) {
        LOG_WARNING("Invalid OSV API response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray vulns = root.value("vulns").toArray();
    
    for (const QJsonValue& vulnValue : vulns) {
        QJsonObject vulnObj = vulnValue.toObject();
        Vulnerability vuln = parseOSVResponse(vulnObj);
        if (!vuln.cveId.isEmpty()) {
            emit vulnerabilityFound(vuln);
        }
    }
    
    reply->deleteLater();
}

void CVEClient::onGitHubReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    QPair<QString, QString> requestInfo = m_pendingRequests.take(reply);
    QString packageName = requestInfo.first;
    QString packageVersion = requestInfo.second;
    
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("GitHub API request failed: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isArray()) {
        LOG_WARNING("Invalid GitHub API response");
        reply->deleteLater();
        return;
    }
    
    QJsonArray advisories = doc.array();
    
    for (const QJsonValue& advisoryValue : advisories) {
        QJsonObject advisoryObj = advisoryValue.toObject();
        Vulnerability vuln = parseGitHubResponse(advisoryObj);
        if (!vuln.cveId.isEmpty()) {
            emit vulnerabilityFound(vuln);
        }
    }
    
    reply->deleteLater();
}

QList<CVEClient::Vulnerability> CVEClient::queryNVD(const QString& packageName, const QString& packageVersion) {
    QList<Vulnerability> vulnerabilities;

    // NVD API query
    // Format: https://services.nvd.nist.gov/rest/json/cves/2.0?keywordSearch=<package>
    QUrl url(QString("https://services.nvd.nist.gov/rest/json/cves/2.0?keywordSearch=%1").arg(packageName));
    QNetworkRequest request(url);
    
    if (!m_nvdApiKey.isEmpty()) {
        request.setRawHeader("apiKey", m_nvdApiKey.toUtf8());
    }
    
    QNetworkReply* reply = m_networkManager->get(request);
    m_pendingRequests[reply] = qMakePair(packageName, packageVersion);
    
    // Connect reply signals
    connect(reply, &QNetworkReply::finished, this, &CVEClient::onNVDReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply]() {
        LOG_ERROR(QString("NVD API network error: %1").arg(reply->errorString()));
    });

    // Return empty list - vulnerabilities will be emitted via signals
    return vulnerabilities;
}

QList<CVEClient::Vulnerability> CVEClient::queryOSV(const QString& packageName, 
                                                     const QString& packageVersion,
                                                     const QString& ecosystem) {
    QList<Vulnerability> vulnerabilities;

    // OSV API query
    // Format: POST https://api.osv.dev/v1/query
    QUrl url("https://api.osv.dev/v1/query");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject query;
    query["package"] = QJsonObject{
        {"name", packageName},
        {"ecosystem", ecosystem.isEmpty() ? "UNSPECIFIED" : ecosystem}
    };
    query["version"] = packageVersion;

    QJsonDocument doc(query);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    m_pendingRequests[reply] = qMakePair(packageName, packageVersion);
    
    // Connect reply signals
    connect(reply, &QNetworkReply::finished, this, &CVEClient::onOSVReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply]() {
        LOG_ERROR(QString("OSV API network error: %1").arg(reply->errorString()));
    });

    // Return empty list - vulnerabilities will be emitted via signals
    return vulnerabilities;
}

QList<CVEClient::Vulnerability> CVEClient::queryGitHub(const QString& packageName, const QString& packageVersion) {
    QList<Vulnerability> vulnerabilities;

    // GitHub Security Advisories API
    // Format: https://api.github.com/advisories?package=<package>
    QUrl url(QString("https://api.github.com/advisories?package=%1").arg(packageName));
    QNetworkRequest request(url);
    
    // GitHub requires authentication for higher rate limits
    QString githubToken = qgetenv("GITHUB_TOKEN");
    if (!githubToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("token %1").arg(githubToken).toUtf8());
    }

    QNetworkReply* reply = m_networkManager->get(request);
    m_pendingRequests[reply] = qMakePair(packageName, packageVersion);
    
    // Connect reply signals
    connect(reply, &QNetworkReply::finished, this, &CVEClient::onGitHubReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply]() {
        LOG_ERROR(QString("GitHub API network error: %1").arg(reply->errorString()));
    });

    // Return empty list - vulnerabilities will be emitted via signals
    return vulnerabilities;
}

CVEClient::Vulnerability CVEClient::parseNVDResponse(const QJsonObject& response) {
    Vulnerability vuln;
    
    QJsonObject cve = response.value("cve").toObject();
    QJsonObject cveMetadata = cve.value("id").toObject();
    vuln.cveId = cveMetadata.value("id").toString();
    
    QJsonArray descriptions = cve.value("descriptions").toArray();
    if (!descriptions.isEmpty()) {
        QJsonObject desc = descriptions.first().toObject();
        vuln.description = desc.value("value").toString();
    }
    
    QJsonObject metrics = cve.value("metrics").toObject();
    if (metrics.contains("cvssMetricV31")) {
        QJsonArray cvssV31 = metrics.value("cvssMetricV31").toArray();
        if (!cvssV31.isEmpty()) {
            QJsonObject cvss = cvssV31.first().toObject();
            QJsonObject cvssData = cvss.value("cvssData").toObject();
            double baseScore = cvssData.value("baseScore").toDouble();
            
            if (baseScore >= 9.0) vuln.severity = "CRITICAL";
            else if (baseScore >= 7.0) vuln.severity = "HIGH";
            else if (baseScore >= 4.0) vuln.severity = "MEDIUM";
            else vuln.severity = "LOW";
        }
    }
    
    QJsonObject published = cve.value("published").toObject();
    vuln.publishedDate = published.value("date").toString();
    
    vuln.source = "NVD";
    return vuln;
}

CVEClient::Vulnerability CVEClient::parseOSVResponse(const QJsonObject& response) {
    Vulnerability vuln;
    
    vuln.cveId = response.value("id").toString();
    vuln.description = response.value("summary").toString();
    
    QJsonObject databaseSpecific = response.value("database_specific").toObject();
    QString severity = databaseSpecific.value("severity").toString();
    if (severity.isEmpty()) {
        severity = "MEDIUM";  // Default
    }
    vuln.severity = severity.toUpper();
    
    vuln.publishedDate = response.value("published").toString();
    
    // Parse affected versions
    QJsonArray affected = response.value("affected").toArray();
    for (const QJsonValue& affectedValue : affected) {
        QJsonObject affectedObj = affectedValue.toObject();
        QJsonArray ranges = affectedObj.value("ranges").toArray();
        for (const QJsonValue& rangeValue : ranges) {
            QJsonObject range = rangeValue.toObject();
            QString rangeType = range.value("type").toString();
            if (rangeType == "SEMVER") {
                QJsonArray events = range.value("events").toArray();
                for (const QJsonValue& eventValue : events) {
                    QJsonObject event = eventValue.toObject();
                    if (event.contains("introduced")) {
                        vuln.affectedVersions.append(event.value("introduced").toString());
                    }
                    if (event.contains("fixed")) {
                        vuln.fixedVersions.append(event.value("fixed").toString());
                    }
                }
            }
        }
    }
    
    vuln.source = "OSV";
    return vuln;
}

CVEClient::Vulnerability CVEClient::parseGitHubResponse(const QJsonObject& response) {
    Vulnerability vuln;
    
    vuln.cveId = response.value("ghsa_id").toString();
    if (vuln.cveId.isEmpty()) {
        vuln.cveId = response.value("cve_id").toString();
    }
    
    vuln.description = response.value("summary").toString();
    vuln.publishedDate = response.value("published_at").toString();
    
    QJsonObject severity = response.value("severity").toObject();
    vuln.severity = severity.value("level").toString().toUpper();
    
    QJsonArray identifiers = response.value("identifiers").toArray();
    for (const QJsonValue& identValue : identifiers) {
        QJsonObject ident = identValue.toObject();
        if (ident.value("type").toString() == "CVE") {
            vuln.cveId = ident.value("value").toString();
            break;
        }
    }
    
    vuln.source = "GITHUB";
    return vuln;
}

bool CVEClient::isVersionAffected(const QString& version, const QStringList& affectedVersions) {
    // Check if version is in affected range
    // This would use semantic versioning comparison
    for (const QString& affected : affectedVersions) {
        if (version == affected) {
            return true;
        }
        // Handle version ranges (e.g., "<2.0.0", ">=1.0.0,<2.0.0")
        // Implementation would parse and compare semantic versions
    }
    return false;
}

