#include "blackarch_data_scraper.h"
#include <milos/logging/logger.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

BlackArchDataScraper::BlackArchDataScraper(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_networkManager(nullptr)
    , m_githubRepoUrl("https://api.github.com/repos/BlackArch/blackarch/contents/packages")
    , m_websiteUrl("https://www.blackarch.org/tools.html")
    , m_progress(0)
    , m_totalTools(0)
{
    m_dataStoragePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/blackarch-data";
    QDir().mkpath(m_dataStoragePath);
}

BlackArchDataScraper::~BlackArchDataScraper() {
    if (m_networkManager) {
        delete m_networkManager;
    }
}

bool BlackArchDataScraper::initialize() {
    if (m_initialized) {
        return true;
    }

    m_networkManager = new QNetworkAccessManager(this);
    
    // Try to load existing scraped data
    QString jsonPath = m_dataStoragePath + "/tools.json";
    if (QFile::exists(jsonPath)) {
        if (loadFromJson(jsonPath)) {
            LOG_INFO(QString("Loaded %1 tools from cached data").arg(m_scrapedTools.size()));
        }
    }

    m_initialized = true;
    return true;
}

bool BlackArchDataScraper::scrapeAllTools() {
    if (!m_initialized) {
        return false;
    }

    emit scrapingStarted();
    m_progress = 0;
    m_scrapedTools.clear();
    m_toolMap.clear();
    m_categoryMap.clear();

    // Scrape from GitHub repository
    scrapeFromGitHub();
    
    return true;
}

bool BlackArchDataScraper::scrapeFromWebsite() {
    if (!m_initialized) {
        return false;
    }

    emit scrapingStarted();
    m_progress = 0;

    // Fetch tools page from BlackArch website
    QNetworkRequest request(QUrl(m_websiteUrl));
    request.setRawHeader("User-Agent", "MilOS-BlackArch-Scraper/1.0");
    
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            LOG_ERROR(QString("Failed to fetch website: %1").arg(reply->errorString()));
            emit scrapingCompleted(false);
            reply->deleteLater();
            return;
        }

        QString html = QString::fromUtf8(reply->readAll());
        reply->deleteLater();

        // Parse HTML to extract tool information
        // This is a simplified parser - in production, use proper HTML parsing
        QRegularExpression toolRegex(R"(<tr[^>]*>.*?<td[^>]*>([^<]+)</td>.*?<td[^>]*>([^<]+)</td>.*?</tr>)", 
                                     QRegularExpression::DotMatchesEverythingOption);
        
        QRegularExpressionMatchIterator matches = toolRegex.globalMatch(html);
        int count = 0;
        
        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString toolName = match.captured(1).trimmed();
            QString category = match.captured(2).trimmed();
            
            if (!toolName.isEmpty()) {
                BlackArchToolInfo tool;
                tool.name = toolName;
                tool.category = category;
                tool.packageName = toolName;
                
                m_scrapedTools.append(tool);
                m_toolMap[toolName] = tool;
                
                if (!m_categoryMap.contains(category)) {
                    m_categoryMap[category] = QList<BlackArchToolInfo>();
                }
                m_categoryMap[category].append(tool);
                
                count++;
                emit toolScraped(tool);
            }
        }

        LOG_INFO(QString("Scraped %1 tools from website").arg(count));
        emit scrapingCompleted(true);
    });

    return true;
}

void BlackArchDataScraper::scrapeFromGitHub() {
    // Fetch list of packages from GitHub API
    QNetworkRequest request(QUrl(m_githubRepoUrl));
    request.setRawHeader("User-Agent", "MilOS-BlackArch-Scraper/1.0");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &BlackArchDataScraper::onRepositoryListFinished);
}

void BlackArchDataScraper::onRepositoryListFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR(QString("Failed to fetch GitHub repository list: %1").arg(reply->errorString()));
        emit scrapingCompleted(false);
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        LOG_ERROR("Invalid JSON response from GitHub API");
        emit scrapingCompleted(false);
        return;
    }

    QJsonArray items = doc.array();
    m_totalTools = items.size();
    m_progress = 0;

    // Process each package directory
    for (const QJsonValue& item : items) {
        if (!item.isObject()) {
            continue;
        }

        QJsonObject obj = item.toObject();
        QString name = obj.value("name").toString();
        QString type = obj.value("type").toString();
        
        if (type != "dir") {
            continue;
        }

        // Fetch PKGBUILD file for this package
        QString pkgbuildUrl = QString("https://raw.githubusercontent.com/BlackArch/blackarch/master/packages/%1/PKGBUILD").arg(name);
        fetchPKGBUILD(pkgbuildUrl);
    }

    if (m_totalTools == 0) {
        emit scrapingCompleted(true);
    }
}

void BlackArchDataScraper::fetchPKGBUILD(const QString& url) {
    QNetworkRequest request(QUrl(url));
    request.setRawHeader("User-Agent", "MilOS-BlackArch-Scraper/1.0");
    
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &BlackArchDataScraper::onPKGBUILDFinished);
}

