#ifndef INSTALLERSTATE_H
#define INSTALLERSTATE_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class InstallerState : public QObject
{
    Q_OBJECT

public:
    explicit InstallerState(QObject *parent = nullptr);
    
    void saveState(const QVariantMap &state);
    QVariantMap loadState();
    void clearState();

private:
    QString m_stateFilePath;
};

#endif // INSTALLERSTATE_H

