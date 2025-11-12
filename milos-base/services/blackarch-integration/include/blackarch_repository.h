#ifndef BLACKARCH_REPOSITORY_H
#define BLACKARCH_REPOSITORY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <memory>

/**
 * @brief BlackArch Repository Manager
 * 
 * Manages BlackArch repository integration and tool installation.
 */
class BlackArchRepository : public QObject {
    Q_OBJECT

public:
    explicit BlackArchRepository(QObject* parent = nullptr);
    ~BlackArchRepository();

    /**
     * @brief Initialize repository manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if BlackArch repository is configured
     * @return true if configured, false otherwise
     */
    bool isRepositoryConfigured() const;

    /**
     * @brief Configure BlackArch repository
     * @return true if configuration successful, false otherwise
     */
    bool configureRepository();

    /**
     * @brief Get list of available tools
     * @return List of available tool names
     */
    QStringList getAvailableTools() const;

    /**
     * @brief Check if tool is installed
     * @param toolName Tool name
     * @return true if installed, false otherwise
     */
    bool isToolInstalled(const QString& toolName) const;

    /**
     * @brief Install tool
     * @param toolName Tool name
     * @return true if installation successful, false otherwise
     */
    bool installTool(const QString& toolName);

    /**
     * @brief Install multiple tools
     * @param toolNames List of tool names
     * @return true if all installations successful, false otherwise
     */
    bool installTools(const QStringList& toolNames);

    /**
     * @brief Uninstall tool
     * @param toolName Tool name
     * @return true if uninstallation successful, false otherwise
     */
    bool uninstallTool(const QString& toolName);

    /**
     * @brief Update tool
     * @param toolName Tool name
     * @return true if update successful, false otherwise
     */
    bool updateTool(const QString& toolName);

    /**
     * @brief Update all tools
     * @return true if update successful, false otherwise
     */
    bool updateAllTools();

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
     * @brief Emitted when tool installation progress changes
     */
    void toolInstallationProgress(const QString& toolName, int progress);

private:
    bool m_initialized;
    bool m_repositoryConfigured;
    QString m_repositoryUrl;
    QString m_mirrorPath;

    /**
     * @brief Execute pacman command
     */
    bool executePacman(const QStringList& arguments, QString* output = nullptr);

    /**
     * @brief Check pacman availability
     */
    bool checkPacmanAvailable() const;
};

#endif // BLACKARCH_REPOSITORY_H

