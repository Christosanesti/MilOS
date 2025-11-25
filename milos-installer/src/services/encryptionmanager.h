#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <QObject>
#include <QString>
#include <QProcess>

class EncryptionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool encryptionInProgress READ encryptionInProgress NOTIFY encryptionInProgressChanged)
    Q_PROPERTY(double encryptionProgress READ encryptionProgress NOTIFY encryptionProgressChanged)

public:
    explicit EncryptionManager(QObject *parent = nullptr);
    
    bool encryptionInProgress() const { return m_encryptionInProgress; }
    double encryptionProgress() const { return m_encryptionProgress; }
    
    Q_INVOKABLE void setupEncryption(const QString &password, const QString &algorithm);
    Q_INVOKABLE void cancelEncryption();
    Q_INVOKABLE bool validateDiskSpace();
    Q_INVOKABLE void setDevicePath(const QString &devicePath) { m_devicePath = devicePath; }

signals:
    void encryptionInProgressChanged();
    void encryptionProgressChanged();
    void encryptionComplete();
    void encryptionError(const QString &error);

private:
    bool m_encryptionInProgress;
    double m_encryptionProgress;
    QProcess* m_process;
    QString m_devicePath;
};

#endif // ENCRYPTIONMANAGER_H

