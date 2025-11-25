#include "layouttemplatemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QDateTime>
#include <QRegExp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>

LayoutTemplateManager::LayoutTemplateManager(QObject* parent)
    : QObject(parent)
    , m_currentTemplate("")
{
    m_categories = QStringList({"Command Center", "Analysis Station", "Field Operations", "Custom"});
    loadTemplates();
}

LayoutTemplateManager::~LayoutTemplateManager()
{
}

QString LayoutTemplateManager::getTemplatesDirectory() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/layout-templates";
    QDir().mkpath(configDir);
    return configDir;
}

void LayoutTemplateManager::loadTemplates()
{
    m_templates.clear();
    
    QString templatesDir = getTemplatesDirectory();
    QDir dir(templatesDir);
    
    QStringList filters;
    filters << "*.yaml" << "*.yml";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    
    for (const QFileInfo& fileInfo : files) {
        QVariantMap templateData = loadTemplateFromFile(fileInfo.absoluteFilePath());
        if (!templateData.isEmpty()) {
            m_templates.append(templateData);
        }
    }
    
    emit templatesChanged();
}

QVariantMap LayoutTemplateManager::loadTemplateFromFile(const QString& filePath)
{
    try {
        YAML::Node config = YAML::LoadFile(filePath.toStdString());
        QVariantMap templateData;
        
        if (config["template"]) {
            YAML::Node templateNode = config["template"];
            
            if (templateNode["id"]) {
                templateData["id"] = QString::fromStdString(templateNode["id"].as<std::string>());
            }
            if (templateNode["name"]) {
                templateData["name"] = QString::fromStdString(templateNode["name"].as<std::string>());
            }
            if (templateNode["description"]) {
                templateData["description"] = QString::fromStdString(templateNode["description"].as<std::string>());
            }
            if (templateNode["category"]) {
                templateData["category"] = QString::fromStdString(templateNode["category"].as<std::string>());
            }
            if (templateNode["monitors"]) {
                templateData["monitors"] = templateNode["monitors"].as<int>();
            }
            if (templateNode["applications"]) {
                QVariantList apps;
                YAML::Node appsNode = templateNode["applications"];
                for (YAML::const_iterator it = appsNode.begin(); it != appsNode.end(); ++it) {
                    QVariantMap app;
                    if ((*it)["name"]) {
                        app["name"] = QString::fromStdString((*it)["name"].as<std::string>());
                    }
                    if ((*it)["monitor"]) {
                        app["monitor"] = (*it)["monitor"].as<int>();
                    }
                    if ((*it)["position"]) {
                        QVariantMap pos;
                        YAML::Node posNode = (*it)["position"];
                        if (posNode["x"]) pos["x"] = posNode["x"].as<int>();
                        if (posNode["y"]) pos["y"] = posNode["y"].as<int>();
                        if (posNode["width"]) pos["width"] = posNode["width"].as<int>();
                        if (posNode["height"]) pos["height"] = posNode["height"].as<int>();
                        app["position"] = pos;
                    }
                    apps.append(app);
                }
                templateData["applications"] = apps;
            }
            if (templateNode["version"]) {
                templateData["version"] = QString::fromStdString(templateNode["version"].as<std::string>());
            }
        }
        
        return templateData;
    } catch (const YAML::Exception& e) {
        qWarning() << "Failed to load template:" << e.what();
        return QVariantMap();
    }
}

bool LayoutTemplateManager::saveTemplateToFile(const QVariantMap& template, const QString& filePath)
{
    try {
        YAML::Node config;
        YAML::Node templateNode;
        
        templateNode["id"] = template.value("id").toString().toStdString();
        templateNode["name"] = template.value("name").toString().toStdString();
        templateNode["description"] = template.value("description").toString().toStdString();
        templateNode["category"] = template.value("category").toString().toStdString();
        templateNode["monitors"] = template.value("monitors").toInt();
        templateNode["version"] = template.value("version", "1.0").toString().toStdString();
        
        if (template.contains("applications")) {
            YAML::Node appsNode;
            QVariantList apps = template.value("applications").toList();
            for (const QVariant& appVar : apps) {
                QVariantMap app = appVar.toMap();
                YAML::Node appNode;
                appNode["name"] = app.value("name").toString().toStdString();
                appNode["monitor"] = app.value("monitor").toInt();
                
                if (app.contains("position")) {
                    QVariantMap pos = app.value("position").toMap();
                    YAML::Node posNode;
                    posNode["x"] = pos.value("x").toInt();
                    posNode["y"] = pos.value("y").toInt();
                    posNode["width"] = pos.value("width").toInt();
                    posNode["height"] = pos.value("height").toInt();
                    appNode["position"] = posNode;
                }
                
                appsNode.push_back(appNode);
            }
            templateNode["applications"] = appsNode;
        }
        
        config["template"] = templateNode;
        
        std::ofstream file(filePath.toStdString());
        file << config;
        file.close();
        
        return true;
    } catch (const YAML::Exception& e) {
        qWarning() << "Failed to save template:" << e.what();
        return false;
    }
}

