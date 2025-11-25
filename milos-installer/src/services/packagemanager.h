#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QProcess>
#include <QStringList>

class PackageManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool installing READ installing NOTIFY installingChanged)
    Q_PROPERTY(double installationProgress READ installationProgress NOTIFY installationProgressChanged)
    Q_PROPERTY(int packagesInstalled READ packagesInstalled NOTIFY packagesInstalledChanged)
    Q_PROPERTY(int totalPackages READ totalPackages NOTIFY totalPackagesChanged)
    Q_PROPERTY(QString currentPackage READ currentPackage NOTIFY currentPackageChanged)

public:
    explicit PackageManager(QObject *parent = nullptr);
    
    bool installing() const { return m_installing; }
    double installationProgress() const { return m_installationProgress; }
    int packagesInstalled() const { return m_packagesInstalled; }
    int totalPackages() const { return m_totalPackages; }
    QString currentPackage() const { return m_currentPackage; }
    
    Q_INVOKABLE void startInstallation();
    Q_INVOKABLE void pauseInstallation();
    Q_INVOKABLE void resumeInstallation();
    Q_INVOKABLE void cancelInstallation();
    Q_INVOKABLE QString estimatedTimeRemaining();

signals:
    void installingChanged();
    void installationProgressChanged();
    void packagesInstalledChanged();
    void totalPackagesChanged();
    void currentPackageChanged();
    void installationComplete();
    void installationError(const QString &error);

private:
    bool m_installing;
    double m_installationProgress;
    int m_packagesInstalled;
    int m_totalPackages;
    QString m_currentPackage;
    QProcess* m_process;
    bool m_paused;
    QStringList m_packageList;
    QString m_packageManager; // "pacman" or "apt"
    
    void detectPackageManager();
    void parsePackageList();
    void updateProgress();
};

#endif // PACKAGEMANAGER_H

