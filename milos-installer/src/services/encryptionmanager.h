#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <QObject>
#include <QString>

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

signals:
    void encryptionInProgressChanged();
    void encryptionProgressChanged();
    void encryptionComplete();
    void encryptionError(const QString &error);

private:
    bool m_encryptionInProgress;
    double m_encryptionProgress;
    
    // TODO: Integrate with libcryptsetup or systemd-cryptsetup for LUKS2
};

#endif // ENCRYPTIONMANAGER_H