void BlackArchDataScraper::onPKGBUILDFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    QString pkgbuildUrl = reply->url().toString();
    QString currentToolName;

    if (reply->error() != QNetworkReply::NoError) {
        // Some PKGBUILDs may not exist, continue
        // Extract tool name from URL
        QUrl url(pkgbuildUrl);
        QStringList pathParts = url.path().split('/');
        if (pathParts.size() >= 3) {
            currentToolName = pathParts[pathParts.size() - 2];  // Package directory name
        }
        reply->deleteLater();
        
        m_progress++;
        int progressPercent = (m_totalTools > 0) ? (m_progress * 100 / m_totalTools) : 0;
        emit scrapingProgress(progressPercent, currentToolName);
        
        if (m_progress >= m_totalTools) {
            QString jsonPath = m_dataStoragePath + "/tools.json";
            saveToJson(jsonPath);
            LOG_INFO(QString("Scraped %1 tools from GitHub repository").arg(m_scrapedTools.size()));
            emit scrapingCompleted(true);
        }
        return;
    }

    QString content = QString::fromUtf8(reply->readAll());
    reply->deleteLater();

    BlackArchToolInfo tool = parsePKGBUILD(content);
    tool.pkgbuildUrl = pkgbuildUrl;
    
    if (!tool.name.isEmpty()) {
        m_scrapedTools.append(tool);
        m_toolMap[tool.name] = tool;
        
        if (!m_categoryMap.contains(tool.category)) {
            m_categoryMap[tool.category] = QList<BlackArchToolInfo>();
        }
        m_categoryMap[tool.category].append(tool);
        
        emit toolScraped(tool);
        currentToolName = tool.name;
    }

    m_progress++;
    int progressPercent = (m_totalTools > 0) ? (m_progress * 100 / m_totalTools) : 0;
    emit scrapingProgress(progressPercent, currentToolName);

    if (m_progress >= m_totalTools) {
        // Save scraped data
        QString jsonPath = m_dataStoragePath + "/tools.json";
        saveToJson(jsonPath);
        
        LOG_INFO(QString("Scraped %1 tools from GitHub repository").arg(m_scrapedTools.size()));
        emit scrapingCompleted(true);
    }
}

