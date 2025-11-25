#include "compatibilitydatabase.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <yaml-cpp/yaml.h>
#include <fstream>

CompatibilityDatabase::CompatibilityDatabase(QObject* parent)
    : QObject(parent)
{
    loadDatabase();
}

CompatibilityDatabase::~CompatibilityDatabase()
{
    saveDatabase();
}

QString CompatibilityDatabase::getDatabasePath() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos";
    QDir().mkpath(configDir);
    return configDir + "/theme-compatibility-db.yaml";
}

void CompatibilityDatabase::loadDatabase()
{
    QString dbPath = getDatabasePath();
    
    if (!QFile::exists(dbPath)) {
        // Create default database
        saveDatabase();
        return;
    }
    
    try {
        YAML::Node config = YAML::LoadFile(dbPath.toStdString());
        m_themes.clear();
        
        if (config["themes"]) {
            YAML::Node themesNode = config["themes"];
            for (YAML::const_iterator it = themesNode.begin(); it != themesNode.end(); ++it) {
                QVariantMap theme;
                QString themeName = QString::fromStdString(it->first.as<std::string>());
                YAML::Node themeData = it->second;
                
                theme["name"] = themeName;
                if (themeData["status"]) {
                    theme["status"] = QString::fromStdString(themeData["status"].as<std::string>());
                }
                if (themeData["conflicts"]) {
                    QVariantList conflicts;
                    YAML::Node conflictsNode = themeData["conflicts"];
                    for (YAML::const_iterator cit = conflictsNode.begin(); cit != conflictsNode.end(); ++cit) {
                        conflicts.append(QString::fromStdString((*cit).as<std::string>()));
                    }
                    theme["conflicts"] = conflicts;
                }
                if (themeData["user_reports"]) {
                    theme["user_reports"] = themeData["user_reports"].as<int>();
                }
                if (themeData["last_validated"]) {
                    theme["last_validated"] = QString::fromStdString(themeData["last_validated"].as<std::string>());
                }
                
                m_themes.append(theme);
            }
        }
        
        emit themesChanged();
    } catch (const YAML::Exception& e) {
        qWarning() << "Failed to load compatibility database:" << e.what();
    }
}

void CompatibilityDatabase::saveDatabase()
{
    QString dbPath = getDatabasePath();
    
    try {
        YAML::Node config;
        YAML::Node themesNode;
        
        for (const QVariant& var : m_themes) {
            QVariantMap theme = var.toMap();
            QString themeName = theme.value("name").toString();
            YAML::Node themeData;
            
            themeData["status"] = theme.value("status", "unknown").toString().toStdString();
            
            if (theme.contains("conflicts")) {
                YAML::Node conflictsNode;
                QVariantList conflicts = theme.value("conflicts").toList();
                for (const QVariant& conflict : conflicts) {
                    conflictsNode.push_back(conflict.toString().toStdString());
                }
                themeData["conflicts"] = conflictsNode;
            }
            
            if (theme.contains("user_reports")) {
                themeData["user_reports"] = theme.value("user_reports").toInt();
            }
            
            if (theme.contains("last_validated")) {
                themeData["last_validated"] = theme.value("last_validated").toString().toStdString();
            }
            
            themesNode[themeName.toStdString()] = themeData;
        }
        
        config["themes"] = themesNode;
        
        std::ofstream file(dbPath.toStdString());
        file << config;
        file.close();
    } catch (const YAML::Exception& e) {
        qWarning() << "Failed to save compatibility database:" << e.what();
    }
}

CompatibilityDatabase::CompatibilityStatus CompatibilityDatabase::getThemeStatus(const QString& themeName)
{
    QVariantMap themeInfo = getThemeInfo(themeName);
    if (themeInfo.isEmpty()) {
        return Unknown;
    }
    return stringToStatus(themeInfo.value("status", "unknown").toString());
}

QVariantMap CompatibilityDatabase::getThemeInfo(const QString& themeName)
{
    for (const QVariant& var : m_themes) {
        QVariantMap theme = var.toMap();
        if (theme.value("name").toString() == themeName) {
            return theme;
        }
    }
    return QVariantMap();
}

void CompatibilityDatabase::reportThemeCompatibility(const QString& themeName, CompatibilityStatus status, const QString& details)
{
    QVariantMap themeInfo = getThemeInfo(themeName);
    if (themeInfo.isEmpty()) {
        themeInfo["name"] = themeName;
        themeInfo["status"] = statusToString(status);
        themeInfo["user_reports"] = 1;
        m_themes.append(themeInfo);
    } else {
        int index = -1;
        for (int i = 0; i < m_themes.size(); ++i) {
            if (m_themes[i].toMap().value("name").toString() == themeName) {
                index = i;
                break;
            }
        }
        if (index >= 0) {
            QVariantMap updated = themeInfo;
            updated["user_reports"] = updated.value("user_reports", 0).toInt() + 1;
            if (details.isEmpty() == false) {
                QVariantList conflicts = updated.value("conflicts", QVariantList()).toList();
                conflicts.append(details);
                updated["conflicts"] = conflicts;
            }
            m_themes[index] = updated;
        }
    }
    
    saveDatabase();
    emit themesChanged();
}

void CompatibilityDatabase::updateThemeStatus(const QString& themeName, CompatibilityStatus status, const QVariantMap& conflicts)
{
    QVariantMap themeInfo = getThemeInfo(themeName);
    int index = -1;
    for (int i = 0; i < m_themes.size(); ++i) {
        if (m_themes[i].toMap().value("name").toString() == themeName) {
            index = i;
            break;
        }
    }
    
    if (index >= 0) {
        QVariantMap updated = m_themes[index].toMap();
        updated["status"] = statusToString(status);
        if (!conflicts.isEmpty()) {
            QVariantList conflictList;
            for (auto it = conflicts.begin(); it != conflicts.end(); ++it) {
                conflictList.append(it.value());
            }
            updated["conflicts"] = conflictList;
        }
        updated["last_validated"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        m_themes[index] = updated;
    } else {
        QVariantMap newTheme;
        newTheme["name"] = themeName;
        newTheme["status"] = statusToString(status);
        if (!conflicts.isEmpty()) {
            QVariantList conflictList;
            for (auto it = conflicts.begin(); it != conflicts.end(); ++it) {
                conflictList.append(it.value());
            }
            newTheme["conflicts"] = conflictList;
        }
        newTheme["last_validated"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        m_themes.append(newTheme);
    }
    
    saveDatabase();
    emit themesChanged();
    emit themeStatusUpdated(themeName, status);
}

QVariantList CompatibilityDatabase::getCompatibleThemes()
{
    QVariantList result;
    for (const QVariant& var : m_themes) {
        QVariantMap theme = var.toMap();
        if (theme.value("status").toString() == "compatible") {
            result.append(theme);
        }
    }
    return result;
}

QVariantList CompatibilityDatabase::getIncompatibleThemes()
{
    QVariantList result;
    for (const QVariant& var : m_themes) {
        QVariantMap theme = var.toMap();
        if (theme.value("status").toString() == "incompatible") {
            result.append(theme);
        }
    }
    return result;
}

QString CompatibilityDatabase::statusToString(CompatibilityStatus status)
{
    switch (status) {
        case Compatible:
            return "compatible";
        case Incompatible:
            return "incompatible";
        default:
            return "unknown";
    }
}

CompatibilityDatabase::CompatibilityStatus CompatibilityDatabase::stringToStatus(const QString& status)
{
    if (status == "compatible") {
        return Compatible;
    } else if (status == "incompatible") {
        return Incompatible;
    }
    return Unknown;
}

