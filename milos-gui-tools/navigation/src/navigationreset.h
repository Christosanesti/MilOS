#ifndef NAVIGATIONRESET_H
#define NAVIGATIONRESET_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>

/**
 * @brief Navigation Reset Service
 * 
 * Manages navigation reset operations with backup and validation
 */
class NavigationReset : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.NavigationReset")

public:
    enum ResetType {
        Full,
        MilOS,
        Lua,
        XenonEffects,
        KeyboardShortcuts
    };
    Q_ENUM(ResetType)

    explicit NavigationReset(QObject* parent = nullptr);
    ~NavigationReset();

    bool initialize();
    bool start();
    void stop();

public Q_SLOTS:
    // D-Bus methods
    QVariantMap ResetNavigation(const QString& resetType);
    QString CreateBackup(const QString& resetType);
    QVariantMap ValidateReset(const QString& resetType);
    QVariantList ListBackups();
    bool RestoreBackup(const QString& backupPath);

Q_SIGNALS:
    void ResetStarted(const QString& resetId, const QString& resetType);
    void ResetCompleted(const QString& resetId, const QString& status);
    void BackupCreated(const QString& backupPath);

private:
    bool registerInterface();
    void unregisterInterface();
    QString createBackupForType(const QString& resetType);
    bool executeReset(const QString& resetType);
    void logToAudit(const QString& eventType, const QVariantMap& eventData);

    bool m_running;
    bool m_initialized;
};

#endif // NAVIGATIONRESET_H

