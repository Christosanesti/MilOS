#ifndef TOOL_MANAGER_H
#define TOOL_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "tool_discovery.h"

/**
 * @brief Tool Manager
 * 
 * Manages tool operations (launch, favorites, etc.).
 */
class ToolManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<ToolInfo> tools READ tools NOTIFY toolsChanged)
    Q_PROPERTY(QList<ToolInfo> favorites READ favorites NOTIFY favoritesChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)

public:
    explicit ToolManager(QObject* parent = nullptr);
    ~ToolManager();

    /**
     * @brief Initialize tool manager
     * @return true if initialization successful, false otherwise
     */
    Q_INVOKABLE bool initialize();

    /**
     * @brief Get all tools
     */
    QList<ToolInfo> tools() const { return m_tools; }

    /**
     * @brief Get favorite tools
     */
    QList<ToolInfo> favorites() const { return m_favorites; }

    /**
     * @brief Get categories
     */
    QStringList categories() const { return m_categories; }

    /**
     * @brief Launch tool
     * @param toolName Tool name
     * @return true if launch successful, false otherwise
     */
    Q_INVOKABLE bool launchTool(const QString& toolName);

    /**
     * @brief Add tool to favorites
     * @param toolName Tool name
     */
    Q_INVOKABLE void addToFavorites(const QString& toolName);

    /**
     * @brief Remove tool from favorites
     * @param toolName Tool name
     */
    Q_INVOKABLE void removeFromFavorites(const QString& toolName);

    /**
     * @brief Check if tool is favorite
     * @param toolName Tool name
     * @return true if favorite, false otherwise
     */
    Q_INVOKABLE bool isFavorite(const QString& toolName) const;

    /**
     * @brief Get tools by category
     * @param category Category name
     * @return List of tools in category
     */
    Q_INVOKABLE QList<ToolInfo> getToolsByCategory(const QString& category) const;

    /**
     * @brief Search tools
     * @param query Search query
     * @return List of matching tools
     */
    Q_INVOKABLE QList<ToolInfo> searchTools(const QString& query) const;

Q_SIGNALS:
    /**
     * @brief Emitted when tools change
     */
    void toolsChanged();

    /**
     * @brief Emitted when favorites change
     */
    void favoritesChanged();

    /**
     * @brief Emitted when categories change
     */
    void categoriesChanged();

    /**
     * @brief Emitted when tool is launched
     */
    void toolLaunched(const QString& toolName, bool success);

private slots:
    /**
     * @brief Handle tools discovered
     */
    void onToolsDiscovered(const QList<ToolInfo>& tools);

private:
    bool m_initialized;
    ToolDiscovery* m_discovery;
    QList<ToolInfo> m_tools;
    QList<ToolInfo> m_favorites;
    QStringList m_categories;

    /**
     * @brief Load favorites from settings
     */
    void loadFavorites();

    /**
     * @brief Save favorites to settings
     */
    void saveFavorites();
};

#endif // TOOL_MANAGER_H

