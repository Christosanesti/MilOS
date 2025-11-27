#ifndef MILOS_LOGGING_CVE_CLIENT_H
#define MILOS_LOGGING_CVE_CLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief CVE Database Client
 * 
 * Integrates with CVE databases (NVD, OSV) to check for vulnerabilities
 * in software dependencies.
 */
class CVEClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Vulnerability information
     */
    struct Vulnerability {
        QString cveId;
        QString description;
        QString severity;  // "CRITICAL", "HIGH", "MEDIUM", "LOW"
        QString publishedDate;
        QStringList affectedVersions;
        QStringList fixedVersions;
        QString source;  // "NVD", "OSV", "GITHUB"
    };

    explicit CVEClient(QObject* parent = nullptr);
    ~CVEClient();

    /**
     * @brief Initialize CVE client
     * @param enableNVD Enable NVD API integration
     * @param enableOSV Enable OSV API integration
     * @param enableGitHub Enable GitHub Security Advisories
     * @return true if initialization successful
     */
    bool initialize(bool enableNVD = true, bool enableOSV = true, bool enableGitHub = true);

    /**
     * @brief Check package for vulnerabilities
     * @param packageName Package name
     * @param packageVersion Package version
     * @param ecosystem Ecosystem (e.g., "npm", "pypi", "cargo", "maven")
     * @return List of vulnerabilities
     */
    QList<Vulnerability> checkPackage(const QString& packageName, 
                                      const QString& packageVersion,
                                      const QString& ecosystem = "");

    /**
     * @brief Check multiple packages
     * @param packages List of packages (name, version, ecosystem)
     * @return Map of package name to vulnerabilities
     */
    QVariantMap checkPackages(const QList<QVariantMap>& packages);

    /**
     * @brief Get CVE details
     * @param cveId CVE identifier (e.g., "CVE-2021-44228")
     * @return Vulnerability details
     */
    Vulnerability getCVEDetails(const QString& cveId);

Q_SIGNALS:
    /**
     * @brief Emitted when vulnerability is found
     */
    void vulnerabilityFound(const Vulnerability& vulnerability);

private slots:
    void onNVDReplyFinished();
    void onOSVReplyFinished();
    void onGitHubReplyFinished();

private:
    QList<Vulnerability> queryNVD(const QString& packageName, const QString& packageVersion);
    QList<Vulnerability> queryOSV(const QString& packageName, const QString& packageVersion, const QString& ecosystem);
    QList<Vulnerability> queryGitHub(const QString& packageName, const QString& packageVersion);
    Vulnerability parseNVDResponse(const QJsonObject& response);
    Vulnerability parseOSVResponse(const QJsonObject& response);
    Vulnerability parseGitHubResponse(const QJsonObject& response);
    bool isVersionAffected(const QString& version, const QStringList& affectedVersions);

Q_SIGNALS:
    /**
     * @brief Emitted when vulnerability is found
     */
    void vulnerabilityFound(const Vulnerability& vulnerability);

private:
    QNetworkAccessManager* m_networkManager;
    bool m_nvdEnabled;
    bool m_osvEnabled;
    bool m_githubEnabled;
    QString m_nvdApiKey;  // Optional API key for higher rate limits
    QHash<QNetworkReply*, QPair<QString, QString>> m_pendingRequests;  // reply -> (package, version)
};

Q_DECLARE_METATYPE(CVEClient::Vulnerability)

#endif // MILOS_LOGGING_CVE_CLIENT_H



#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief CVE Database Client
 * 
 * Integrates with CVE databases (NVD, OSV) to check for vulnerabilities
 * in software dependencies.
 */
class CVEClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Vulnerability information
     */
    struct Vulnerability {
        QString cveId;
        QString description;
        QString severity;  // "CRITICAL", "HIGH", "MEDIUM", "LOW"
        QString publishedDate;
        QStringList affectedVersions;
        QStringList fixedVersions;
        QString source;  // "NVD", "OSV", "GITHUB"
    };

    explicit CVEClient(QObject* parent = nullptr);
    ~CVEClient();

    /**
     * @brief Initialize CVE client
     * @param enableNVD Enable NVD API integration
     * @param enableOSV Enable OSV API integration
     * @param enableGitHub Enable GitHub Security Advisories
     * @return true if initialization successful
     */
    bool initialize(bool enableNVD = true, bool enableOSV = true, bool enableGitHub = true);

    /**
     * @brief Check package for vulnerabilities
     * @param packageName Package name
     * @param packageVersion Package version
     * @param ecosystem Ecosystem (e.g., "npm", "pypi", "cargo", "maven")
     * @return List of vulnerabilities
     */
    QList<Vulnerability> checkPackage(const QString& packageName, 
                                      const QString& packageVersion,
                                      const QString& ecosystem = "");

    /**
     * @brief Check multiple packages
     * @param packages List of packages (name, version, ecosystem)
     * @return Map of package name to vulnerabilities
     */
    QVariantMap checkPackages(const QList<QVariantMap>& packages);

    /**
     * @brief Get CVE details
     * @param cveId CVE identifier (e.g., "CVE-2021-44228")
     * @return Vulnerability details
     */
    Vulnerability getCVEDetails(const QString& cveId);

Q_SIGNALS:
    /**
     * @brief Emitted when vulnerability is found
     */
    void vulnerabilityFound(const Vulnerability& vulnerability);

private slots:
    void onNVDReplyFinished();
    void onOSVReplyFinished();
    void onGitHubReplyFinished();

private:
    QList<Vulnerability> queryNVD(const QString& packageName, const QString& packageVersion);
    QList<Vulnerability> queryOSV(const QString& packageName, const QString& packageVersion, const QString& ecosystem);
    QList<Vulnerability> queryGitHub(const QString& packageName, const QString& packageVersion);
    Vulnerability parseNVDResponse(const QJsonObject& response);
    Vulnerability parseOSVResponse(const QJsonObject& response);
    Vulnerability parseGitHubResponse(const QJsonObject& response);
    bool isVersionAffected(const QString& version, const QStringList& affectedVersions);

Q_SIGNALS:
    /**
     * @brief Emitted when vulnerability is found
     */
    void vulnerabilityFound(const Vulnerability& vulnerability);

private:
    QNetworkAccessManager* m_networkManager;
    bool m_nvdEnabled;
    bool m_osvEnabled;
    bool m_githubEnabled;
    QString m_nvdApiKey;  // Optional API key for higher rate limits
    QHash<QNetworkReply*, QPair<QString, QString>> m_pendingRequests;  // reply -> (package, version)
};

Q_DECLARE_METATYPE(CVEClient::Vulnerability)

#endif // MILOS_LOGGING_CVE_CLIENT_H

