#ifndef BLACKARCH_DATA_SCRAPER_H
#define BLACKARCH_DATA_SCRAPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief BlackArch Tool Information
 */
struct BlackArchToolInfo {
    QString name;
    QString packageName;
    QString description;
    QString category;
    QString version;
    QStringList dependencies;
    QString homepage;
    QString license;
    QString repositoryUrl;
    QString pkgbuildUrl;
    QJsonObject metadata;  // Additional metadata from PKGBUILD
};

/**
 * @brief BlackArch Data Scraper
 * 
 * Scrapes tool information from BlackArch repositories and tool lists.
 * Does NOT install tools, only extracts metadata.
 */
class BlackArchDataScraper : public QObject {
    Q_OBJECT

public:
    explicit BlackArchDataScraper(QObject* parent = nullptr);
    ~BlackArchDataScraper();

    /**
     * @brief Initialize scraper
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Scrape all tools from BlackArch GitHub repository
     * @return true if scraping successful, false otherwise
     */
    Q_INVOKABLE bool scrapeAllTools();

    /**
     * @brief Scrape tools from BlackArch website tools list
     * @return true if scraping successful, false otherwise
     */
    Q_INVOKABLE bool scrapeFromWebsite();

    /**
     * @brief Scrape tool metadata from PKGBUILD file
     * @param pkgbuildUrl URL to PKGBUILD file
     * @return Tool information
     */
    Q_INVOKABLE BlackArchToolInfo scrapeToolFromPKGBUILD(const QString& pkgbuildUrl);

    /**
     * @brief Get scraped tools
     * @return List of scraped tool information
     */
    QList<BlackArchToolInfo> getScrapedTools() const { return m_scrapedTools; }

    /**
     * @brief Get tool by name
     * @param toolName Tool name
     * @return Tool information
     */
    BlackArchToolInfo getTool(const QString& toolName) const;

    /**
     * @brief Get tools by category
     * @param category Category name
     * @return List of tools in category
     */
    QList<BlackArchToolInfo> getToolsByCategory(const QString& category) const;

    /**
     * @brief Get all categories
     * @return List of category names
     */
    QStringList getCategories() const;

    /**
     * @brief Search tools
     * @param query Search query
     * @return List of matching tools
     */
    QList<BlackArchToolInfo> searchTools(const QString& query) const;

    /**
     * @brief Save scraped data to JSON file
     * @param filePath File path to save to
     * @return true if save successful, false otherwise
     */
    Q_INVOKABLE bool saveToJson(const QString& filePath);

    /**
     * @brief Load scraped data from JSON file
     * @param filePath File path to load from
     * @return true if load successful, false otherwise
     */
    Q_INVOKABLE bool loadFromJson(const QString& filePath);

    /**
     * @brief Get scraping progress (0-100)
     */
    int getProgress() const { return m_progress; }

Q_SIGNALS:
    /**
     * @brief Emitted when scraping starts
     */
    void scrapingStarted();

    /**
     * @brief Emitted when scraping completes
     */
    void scrapingCompleted(bool success);

    /**
     * @brief Emitted when scraping progress changes
     */
    void scrapingProgress(int progress, const QString& currentTool);

    /**
     * @brief Emitted when tool is scraped
     */
    void toolScraped(const BlackArchToolInfo& tool);

private slots:
    void onRepositoryListFinished();
    void onPKGBUILDFinished();

private:
    /**
     * @brief Scrape tools from GitHub repository
     */
    void scrapeFromGitHub();

    /**
     * @brief Parse PKGBUILD content
     * @param content PKGBUILD file content
     * @return Tool information
     */
    BlackArchToolInfo parsePKGBUILD(const QString& content);

    /**
     * @brief Extract tool metadata from PKGBUILD
     */
    QJsonObject extractPKGBUILDMetadata(const QString& content);

    /**
     * @brief Fetch PKGBUILD file from URL
     */
    void fetchPKGBUILD(const QString& url);

    bool m_initialized;
    QNetworkAccessManager* m_networkManager;
    QList<BlackArchToolInfo> m_scrapedTools;
    QMap<QString, BlackArchToolInfo> m_toolMap;
    QMap<QString, QList<BlackArchToolInfo>> m_categoryMap;
    QString m_githubRepoUrl;
    QString m_websiteUrl;
    QString m_dataStoragePath;
    int m_progress;
    int m_totalTools;
    QString m_currentTool;
};

Q_DECLARE_METATYPE(BlackArchToolInfo)
Q_DECLARE_METATYPE(QList<BlackArchToolInfo>)

#endif // BLACKARCH_DATA_SCRAPER_H

