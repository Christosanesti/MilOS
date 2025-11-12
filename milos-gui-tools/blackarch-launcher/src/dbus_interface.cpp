#include "dbus_interface.h"
#include "tool_manager.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_toolManager(nullptr)
{
}

DBusInterface::~DBusInterface() {
}

void DBusInterface::setToolManager(class ToolManager* toolManager) {
    m_toolManager = toolManager;
}

bool DBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    // Register D-Bus service
    if (!QDBusConnection::sessionBus().registerService("org.milos.BlackArchLauncher")) {
        std::cerr << "Failed to register D-Bus service" << std::endl;
        return false;
    }

    // Register object
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/BlackArchLauncher", this,
                                                      QDBusConnection::ExportAllContents)) {
        std::cerr << "Failed to register D-Bus object" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool DBusInterface::LaunchTool(const QString& toolName) {
    if (!m_toolManager) {
        return false;
    }

    bool success = m_toolManager->launchTool(toolName);
    emit ToolLaunched(toolName, success);
    return success;
}

QStringList DBusInterface::ListTools() {
    if (!m_toolManager) {
        return QStringList();
    }

    QStringList toolNames;
    QList<ToolInfo> tools = m_toolManager->tools();
    for (const ToolInfo& tool : tools) {
        toolNames.append(tool.name);
    }
    return toolNames;
}

QString DBusInterface::GetToolInfo(const QString& toolName) {
    if (!m_toolManager) {
        return QString();
    }

    // Get tool info from tool manager
    QList<ToolInfo> tools = m_toolManager->tools();
    for (const ToolInfo& tool : tools) {
        if (tool.name == toolName) {
            QJsonObject obj;
            obj["name"] = tool.name;
            obj["displayName"] = tool.displayName;
            obj["description"] = tool.description;
            obj["category"] = tool.category;
            obj["executable"] = tool.executable;
            obj["isInstalled"] = tool.isInstalled;
            obj["isFavorite"] = tool.isFavorite;
            
            QJsonDocument doc(obj);
            return QString::fromUtf8(doc.toJson());
        }
    }

    return QString();
}

QStringList DBusInterface::SearchTools(const QString& query) {
    if (!m_toolManager) {
        return QStringList();
    }

    QStringList toolNames;
    QList<ToolInfo> tools = m_toolManager->searchTools(query);
    for (const ToolInfo& tool : tools) {
        toolNames.append(tool.name);
    }
    return toolNames;
}

QStringList DBusInterface::GetToolsByCategory(const QString& category) {
    if (!m_toolManager) {
        return QStringList();
    }

    QStringList toolNames;
    QList<ToolInfo> tools = m_toolManager->getToolsByCategory(category);
    for (const ToolInfo& tool : tools) {
        toolNames.append(tool.name);
    }
    return toolNames;
}

void DBusInterface::AddToFavorites(const QString& toolName) {
    if (!m_toolManager) {
        return;
    }

    m_toolManager->addToFavorites(toolName);
}

void DBusInterface::RemoveFromFavorites(const QString& toolName) {
    if (!m_toolManager) {
        return;
    }

    m_toolManager->removeFromFavorites(toolName);
}

QStringList DBusInterface::GetFavorites() {
    if (!m_toolManager) {
        return QStringList();
    }

    QStringList toolNames;
    QList<ToolInfo> favorites = m_toolManager->favorites();
    for (const ToolInfo& tool : favorites) {
        toolNames.append(tool.name);
    }
    return toolNames;
}

