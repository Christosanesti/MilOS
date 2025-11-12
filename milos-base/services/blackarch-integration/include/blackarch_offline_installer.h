#ifndef BLACKARCH_OFFLINE_INSTALLER_H
#define BLACKARCH_OFFLINE_INSTALLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>

/**
 * @brief BlackArch Offline Installer
 * 
 * Manages offline installation via local repository mirrors.
 */
class BlackArchOfflineInstaller : public QObject {
    Q_OBJECT

public:
    explicit BlackArchOfflineInstaller(QObject* parent = nullptr);
    ~BlackArchOfflineInstaller();

    /**
     * @brief Initialize offline installer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create local repository mirror
     * @param toolNames List of tool names to mirror
     * @param mirrorPath Path to store mirror
     * @return true if mirror creation successful, false otherwise
     */
    bool createMirror(const QStringList& toolNames, const QString& mirrorPath);

    /**
     * @brief Install from local mirror
     * @param mirrorPath Path to local mirror
     * @param toolNames List of tool names to install
     * @return true if installation successful, false otherwise
     */
    bool installFromMirror(const QString& mirrorPath, const QStringList& toolNames);

    /**
     * @brief Check if mirror exists
     * @param mirrorPath Path to mirror
     * @return true if mirror exists, false otherwise
     */
    bool mirrorExists(const QString& mirrorPath) const;

    /**
     * @brief Get mirror size
     * @param mirrorPath Path to mirror
     * @return Mirror size in bytes, -1 if error
     */
    qint64 getMirrorSize(const QString& mirrorPath) const;

Q_SIGNALS:
    /**
     * @brief Emitted when mirror creation starts
     */
    void mirrorCreationStarted(const QString& mirrorPath);

    /**
     * @brief Emitted when mirror creation completes
     */
    void mirrorCreationCompleted(const QString& mirrorPath, bool success);

    /**
     * @brief Emitted when mirror creation progress changes
     */
    void mirrorCreationProgress(const QString& mirrorPath, int progress);

private:
    bool m_initialized;
    QString m_defaultMirrorPath;

    /**
     * @brief Execute rsync command for mirroring
     */
    bool executeRsync(const QStringList& arguments);

    /**
     * @brief Execute pacman command with local repository
     */
    bool executePacmanLocal(const QStringList& arguments, const QString& mirrorPath);
};

#endif // BLACKARCH_OFFLINE_INSTALLER_H

