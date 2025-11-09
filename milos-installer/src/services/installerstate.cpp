#include "installerstate.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIODevice>

InstallerState::InstallerState(QObject *parent)
    : QObject(parent)
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    m_stateFilePath = dataPath + "/installer_state.json";
}

void InstallerState::saveState(const QVariantMap &state)
{
    QJsonObject json = QJsonObject::fromVariantMap(state);
    QJsonDocument doc(json);
    
    QFile file(m_stateFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

QVariantMap InstallerState::loadState()
{
    QFile file(m_stateFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        return doc.object().toVariantMap();
    }
    return QVariantMap();
}

void InstallerState::clearState()
{
    QFile::remove(m_stateFilePath);
}

