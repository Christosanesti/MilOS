#ifndef BLACKARCH_TOOL_MANAGER_H
#define BLACKARCH_TOOL_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include "blackarch_repository.h"
#include "blackarch_offline_installer.h"

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
     * @brief Install selected tools
     * @param toolNames List of tool names to install
     * @return true if installation successful, false otherwise
     */
    bool installSelectedTools(const QStringList& toolNames);

    /**
     * @brief Get installed tools
     * @return List of installed tool names
     */
    QStringList getInstalledTools() const;

    /**
     * @brief Get tool status
     * @param toolName Tool name
     * @return Tool status (installed, not_installed, error)
     */
    QString getToolStatus(const QString& toolName) const;

    /**
     * @brief Update tools
     * @param toolNames List of tool names to update (empty = all)
     * @return true if update successful, false otherwise
     */
    bool updateTools(const QStringList& toolNames = QStringList());

    /**
     * @brief Create offline mirror for tools
     * @param toolNames List of tool names
     * @param mirrorPath Path to store mirror
     * @return true if mirror creation successful, false otherwise
     */
    bool createOfflineMirror(const QStringList& toolNames, const QString& mirrorPath);

Q_SIGNALS:
    /**
     * @brief Emitted when tool installation starts
     */
    void toolInstallationStarted(const QString& toolName);

    /**
     * @brief Emitted when tool installation completes
     */
    void toolInstallationCompleted(const QString& toolName, bool success);

    /**
     * @brief Emitted when tool update starts
     */
    void toolUpdateStarted(const QString& toolName);

    /**
     * @brief Emitted when tool update completes
     */
    void toolUpdateCompleted(const QString& toolName, bool success);

private slots:
    /**
     * @brief Handle tool installation started
     */
    void onToolInstallationStarted(const QString& toolName);

    /**
     * @brief Handle tool installation completed
     */
    void onToolInstallationCompleted(const QString& toolName, bool success);

private:
    bool m_initialized;
    BlackArchRepository* m_repository;
    BlackArchOfflineInstaller* m_offlineInstaller;
    class AuditLogger* m_auditLogger;
    QMap<QString, QString> m_toolStatus;

    /**
     * @brief Log tool installation to audit service
     */
    void logToolInstallation(const QString& toolName, bool success);

    /**
     * @brief Register tool with update service
     */
    void registerToolWithUpdateService(const QString& toolName);
};

#endif // BLACKARCH_TOOL_MANAGER_H

