#include "jamming_manager.h"
#include <QUuid>
#include <QDebug>

JammingManager::JammingManager(QObject* parent)
    : QObject(parent)
    , m_spectrumAnalyzer(new SpectrumAnalyzer(this))
{
}

JammingManager::~JammingManager() {
}

bool JammingManager::initialize() {
    if (!m_spectrumAnalyzer->initialize()) {
        qWarning() << "Failed to initialize spectrum analyzer";
        return false;
    }
    
    return true;
}

bool JammingManager::addDevice(JammingDevice* device) {
    if (!device) {
        return false;
    }
    
    QString deviceId = device->deviceId();
    if (m_devices.contains(deviceId)) {
        return false;  // Already exists
    }
    
    m_devices[deviceId] = device;
    emit deviceAdded(deviceId);
    
    return true;
}

bool JammingManager::removeDevice(const QString& deviceId) {
    if (!m_devices.contains(deviceId)) {
        return false;
    }
    
    JammingDevice* device = m_devices[deviceId];
    if (device->isJamming()) {
        device->stopJamming();
    }
    device->disconnectDevice();
    
    m_devices.remove(deviceId);
    emit deviceRemoved(deviceId);
    
    return true;
}

QStringList JammingManager::getAllDevices() const {
    return m_devices.keys();
}

JammingDevice* JammingManager::getDevice(const QString& deviceId) const {
    return m_devices.value(deviceId, nullptr);
}

QString JammingManager::createPattern(const QString& name, double frequencyStart, double frequencyEnd,
                                     double power, const QString& patternType) {
    JammingPattern pattern;
    pattern.id = generatePatternId();
    pattern.name = name;
    pattern.frequencyStart = frequencyStart;
    pattern.frequencyEnd = frequencyEnd;
    pattern.power = power;
    pattern.patternType = patternType;
    
    m_patterns[pattern.id] = pattern;
    
    return pattern.id;
}

JammingPattern JammingManager::getPattern(const QString& patternId) const {
    return m_patterns.value(patternId, JammingPattern());
}

QList<JammingPattern> JammingManager::getAllPatterns() const {
    return m_patterns.values();
}

bool JammingManager::startJamming(const QString& deviceId, const QString& patternId) {
    if (!m_devices.contains(deviceId)) {
        return false;
    }
    
    if (!m_patterns.contains(patternId)) {
        return false;
    }
    
    JammingDevice* device = m_devices[deviceId];
    JammingPattern pattern = m_patterns[patternId];
    
    if (!device->isConnected()) {
        if (!device->connectDevice()) {
            return false;
        }
    }
    
    if (device->startJamming(pattern)) {
        emit jammingStarted(deviceId, patternId);
        return true;
    }
    
    return false;
}

bool JammingManager::stopJamming(const QString& deviceId) {
    if (!m_devices.contains(deviceId)) {
        return false;
    }
    
    JammingDevice* device = m_devices[deviceId];
    
    if (device->stopJamming()) {
        emit jammingStopped(deviceId);
        return true;
    }
    
    return false;
}

QString JammingManager::generatePatternId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