void LayoutTemplateManager::saveTemplate(const QVariantMap& template)
{
    if (!validateTemplate(template)) {
        emit error("Template validation failed");
        return;
    }
    
    QString templateId = template.value("id").toString();
    if (templateId.isEmpty()) {
        templateId = generateTemplateId(template.value("name").toString());
    }
    
    QString fileName = QString("%1.yaml").arg(templateId);
    QString filePath = getTemplatesDirectory() + "/" + fileName;
    
    QVariantMap templateCopy = template;
    templateCopy["id"] = templateId;
    
    if (saveTemplateToFile(templateCopy, filePath)) {
        loadTemplates(); // Reload templates
    } else {
        emit error("Failed to save template");
    }
}

void LayoutTemplateManager::deleteTemplate(const QString& templateId)
{
    QString fileName = QString("%1.yaml").arg(templateId);
    QString filePath = getTemplatesDirectory() + "/" + fileName;
    
    if (QFile::exists(filePath)) {
        QFile::remove(filePath);
        loadTemplates(); // Reload templates
    }
}

void LayoutTemplateManager::duplicateTemplate(const QString& templateId)
{
    QVariantMap template = getTemplate(templateId);
    if (template.isEmpty()) {
        emit error("Template not found");
        return;
    }
    
    QString newName = template.value("name").toString() + " (Copy)";
    template["id"] = "";
    template["name"] = newName;
    saveTemplate(template);
}

QVariantMap LayoutTemplateManager::getTemplate(const QString& templateId)
{
    for (const QVariant& var : m_templates) {
        QVariantMap template = var.toMap();
        if (template.value("id").toString() == templateId) {
            return template;
        }
    }
    return QVariantMap();
}

QVariantMap LayoutTemplateManager::previewTemplate(const QString& templateId)
{
    return getTemplate(templateId);
}

bool LayoutTemplateManager::validateTemplate(const QVariantMap& template)
{
    if (!template.contains("name") || template.value("name").toString().isEmpty()) {
        return false;
    }
    if (!template.contains("category") || template.value("category").toString().isEmpty()) {
        return false;
    }
    if (!template.contains("monitors") || template.value("monitors").toInt() < 1) {
        return false;
    }
    return true;
}

void LayoutTemplateManager::applyTemplate(const QString& templateId)
{
    QVariantMap template = getTemplate(templateId);
    if (template.isEmpty()) {
        emit error("Template not found");
        return;
    }
    
    m_currentTemplate = templateId;
    emit currentTemplateChanged();
    emit templateApplied(templateId);
}

void LayoutTemplateManager::exportTemplate(const QString& templateId, const QString& filePath)
{
    QVariantMap template = getTemplate(templateId);
    if (template.isEmpty()) {
        emit error("Template not found");
        return;
    }
    
    if (saveTemplateToFile(template, filePath)) {
        emit templateExported(templateId, filePath);
    } else {
        emit error("Failed to export template");
    }
}

bool LayoutTemplateManager::importTemplate(const QString& filePath)
{
    QVariantMap template = loadTemplateFromFile(filePath);
    if (template.isEmpty()) {
        emit error("Failed to load template from file");
        return false;
    }
    
    // Check for name conflicts
    QString templateName = template.value("name").toString();
    for (const QVariant& var : m_templates) {
        QVariantMap existing = var.toMap();
        if (existing.value("name").toString() == templateName) {
            // Generate unique name
            templateName += " (Imported)";
            template["name"] = templateName;
            break;
        }
    }
    
    template["id"] = ""; // Generate new ID
    saveTemplate(template);
    emit templateImported(template.value("id").toString());
    return true;
}

QVariantList LayoutTemplateManager::getTemplatesByCategory(const QString& category)
{
    QVariantList result;
    for (const QVariant& var : m_templates) {
        QVariantMap template = var.toMap();
        if (template.value("category").toString() == category) {
            result.append(template);
        }
    }
    return result;
}

QString LayoutTemplateManager::generateTemplateId(const QString& name)
{
    QString id = name.toLower().replace(" ", "_").replace(QRegExp("[^a-z0-9_]"), "");
    return QString("%1_%2").arg(id).arg(QDateTime::currentMSecsSinceEpoch());
}

