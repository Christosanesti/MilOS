#include "dbus_interface.h"
#include "jamming_manager.h"
#include "jamming_device.h"
#include "spectrum_analyzer.h"
#include <QDebug>
#include <QDBusConnection>

SignalJammingDBusInterface::SignalJammingDBusInterface(QObject* parent)
    : QObject(parent)
    , m_manager(nullptr)
{
}

SignalJammingDBusInterface::~SignalJammingDBusInterface() {
}

bool SignalJammingDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.SignalJamming")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SignalJamming", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void SignalJammingDBusInterface::setManager(JammingManager* manager) {
    m_manager = manager;
}

QStringList SignalJammingDBusInterface::GetDevices() {
    if (!m_manager) {
        return QStringList();
    }
    
    return m_manager->getAllDevices();
}

QVariantMap SignalJammingDBusInterface::GetDeviceStatus(const QString& deviceId) {
    if (!m_manager) {
        return QVariantMap();
    }
    
    JammingDevice* device = m_manager->getDevice(deviceId);
    if (!device) {
        return QVariantMap();
    }
    
    return device->getDeviceStatus();
}

QString SignalJammingDBusInterface::CreatePattern(const QString& name, double frequencyStart, double frequencyEnd, double power, const QString& patternType) {
    if (!m_manager) {
        return QString();
    }
    
    return m_manager->createPattern(name, frequencyStart, frequencyEnd, power, patternType);
}

bool SignalJammingDBusInterface::StartJamming(const QString& deviceId, const QString& patternId) {
    if (!m_manager) {
        return false;
    }
    
    return m_manager->startJamming(deviceId, patternId);
}

bool SignalJammingDBusInterface::StopJamming(const QString& deviceId) {
    if (!m_manager) {
        return false;
    }
    
    return m_manager->stopJamming(deviceId);
}

bool SignalJammingDBusInterface::StartSpectrumAnalysis(double frequencyStart, double frequencyEnd) {
    if (!m_manager) {
        return false;
    }
    
    SpectrumAnalyzer* analyzer = m_manager->getSpectrumAnalyzer();
    if (!analyzer) {
        return false;
    }
    
    return analyzer->startAnalysis(frequencyStart, frequencyEnd);
}

void SignalJammingDBusInterface::StopSpectrumAnalysis() {
    if (!m_manager) {
        return;
    }
    
    SpectrumAnalyzer* analyzer = m_manager->getSpectrumAnalyzer();
    if (analyzer) {
        analyzer->stopAnalysis();
    }
}

QVariantMap SignalJammingDBusInterface::GetSpectrumData() {
    QVariantMap result;
    
    if (!m_manager) {
        return result;
    }
    
    SpectrumAnalyzer* analyzer = m_manager->getSpectrumAnalyzer();
    if (!analyzer) {
        return result;
    }
    
    QList<FrequencyDataPoint> data = analyzer->getSpectrumData();
    
    QVariantList points;
    for (const FrequencyDataPoint& point : data) {
        QVariantMap pointMap;
        pointMap["frequency"] = point.frequency;
        pointMap["power"] = point.power;
        pointMap["timestamp"] = point.timestamp.toString(Qt::ISODate);
        points.append(pointMap);
    }
    
    result["data_points"] = points;
    result["statistics"] = analyzer->getFrequencyStatistics();
    
    return result;
}

