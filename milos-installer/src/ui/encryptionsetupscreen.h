#ifndef ENCRYPTIONSETUPSCREEN_H
#define ENCRYPTIONSETUPSCREEN_H

#include <QObject>
#include <QQuickItem>

class EncryptionSetupScreen : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString encryptionPassword READ encryptionPassword WRITE setEncryptionPassword NOTIFY encryptionPasswordChanged)
    Q_PROPERTY(QString selectedAlgorithm READ selectedAlgorithm WRITE setSelectedAlgorithm NOTIFY selectedAlgorithmChanged)
    Q_PROPERTY(bool encryptionInProgress READ encryptionInProgress NOTIFY encryptionInProgressChanged)
    Q_PROPERTY(double encryptionProgress READ encryptionProgress NOTIFY encryptionProgressChanged)

public:
    explicit EncryptionSetupScreen(QQuickItem *parent = nullptr);
    
    QString encryptionPassword() const { return m_encryptionPassword; }
    void setEncryptionPassword(const QString &password);
    
    QString selectedAlgorithm() const { return m_selectedAlgorithm; }
    void setSelectedAlgorithm(const QString &algorithm);
    
    bool encryptionInProgress() const { return m_encryptionInProgress; }
    double encryptionProgress() const { return m_encryptionProgress; }

signals:
    void encryptionPasswordChanged();
    void selectedAlgorithmChanged();
    void encryptionInProgressChanged();
    void encryptionProgressChanged();
    void backClicked();
    void nextClicked();

public slots:
    void startEncryption();

private:
    QString m_encryptionPassword;
    QString m_selectedAlgorithm;
    bool m_encryptionInProgress;
    double m_encryptionProgress;
};

#endif // ENCRYPTIONSETUPSCREEN_H

