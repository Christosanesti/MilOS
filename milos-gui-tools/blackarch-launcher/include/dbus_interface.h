#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "tool_discovery.h"

/**
 * @brief D-Bus Interface for BlackArch Launcher
 * 
 * Provides D-Bus interface org.milos.BlackArchLauncher
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.BlackArchLauncher")

public:
    explicit DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Launch tool
     * @param toolName Tool name
     * @return true if launch successful, false otherwise
     */
    Q_SCRIPTABLE bool LaunchTool(const QString& toolName);

    /**
     * @brief Get list of tools
     * @return List of tool names
     */
    Q_SCRIPTABLE QStringList ListTools();

    /**
     * @brief Get tool information
     * @param toolName Tool name
     * @return Tool information as JSON string
     */
    Q_SCRIPTABLE QString GetToolInfo(const QString& toolName);

    /**
     * @brief Search tools
     * @param query Search query
     * @return List of matching tool names
     */
    Q_SCRIPTABLE QStringList SearchTools(const QString& query);

    /**
     * @brief Get tools by category
     * @param category Category name
     * @return List of tool names in category
     */
    Q_SCRIPTABLE QStringList GetToolsByCategory(const QString& category);

    /**
     * @brief Add tool to favorites
     * @param toolName Tool name
     */
    Q_SCRIPTABLE void AddToFavorites(const QString& toolName);

    /**
     * @brief Remove tool from favorites
     * @param toolName Tool name
     */
    Q_SCRIPTABLE void RemoveFromFavorites(const QString& toolName);

    /**
     * @brief Get favorite tools
     * @return List of favorite tool names
     */
    Q_SCRIPTABLE QStringList GetFavorites();

Q_SIGNALS:
    /**
     * @brief Emitted when tool is launched
     */
    Q_SCRIPTABLE void ToolLaunched(const QString& toolName, bool success);

    /**
     * @brief Set tool manager
     */
    void setToolManager(class ToolManager* toolManager);

private:
    bool m_initialized;
    class ToolManager* m_toolManager;
};

#endif // DBUS_INTERFACE_H

