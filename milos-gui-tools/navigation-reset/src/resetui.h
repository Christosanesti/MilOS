#ifndef NAVIGATION_RESET_UI_H
#define NAVIGATION_RESET_UI_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

/**
 * @brief Navigation Reset UI Manager
 * 
 * Manages the UI for navigation reset operations
 */
class NavigationResetUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList resetTypes READ resetTypes CONSTANT)
    Q_PROPERTY(QVariantMap resetPreview READ resetPreview NOTIFY resetPreviewChanged)
    Q_PROPERTY(bool resetInProgress READ resetInProgress NOTIFY resetInProgressChanged)
    Q_PROPERTY(double resetProgress READ resetProgress NOTIFY resetProgressChanged)
    Q_PROPERTY(QVariantList backups READ backups NOTIFY backupsChanged)

public:
    explicit NavigationResetUI(QObject *parent = nullptr);
    
    QStringList resetTypes() const { 
        return QStringList() << "Full" << "MilOS" << "Lua" << "XenonEffects" << "KeyboardShortcuts"; 
    }
    
    QVariantMap resetPreview() const { return m_resetPreview; }
    bool resetInProgress() const { return m_resetInProgress; }
    double resetProgress() const { return m_resetProgress; }
    QVariantList backups() const { return m_backups; }
    
    Q_INVOKABLE void previewReset(const QString &resetType);
    Q_INVOKABLE void executeReset(const QString &resetType);
    Q_INVOKABLE void cancelReset();
    Q_INVOKABLE void loadBackups();
    Q_INVOKABLE QVariantMap validateBackup(const QString &backupPath);
    Q_INVOKABLE void restoreBackup(const QString &backupPath);
    Q_INVOKABLE void deleteBackup(const QString &backupPath);

signals:
    void resetPreviewChanged();
    void resetInProgressChanged();
    void resetProgressChanged();
    void resetCompleted(const QString &status, const QString &backupPath);
    void resetError(const QString &error);
    void backupsChanged();
    void backupRestored(const QString &backupPath);
    void backupRestoreError(const QString &error);

private slots:
    void onResetStarted(const QString &resetId, const QString &resetType);
    void onResetCompleted(const QString &resetId, const QString &status);
    void onBackupCreated(const QString &backupPath);

private:
    QDBusInterface* getResetInterface();
    void updateResetProgress();
    
    QVariantMap m_resetPreview;
    bool m_resetInProgress;
    double m_resetProgress;
    QVariantList m_backups;
    QString m_currentResetId;
    QDBusInterface* m_resetInterface;
};

#endif // NAVIGATION_RESET_UI_H

