#include "preferencesmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIODevice>

PreferencesManager::PreferencesManager(QObject *parent)
    : QObject(parent)
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dataPath);
    m_preferencesFilePath = dataPath + "/security_preferences.json";
}

void PreferencesManager::savePreferences(const QVariantMap &preferences)
{
    QJsonObject json = QJsonObject::fromVariantMap(preferences);
    QJsonDocument doc(json);
    
    QFile file(m_preferencesFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        emit preferencesSaved();
    }
}

QVariantMap PreferencesManager::loadPreferences()
{
    QFile file(m_preferencesFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QVariantMap preferences = doc.object().toVariantMap();
        emit preferencesLoaded(preferences);
        return preferences;
    }
    return QVariantMap();
}

void PreferencesManager::resetPreferences()
{
    QFile::remove(m_preferencesFilePath);
}

