#ifndef BLACKARCH_TOOL_MANAGER_H
#define BLACKARCH_TOOL_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include "blackarch_repository.h"

// Forward declarations
class QDBusInterface;

/**
 * @brief BlackArch Tool Manager
 * 
 * Manages BlackArch tool integration with system services.
 */
class BlackArchToolManager : public QObject {
    Q_OBJECT

public:
    explicit BlackArchToolManager(QObject* parent = nullptr);
    ~BlackArchToolManager();

    /**
     * @brief Initialize tool manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Verify selected tools exist in scraped data
     * @param toolNames List of tool names to verify
     * @return true if all tools found in scraped data, false otherwise
     */
    bool installSelectedTools(const QStringList& toolNames);

    /**
     * @brief Get available tools from scraped data
     * @return List of available tool names
     */
    QStringList getInstalledTools() const;

    /**
     * @brief Get tool status
     * @param toolName Tool name
     * @return Tool status (available, not_found, error)
     */
    QString getToolStatus(const QString& toolName) const;

    /**
     * @brief Refresh scraped tool data
     * @param toolNames Not used (kept for compatibility)
     * @return true if refresh successful, false otherwise
     */
    bool updateTools(const QStringList& toolNames = QStringList());

    /**
     * @brief Export tool data (replaces offline mirror)
     * @param toolNames List of tool names
     * @param mirrorPath Path to export data
     * @return true if export successful, false otherwise
     */
    bool createOfflineMirror(const QStringList& toolNames, const QString& mirrorPath);

Q_SIGNALS:
    /**
     * @brief Emitted when data scraping starts
     */
    void dataScrapingStarted();

    /**
     * @brief Emitted when data scraping completes
     */
    void dataScrapingCompleted(bool success);

    /**
     * @brief Emitted when data scraping progress changes
     */
    void dataScrapingProgress(int progress, const QString& currentTool);

private slots:
    /**
     * @brief Handle data scraping started (placeholder for compatibility)
     */
    void onToolInstallationStarted(const QString& toolName);

    /**
     * @brief Handle data scraping completed (placeholder for compatibility)
     */
    void onToolInstallationCompleted(const QString& toolName, bool success);

private:
    bool m_initialized;
    BlackArchRepository* m_repository;
    class AuditLogger* m_auditLogger;
    QMap<QString, QString> m_toolStatus;
};

#endif // BLACKARCH_TOOL_MANAGER_H