BlackArchToolInfo BlackArchDataScraper::parsePKGBUILD(const QString& content) {
    BlackArchToolInfo tool;

    // Extract package name
    QRegularExpression pkgnameRegex(R"(^pkgname=['"]?([^'"]+)['"]?)", QRegularExpression::MultilineOption);
    QRegularExpressionMatch match = pkgnameRegex.match(content);
    if (match.hasMatch()) {
        tool.packageName = match.captured(1).trimmed();
        tool.name = tool.packageName;
    }

    // Extract version
    QRegularExpression pkgverRegex(R"(^pkgver=['"]?([^'"]+)['"]?)", QRegularExpression::MultilineOption);
    match = pkgverRegex.match(content);
    if (match.hasMatch()) {
        tool.version = match.captured(1).trimmed();
    }

    // Extract description
    QRegularExpression pkgdescRegex(R"(^pkgdesc=['"]?([^'"]+)['"]?)", QRegularExpression::MultilineOption);
    match = pkgdescRegex.match(content);
    if (match.hasMatch()) {
        tool.description = match.captured(1).trimmed();
    }

    // Extract category (from groups or url)
    QRegularExpression groupsRegex(R"(^groups=['"]?blackarch-([^'"]+)['"]?)", QRegularExpression::MultilineOption);
    match = groupsRegex.match(content);
    if (match.hasMatch()) {
        tool.category = match.captured(1).trimmed();
        // Capitalize first letter
        if (!tool.category.isEmpty()) {
            tool.category[0] = tool.category[0].toUpper();
        }
    }

    // Extract URL/homepage
    QRegularExpression urlRegex(R"(^url=['"]?([^'"]+)['"]?)", QRegularExpression::MultilineOption);
    match = urlRegex.match(content);
    if (match.hasMatch()) {
        tool.homepage = match.captured(1).trimmed();
    }

    // Extract license
    QRegularExpression licenseRegex(R"(^license=['"]?([^'"]+)['"]?)", QRegularExpression::MultilineOption);
    match = licenseRegex.match(content);
    if (match.hasMatch()) {
        tool.license = match.captured(1).trimmed();
    }

    // Extract dependencies
    QRegularExpression dependsRegex(R"(^depends=\(([^)]+)\))", QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
    match = dependsRegex.match(content);
    if (match.hasMatch()) {
        QString depsStr = match.captured(1);
        // Parse dependencies (remove quotes, split by space)
        QRegularExpression depRegex(R"(['"]?([^'"]+)['"]?)");
        QRegularExpressionMatchIterator depMatches = depRegex.globalMatch(depsStr);
        while (depMatches.hasNext()) {
            QRegularExpressionMatch depMatch = depMatches.next();
            QString dep = depMatch.captured(1).trimmed();
            if (!dep.isEmpty() && dep != "\\") {
                tool.dependencies.append(dep);
            }
        }
    }

    // Store full PKGBUILD metadata
    tool.metadata = extractPKGBUILDMetadata(content);

    return tool;
}

QJsonObject BlackArchDataScraper::extractPKGBUILDMetadata(const QString& content) {
    QJsonObject metadata;
    metadata["pkgbuild_content"] = content;
    // Additional metadata extraction can be added here
    return metadata;
}

BlackArchToolInfo BlackArchDataScraper::getTool(const QString& toolName) const {
    return m_toolMap.value(toolName, BlackArchToolInfo());
}

QList<BlackArchToolInfo> BlackArchDataScraper::getToolsByCategory(const QString& category) const {
    return m_categoryMap.value(category, QList<BlackArchToolInfo>());
}

QStringList BlackArchDataScraper::getCategories() const {
    return m_categoryMap.keys();
}

QList<BlackArchToolInfo> BlackArchDataScraper::searchTools(const QString& query) const {
    QList<BlackArchToolInfo> results;
    QString lowerQuery = query.toLower();

    for (const BlackArchToolInfo& tool : m_scrapedTools) {
        if (tool.name.toLower().contains(lowerQuery) ||
            tool.description.toLower().contains(lowerQuery) ||
            tool.category.toLower().contains(lowerQuery)) {
            results.append(tool);
        }
    }

    return results;
}

bool BlackArchDataScraper::saveToJson(const QString& filePath) {
    QJsonArray toolsArray;
    
    for (const BlackArchToolInfo& tool : m_scrapedTools) {
        QJsonObject toolObj;
        toolObj["name"] = tool.name;
        toolObj["packageName"] = tool.packageName;
        toolObj["description"] = tool.description;
        toolObj["category"] = tool.category;
        toolObj["version"] = tool.version;
        toolObj["homepage"] = tool.homepage;
        toolObj["license"] = tool.license;
        toolObj["repositoryUrl"] = tool.repositoryUrl;
        toolObj["pkgbuildUrl"] = tool.pkgbuildUrl;
        
        QJsonArray depsArray;
        for (const QString& dep : tool.dependencies) {
            depsArray.append(dep);
        }
        toolObj["dependencies"] = depsArray;
        toolObj["metadata"] = tool.metadata;
        
        toolsArray.append(toolObj);
    }

    QJsonDocument doc(toolsArray);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to save JSON file: %1").arg(filePath));
        return false;
    }

    QTextStream stream(&file);
    stream << doc.toJson(QJsonDocument::Indented);
    file.close();

    LOG_INFO(QString("Saved %1 tools to %2").arg(m_scrapedTools.size()).arg(filePath));
    return true;
}

bool BlackArchDataScraper::loadFromJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to load JSON file: %1").arg(filePath));
        return false;
    }

    QTextStream stream(&file);
    QString jsonContent = stream.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        LOG_ERROR("Invalid JSON format");
        return false;
    }

    QJsonArray toolsArray = doc.array();
    m_scrapedTools.clear();
    m_toolMap.clear();
    m_categoryMap.clear();

    for (const QJsonValue& value : toolsArray) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject toolObj = value.toObject();
        BlackArchToolInfo tool;
        tool.name = toolObj.value("name").toString();
        tool.packageName = toolObj.value("packageName").toString();
        tool.description = toolObj.value("description").toString();
        tool.category = toolObj.value("category").toString();
        tool.version = toolObj.value("version").toString();
        tool.homepage = toolObj.value("homepage").toString();
        tool.license = toolObj.value("license").toString();
        tool.repositoryUrl = toolObj.value("repositoryUrl").toString();
        tool.pkgbuildUrl = toolObj.value("pkgbuildUrl").toString();
        tool.metadata = toolObj.value("metadata").toObject();

        QJsonArray depsArray = toolObj.value("dependencies").toArray();
        for (const QJsonValue& depValue : depsArray) {
            tool.dependencies.append(depValue.toString());
        }

        m_scrapedTools.append(tool);
        m_toolMap[tool.name] = tool;
        
        if (!m_categoryMap.contains(tool.category)) {
            m_categoryMap[tool.category] = QList<BlackArchToolInfo>();
        }
        m_categoryMap[tool.category].append(tool);
    }

    LOG_INFO(QString("Loaded %1 tools from JSON file").arg(m_scrapedTools.size()));
    return true;
}

BlackArchToolInfo BlackArchDataScraper::scrapeToolFromPKGBUILD(const QString& pkgbuildUrl) {
    // This method is async - fetch PKGBUILD and result comes via signal
    fetchPKGBUILD(pkgbuildUrl);
    // Return empty tool info - actual result will be emitted via toolScraped signal
    return BlackArchToolInfo();
}

