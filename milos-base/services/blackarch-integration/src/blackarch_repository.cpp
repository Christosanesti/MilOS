#include "blackarch_repository.h"
#include "blackarch_data_scraper.h"
#include <milos/logging/logger.h>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BlackArchRepository::BlackArchRepository(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_dataScraper(nullptr)
{
    m_dataStoragePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/blackarch-data";
    QDir().mkpath(m_dataStoragePath);
}

BlackArchRepository::~BlackArchRepository() {
}

bool BlackArchRepository::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize data scraper
    m_dataScraper = new BlackArchDataScraper(this);
    if (!m_dataScraper->initialize()) {
        LOG_ERROR("Failed to initialize BlackArch data scraper");
        return false;
    }

    // Connect signals
    connect(m_dataScraper, &BlackArchDataScraper::scrapingStarted,
            this, &BlackArchRepository::dataScrapingStarted);
    connect(m_dataScraper, &BlackArchDataScraper::scrapingCompleted,
            this, &BlackArchRepository::dataScrapingCompleted);
    connect(m_dataScraper, &BlackArchDataScraper::scrapingProgress,
            this, &BlackArchRepository::dataScrapingProgress);

    // Try to load existing scraped data
    QString jsonPath = m_dataStoragePath + "/tools.json";
    if (QFile::exists(jsonPath)) {
        if (m_dataScraper->loadFromJson(jsonPath)) {
            LOG_INFO(QString("Loaded %1 tools from cached data").arg(m_dataScraper->getScrapedTools().size()));
        }
    }

    m_initialized = true;
    return true;
}

bool BlackArchRepository::isRepositoryConfigured() const {
    // Check if we have scraped data available
    QString jsonPath = m_dataStoragePath + "/tools.json";
    return QFile::exists(jsonPath) && !m_dataScraper->getScrapedTools().isEmpty();
}

bool BlackArchRepository::configureRepository() {
    // This now triggers data scraping instead of pacman configuration
    if (!m_initialized || !m_dataScraper) {
        return false;
    }

    // Start scraping if data not available
    if (!isRepositoryConfigured()) {
        LOG_INFO("Starting BlackArch data scraping...");
        return m_dataScraper->scrapeAllTools();
    }

    return true;
}

QStringList BlackArchRepository::getAvailableTools() const {
    if (!m_initialized || !m_dataScraper) {
        return QStringList();
    }

    QStringList tools;
    QList<BlackArchToolInfo> scrapedTools = m_dataScraper->getScrapedTools();
    for (const BlackArchToolInfo& tool : scrapedTools) {
        tools.append(tool.name);
    }
    return tools;
}

QString BlackArchRepository::getToolInfo(const QString& toolName) const {
    if (!m_initialized || !m_dataScraper) {
        return QString();
    }

    BlackArchToolInfo tool = m_dataScraper->getTool(toolName);
    if (tool.name.isEmpty()) {
        return QString();
    }

    // Convert to JSON
    QJsonObject toolObj;
    toolObj["name"] = tool.name;
    toolObj["packageName"] = tool.packageName;
    toolObj["description"] = tool.description;
    toolObj["category"] = tool.category;
    toolObj["version"] = tool.version;
    toolObj["homepage"] = tool.homepage;
    toolObj["license"] = tool.license;
    toolObj["pkgbuildUrl"] = tool.pkgbuildUrl;
    
    QJsonArray depsArray;
    for (const QString& dep : tool.dependencies) {
        depsArray.append(dep);
    }
    toolObj["dependencies"] = depsArray;
    toolObj["metadata"] = tool.metadata;

    QJsonDocument doc(toolObj);
    return QString::fromUtf8(doc.toJson());
}

QStringList BlackArchRepository::getToolsByCategory(const QString& category) const {
    if (!m_initialized || !m_dataScraper) {
        return QStringList();
    }

    QStringList tools;
    QList<BlackArchToolInfo> categoryTools = m_dataScraper->getToolsByCategory(category);
    for (const BlackArchToolInfo& tool : categoryTools) {
        tools.append(tool.name);
    }
    return tools;
}

QStringList BlackArchRepository::getCategories() const {
    if (!m_initialized || !m_dataScraper) {
        return QStringList();
    }

    return m_dataScraper->getCategories();
}

QStringList BlackArchRepository::searchTools(const QString& query) const {
    if (!m_initialized || !m_dataScraper) {
        return QStringList();
    }

    QStringList tools;
    QList<BlackArchToolInfo> results = m_dataScraper->searchTools(query);
    for (const BlackArchToolInfo& tool : results) {
        tools.append(tool.name);
    }
    return tools;
}

bool BlackArchRepository::refreshData() {
    if (!m_initialized || !m_dataScraper) {
        return false;
    }

    LOG_INFO("Refreshing BlackArch tool data...");
    return m_dataScraper->scrapeAllTools();
}
