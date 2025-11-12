#ifndef TOOL_DISCOVERY_H
#define TOOL_DISCOVERY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QProcess>

/**
 * @brief Tool Information
 */
struct ToolInfo {
    ToolInfo() : isInstalled(false), isFavorite(false) {}
    
    bool operator==(const ToolInfo& other) const {
        return name == other.name;
    }
    QString name;
    QString displayName;
    QString description;
    QString category;
    QString executable;
    QString icon;
    bool isInstalled;
    bool isFavorite;
    QStringList tags;
};

Q_DECLARE_METATYPE(ToolInfo)
Q_DECLARE_METATYPE(QList<ToolInfo>)

/**
 * @brief Tool Discovery
 * 
 * Discovers installed BlackArch tools and provides tool information.
 */
class ToolDiscovery : public QObject {
    Q_OBJECT

public:
    explicit ToolDiscovery(QObject* parent = nullptr);
    ~ToolDiscovery();

    /**
     * @brief Initialize tool discovery
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Discover installed tools
     * @return List of discovered tools
     */
    QList<ToolInfo> discoverTools();

    /**
     * @brief Get tool information
     * @param toolName Tool name
     * @return Tool information
     */
    ToolInfo getToolInfo(const QString& toolName) const;

    /**
     * @brief Get tools by category
     * @param category Category name
     * @return List of tools in category
     */
    QList<ToolInfo> getToolsByCategory(const QString& category) const;

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
    QList<ToolInfo> searchTools(const QString& query) const;

Q_SIGNALS:
    /**
     * @brief Emitted when tools are discovered
     */
    void toolsDiscovered(const QList<ToolInfo>& tools);

private:
    bool m_initialized;
    QList<ToolInfo> m_tools;
    QMap<QString, ToolInfo> m_toolMap;
    QMap<QString, QList<ToolInfo>> m_categoryMap;

    /**
     * @brief Discover tool from package
     */
    ToolInfo discoverToolFromPackage(const QString& packageName);

    /**
     * @brief Get tool category from package
     */
    QString getToolCategory(const QString& packageName) const;

    /**
     * @brief Check if tool is installed
     */
    bool isToolInstalled(const QString& toolName) const;

    /**
     * @brief Get tool executable path
     */
    QString getToolExecutable(const QString& toolName) const;
};

#endif // TOOL_DISCOVERY_H

