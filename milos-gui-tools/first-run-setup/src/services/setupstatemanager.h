#ifndef SETUPSTATEMANAGER_H
#define SETUPSTATEMANAGER_H

#include <QObject>
#include <QVariantMap>

class SetupStateManager : public QObject
{
    Q_OBJECT

public:
    explicit SetupStateManager(QObject *parent = nullptr);
    
    Q_INVOKABLE void saveState(const QVariantMap &state);
    Q_INVOKABLE QVariantMap loadState();
    Q_INVOKABLE void clearState();

signals:
    void stateSaved();
    void stateLoaded(const QVariantMap &state);

private:
    QString m_stateFilePath;
};

#endif // SETUPSTATEMANAGER_H

