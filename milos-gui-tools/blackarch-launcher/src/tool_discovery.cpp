#include "tool_discovery.h"
#include <milos/logging/logger.h>
#include <blackarch-integration/blackarch_repository.h>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>

Q_DECLARE_METATYPE(ToolInfo)
Q_DECLARE_METATYPE(QList<ToolInfo>)

ToolDiscovery::ToolDiscovery(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_repository(nullptr)
{
}

ToolDiscovery::~ToolDiscovery() {
}

bool ToolDiscovery::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize BlackArch repository (data scraper)
    m_repository = new BlackArchRepository(this);
    if (!m_repository->initialize()) {
        LOG_WARNING("Failed to initialize BlackArch repository, using fallback tool list");
        // Continue with fallback
    } else {
        // Ensure data is available
        if (!m_repository->isRepositoryConfigured()) {
            LOG_INFO("BlackArch data not found, triggering data scraping...");
            m_repository->configureRepository();
        }
    }

    // Discover tools
    m_tools = discoverTools();

    // Build tool map
    for (const ToolInfo& tool : m_tools) {
        m_toolMap[tool.name] = tool;
        
        // Build category map
        if (!m_categoryMap.contains(tool.category)) {
            m_categoryMap[tool.category] = QList<ToolInfo>();
        }
        m_categoryMap[tool.category].append(tool);
    }

    m_initialized = true;
    emit toolsDiscovered(m_tools);
    return true;
}

QList<ToolInfo> ToolDiscovery::discoverTools() {
    QList<ToolInfo> tools;

    // Try to get tools from scraped data
    if (m_repository && m_repository->isRepositoryConfigured()) {
        QStringList availableTools = m_repository->getAvailableTools();
        
        for (const QString& toolName : availableTools) {
            // Get tool info from repository
            QString toolInfoJson = m_repository->getToolInfo(toolName);
            if (toolInfoJson.isEmpty()) {
                continue;
            }

            // Parse JSON
            QJsonDocument doc = QJsonDocument::fromJson(toolInfoJson.toUtf8());
            if (doc.isNull() || !doc.isObject()) {
                continue;
            }

            QJsonObject toolObj = doc.object();
            ToolInfo tool;
            tool.name = toolObj.value("name").toString();
            tool.displayName = toolObj.value("packageName").toString();
            tool.description = toolObj.value("description").toString();
            tool.category = toolObj.value("category").toString();
            tool.executable = getToolExecutable(toolName);
            tool.isInstalled = isToolInstalled(toolName);
            tool.isFavorite = false;  // Load from settings
            tool.tags = QStringList() << toolName << tool.category.toLower();

            tools.append(tool);
        }

        LOG_INFO(QString("Discovered %1 tools from scraped data").arg(tools.size()));
    } else {
        // Fallback to curated list if scraper not available
        LOG_WARNING("BlackArch repository not available, using fallback tool list");
        
        QStringList selectedTools = {
            "nmap", "wireshark-cli", "john", "hashcat", "sqlmap",
            "aircrack-ng", "nikto", "hydra", "tcpdump", "ettercap",
            "metasploit", "burpsuite", "openvas"
        };

        QMap<QString, QString> toolCategories = {
            {"nmap", "Network Scanning"},
            {"wireshark-cli", "Network Analysis"},
            {"john", "Password Cracking"},
            {"hashcat", "Password Cracking"},
            {"sqlmap", "Web Application Security"},
            {"aircrack-ng", "Wireless Security"},
            {"nikto", "Web Vulnerability Scanning"},
            {"hydra", "Password Cracking"},
            {"tcpdump", "Network Analysis"},
            {"ettercap", "Network Security"},
            {"metasploit", "Exploitation Framework"},
            {"burpsuite", "Web Application Security"},
            {"openvas", "Vulnerability Scanning"}
        };

        QMap<QString, QString> toolDescriptions = {
            {"nmap", "Network exploration and security auditing tool"},
            {"wireshark-cli", "Network protocol analyzer (command-line)"},
            {"john", "Password cracker for various hash types"},
            {"hashcat", "Advanced password recovery tool with GPU acceleration"},
            {"sqlmap", "Automatic SQL injection and database takeover tool"},
            {"aircrack-ng", "Wireless network security auditing tool"},
            {"nikto", "Web server scanner"},
            {"hydra", "Password brute force tool"},
            {"tcpdump", "Network packet analyzer"},
            {"ettercap", "Network security tool for man-in-the-middle attacks"},
            {"metasploit", "Penetration testing framework"},
            {"burpsuite", "Web application security testing platform"},
            {"openvas", "Vulnerability scanner and manager"}
        };

        for (const QString& toolName : selectedTools) {
            ToolInfo tool;
            tool.name = toolName;
            tool.displayName = toolName;
            tool.description = toolDescriptions.value(toolName, "BlackArch security tool");
            tool.category = toolCategories.value(toolName, "Other");
            tool.executable = getToolExecutable(toolName);
            tool.isInstalled = isToolInstalled(toolName);
            tool.isFavorite = false;
            tool.tags = QStringList() << toolName << tool.category.toLower();

            tools.append(tool);
        }
    }

    // Load favorites from settings
    QSettings settings;
    settings.beginGroup("BlackArchLauncher");
    QStringList favorites = settings.value("favorites", QStringList()).toStringList();
    settings.endGroup();

    for (ToolInfo& tool : tools) {
        if (favorites.contains(tool.name)) {
            tool.isFavorite = true;
        }
    }

    return tools;
    QSettings settings;
    settings.beginGroup("BlackArchLauncher");
    QStringList favorites = settings.value("favorites", QStringList()).toStringList();
    settings.endGroup();

    for (ToolInfo& tool : tools) {
        if (favorites.contains(tool.name)) {
            tool.isFavorite = true;
        }
    }

    return tools;
}

