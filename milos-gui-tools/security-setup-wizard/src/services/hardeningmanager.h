#ifndef HARDENINGMANAGER_H
#define HARDENINGMANAGER_H

#include <QObject>
#include <QString>

class HardeningManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLevel READ currentLevel WRITE setCurrentLevel NOTIFY currentLevelChanged)

public:
    explicit HardeningManager(QObject *parent = nullptr);
    
    QString currentLevel() const { return m_currentLevel; }
    void setCurrentLevel(const QString &level);
    
    Q_INVOKABLE void applyHardening(const QString &level);
    Q_INVOKABLE bool isHardeningComplete();

signals:
    void currentLevelChanged();
    void hardeningProgress(int percentage);
    void hardeningComplete();
    void hardeningError(const QString &error);

private:
    QString m_currentLevel;
};

#endif // HARDENINGMANAGER_H

