#include "control.h"
#include "styling.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

PerformanceControl::PerformanceControl(QObject *parent)
    : QObject(parent)
    , m_currentMode("standard")
    , m_overrideEnabled(false)
    , m_overrideMode("standard")
    , m_styling(new PerformanceStyling(this))
{
    m_configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/performance-mode.json";
    loadConfiguration();
    
    // Apply initial mode
    m_styling->applyMode(m_currentMode);
}

void PerformanceControl::setCurrentMode(const QString &mode)
{
    if (m_currentMode != mode && availableModes().contains(mode)) {
        m_currentMode = mode;
        m_styling->applyMode(mode);
        saveConfiguration();
        emit currentModeChanged();
        emit modeApplied(mode);
    }
}

void PerformanceControl::setOverrideEnabled(bool enabled)
{
    if (m_overrideEnabled != enabled) {
        m_overrideEnabled = enabled;
        if (enabled) {
            m_styling->applyMode(m_overrideMode);
        } else {
            m_styling->applyMode(m_currentMode);
        }
        saveConfiguration();
        emit overrideEnabledChanged();
    }
}

void PerformanceControl::setOverrideMode(const QString &mode)
{
    if (m_overrideMode != mode && availableModes().contains(mode)) {
        m_overrideMode = mode;
        if (m_overrideEnabled) {
            m_styling->applyMode(mode);
        }
        saveConfiguration();
        emit overrideModeChanged();
    }
}

void PerformanceControl::setApplicationMode(const QString &applicationId, const QString &mode)
{
    if (availableModes().contains(mode)) {
        m_applicationOverrides[applicationId] = mode;
        saveConfiguration();
        emit applicationOverridesChanged();
    }
}

QString PerformanceControl::getApplicationMode(const QString &applicationId)
{
    if (m_applicationOverrides.contains(applicationId)) {
        return m_applicationOverrides[applicationId].toString();
    }
    return m_overrideEnabled ? m_overrideMode : m_currentMode;
}

void PerformanceControl::removeApplicationOverride(const QString &applicationId)
{
    if (m_applicationOverrides.contains(applicationId)) {
        m_applicationOverrides.remove(applicationId);
        saveConfiguration();
        emit applicationOverridesChanged();
    }
}

void PerformanceControl::clearAllOverrides()
{
    m_applicationOverrides.clear();
    saveConfiguration();
    emit applicationOverridesChanged();
}

void PerformanceControl::loadConfiguration()
{
    QFile file(m_configPath);
    if (!file.exists()) {
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open performance mode configuration file:" << m_configPath;
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse performance mode configuration:" << error.errorString();
        return;
    }
    
    QJsonObject obj = doc.object();
    
    if (obj.contains("currentMode")) {
        m_currentMode = obj["currentMode"].toString();
    }
    
    if (obj.contains("overrideEnabled")) {
        m_overrideEnabled = obj["overrideEnabled"].toBool();
    }
    
    if (obj.contains("overrideMode")) {
        m_overrideMode = obj["overrideMode"].toString();
    }
    
    if (obj.contains("applicationOverrides")) {
        QJsonObject overrides = obj["applicationOverrides"].toObject();
        for (auto it = overrides.begin(); it != overrides.end(); ++it) {
            m_applicationOverrides[it.key()] = it.value().toString();
        }
    }
    
    // Apply loaded mode
    if (m_overrideEnabled) {
        m_styling->applyMode(m_overrideMode);
    } else {
        m_styling->applyMode(m_currentMode);
    }
}

void PerformanceControl::saveConfiguration()
{
    QDir dir = QFileInfo(m_configPath).dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QFile file(m_configPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open performance mode configuration file for writing:" << m_configPath;
        return;
    }
    
    QJsonObject obj;
    obj["currentMode"] = m_currentMode;
    obj["overrideEnabled"] = m_overrideEnabled;
    obj["overrideMode"] = m_overrideMode;
    
    QJsonObject overrides;
    for (auto it = m_applicationOverrides.constBegin(); it != m_applicationOverrides.constEnd(); ++it) {
        overrides[it.key()] = it.value().toString();
    }
    obj["applicationOverrides"] = overrides;
    
    QJsonDocument doc(obj);
    file.write(doc.toJson());
    file.close();
}