ToolInfo ToolDiscovery::getToolInfo(const QString& toolName) const {
    return m_toolMap.value(toolName, ToolInfo());
}

QList<ToolInfo> ToolDiscovery::getToolsByCategory(const QString& category) const {
    return m_categoryMap.value(category, QList<ToolInfo>());
}

QStringList ToolDiscovery::getCategories() const {
    return m_categoryMap.keys();
}

QList<ToolInfo> ToolDiscovery::searchTools(const QString& query) const {
    QList<ToolInfo> results;
    QString lowerQuery = query.toLower();

    for (const ToolInfo& tool : m_tools) {
        if (tool.name.toLower().contains(lowerQuery) ||
            tool.displayName.toLower().contains(lowerQuery) ||
            tool.description.toLower().contains(lowerQuery) ||
            tool.category.toLower().contains(lowerQuery)) {
            results.append(tool);
        }
    }

    return results;
}

ToolInfo ToolDiscovery::discoverToolFromPackage(const QString& packageName) {
    ToolInfo tool;
    tool.name = packageName;
    tool.isInstalled = isToolInstalled(packageName);
    return tool;
}

QString ToolDiscovery::getToolCategory(const QString& packageName) const {
    // Category mapping based on package name patterns
    if (packageName.contains("web") || packageName.contains("http")) {
        return "Web Application Security";
    } else if (packageName.contains("wireless") || packageName.contains("wifi")) {
        return "Wireless Security";
    } else if (packageName.contains("password") || packageName.contains("crack")) {
        return "Password Cracking";
    } else if (packageName.contains("network") || packageName.contains("scan")) {
        return "Network Scanning";
    } else if (packageName.contains("exploit") || packageName.contains("metasploit")) {
        return "Exploitation Framework";
    }
    return "Other";
}

bool ToolDiscovery::isToolInstalled(const QString& toolName) const {
    // Check if tool executable exists
    QString executable = getToolExecutable(toolName);
    if (!executable.isEmpty()) {
        QFileInfo fileInfo(executable);
        return fileInfo.exists() && fileInfo.isExecutable();
    }

    // Check via which command
    QProcess process;
    process.start("which", QStringList() << toolName);
    process.waitForFinished();
    return process.exitCode() == 0;
}

QString ToolDiscovery::getToolExecutable(const QString& toolName) const {
    // Check common paths
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    paths << "/usr/bin" << "/usr/local/bin" << QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "/.local/bin";

    for (const QString& path : paths) {
        QString executable = path + "/" + toolName;
        QFileInfo fileInfo(executable);
        if (fileInfo.exists() && fileInfo.isExecutable()) {
            return executable;
        }
    }

    // Try which command
    QProcess process;
    process.start("which", QStringList() << toolName);
    process.waitForFinished();
    if (process.exitCode() == 0) {
        return QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    }

    return QString();
}

