#include "setupstatemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIODevice>

SetupStateManager::SetupStateManager(QObject *parent)
    : QObject(parent)
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dataPath);
    m_stateFilePath = dataPath + "/setup_state.json";
}

void SetupStateManager::saveState(const QVariantMap &state)
{
    QJsonObject json = QJsonObject::fromVariantMap(state);
    QJsonDocument doc(json);
    
    QFile file(m_stateFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        emit stateSaved();
    }
}

QVariantMap SetupStateManager::loadState()
{
    QFile file(m_stateFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QVariantMap state = doc.object().toVariantMap();
        emit stateLoaded(state);
        return state;
    }
    return QVariantMap();
}

void SetupStateManager::clearState()
{
    QFile::remove(m_stateFilePath);
}

