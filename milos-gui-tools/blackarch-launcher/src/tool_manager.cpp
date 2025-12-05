#include "tool_manager.h"
#include "tool_discovery.h"
#include <milos/logging/logger.h>
#include <QProcess>
#include <QSettings>

ToolManager::ToolManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_discovery(nullptr)
{
}

ToolManager::~ToolManager() {
}

bool ToolManager::initialize() {
    if (m_initialized) {
        return true;
    }

    m_discovery = new ToolDiscovery(this);
    connect(m_discovery, &ToolDiscovery::toolsDiscovered,
            this, &ToolManager::onToolsDiscovered);

    if (!m_discovery->initialize()) {
        LOG_ERROR("Failed to initialize tool discovery");
        return false;
    }

    loadFavorites();

    m_initialized = true;
    return true;
}

bool ToolManager::launchTool(const QString& toolName) {
    if (!m_discovery) {
        return false;
    }

    ToolInfo tool = m_discovery->getToolInfo(toolName);
    if (tool.name.isEmpty()) {
        LOG_WARNING(QString("Tool not found: %1").arg(toolName));
        emit toolLaunched(toolName, false);
        return false;
    }

    if (!tool.isInstalled) {
        LOG_WARNING(QString("Tool not installed: %1").arg(toolName));
        emit toolLaunched(toolName, false);
        return false;
    }

    // Launch tool
    QProcess* process = new QProcess(this);
    process->setProgram(tool.executable);
    
    bool started = process->startDetached();
    
    if (started) {
        LOG_INFO(QString("Launched tool: %1").arg(toolName));
        emit toolLaunched(toolName, true);
    } else {
        LOG_ERROR(QString("Failed to launch tool: %1").arg(toolName));
        emit toolLaunched(toolName, false);
    }

    return started;
}

void ToolManager::addToFavorites(const QString& toolName) {
    if (isFavorite(toolName)) {
        return;
    }

    ToolInfo tool = m_discovery->getToolInfo(toolName);
    if (tool.name.isEmpty()) {
        return;
    }

    m_favorites.append(tool);
    saveFavorites();
    emit favoritesChanged();
}

void ToolManager::removeFromFavorites(const QString& toolName) {
    for (int i = 0; i < m_favorites.size(); i++) {
        if (m_favorites[i].name == toolName) {
            m_favorites.removeAt(i);
            saveFavorites();
            emit favoritesChanged();
            return;
        }
    }
}

bool ToolManager::isFavorite(const QString& toolName) const {
    for (const ToolInfo& tool : m_favorites) {
        if (tool.name == toolName) {
            return true;
        }
    }
    return false;
}

QList<ToolInfo> ToolManager::getToolsByCategory(const QString& category) const {
    if (!m_discovery) {
        return QList<ToolInfo>();
    }
    return m_discovery->getToolsByCategory(category);
}

QList<ToolInfo> ToolManager::searchTools(const QString& query) const {
    if (!m_discovery) {
        return QList<ToolInfo>();
    }
    return m_discovery->searchTools(query);
}

void ToolManager::onToolsDiscovered(const QList<ToolInfo>& tools) {
    m_tools = tools;
    
    // Update categories
    m_categories.clear();
    for (const ToolInfo& tool : tools) {
        if (!m_categories.contains(tool.category)) {
            m_categories.append(tool.category);
        }
    }

    // Update favorites
    loadFavorites();

    emit toolsChanged();
    emit categoriesChanged();
}

void ToolManager::loadFavorites() {
    QSettings settings;
    settings.beginGroup("BlackArchLauncher");
    QStringList favoriteNames = settings.value("favorites", QStringList()).toStringList();
    settings.endGroup();

    m_favorites.clear();
    for (const QString& name : favoriteNames) {
        ToolInfo tool = m_discovery->getToolInfo(name);
        if (!tool.name.isEmpty()) {
            m_favorites.append(tool);
        }
    }

    emit favoritesChanged();
}

void ToolManager::saveFavorites() {
    QSettings settings;
    settings.beginGroup("BlackArchLauncher");
    
    QStringList favoriteNames;
    for (const ToolInfo& tool : m_favorites) {
        favoriteNames.append(tool.name);
    }
    
    settings.setValue("favorites", favoriteNames);
    settings.endGroup();
}

