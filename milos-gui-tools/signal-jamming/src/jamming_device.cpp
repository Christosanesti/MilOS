#include "jamming_device.h"
#include <QUuid>
#include <QDebug>

JammingDevice::JammingDevice(QObject* parent)
    : QObject(parent)
    , m_connected(false)
    , m_jamming(false)
{
}

JammingDevice::~JammingDevice() {
    if (m_connected) {
        disconnectDevice();
    }
}

bool JammingDevice::initialize(const QString& deviceId, JammingDeviceType deviceType) {
    m_deviceId = deviceId;
    m_deviceType = deviceType;
    
    // Set device name based on type
    switch (deviceType) {
        case JammingDeviceType::RFJammer:
            m_deviceName = "RF Jammer";
            break;
        case JammingDeviceType::WiFiJammer:
            m_deviceName = "WiFi Jammer";
            break;
        case JammingDeviceType::CellularJammer:
            m_deviceName = "Cellular Jammer";
            break;
        case JammingDeviceType::GPSJammer:
            m_deviceName = "GPS Jammer";
            break;
        default:
            m_deviceName = "Generic Jammer";
            break;
    }
    
    return true;
}

bool JammingDevice::connectDevice() {
    if (m_connected) {
        return true;
    }
    
    // In production, would connect to actual hardware
    // For now, simulate connection
    m_connected = true;
    emit connectionChanged(true);
    
    return true;
}

void JammingDevice::disconnectDevice() {
    if (!m_connected) {
        return;
    }
    
    if (m_jamming) {
        stopJamming();
    }
    
    m_connected = false;
    emit connectionChanged(false);
}

bool JammingDevice::startJamming(const JammingPattern& pattern) {
    if (!m_connected) {
        return false;
    }
    
    if (m_jamming) {
        stopJamming();
    }
    
    m_currentPattern = pattern;
    
    // In production, would send command to hardware
    QVariantMap params;
    params["frequency_start"] = pattern.frequencyStart;
    params["frequency_end"] = pattern.frequencyEnd;
    params["power"] = pattern.power;
    params["pattern_type"] = pattern.patternType;
    
    if (!sendCommand("START_JAMMING", params)) {
        return false;
    }
    
    m_jamming = true;
    emit jammingStateChanged(true);
    emit patternChanged(pattern);
    
    return true;
}

bool JammingDevice::stopJamming() {
    if (!m_jamming) {
        return true;
    }
    
    if (!sendCommand("STOP_JAMMING")) {
        return false;
    }
    
    m_jamming = false;
    emit jammingStateChanged(false);
    
    return true;
}

QList<QPair<double, double>> JammingDevice::getSupportedFrequencies() const {
    QList<QPair<double, double>> frequencies;
    
    // Return supported frequencies based on device type
    switch (m_deviceType) {
        case JammingDeviceType::WiFiJammer:
            frequencies.append(qMakePair(2400.0, 2500.0));  // 2.4 GHz
            frequencies.append(qMakePair(5000.0, 6000.0));  // 5 GHz
            break;
        case JammingDeviceType::CellularJammer:
            frequencies.append(qMakePair(800.0, 900.0));    // 800 MHz
            frequencies.append(qMakePair(1800.0, 1900.0));   // 1.8 GHz
            frequencies.append(qMakePair(2100.0, 2200.0));   // 2.1 GHz
            break;
        case JammingDeviceType::GPSJammer:
            frequencies.append(qMakePair(1575.42, 1575.42)); // GPS L1
            break;
        default:
            frequencies.append(qMakePair(1.0, 6000.0));      // Wide range
            break;
    }
    
    return frequencies;
}

QVariantMap JammingDevice::getDeviceStatus() const {
    QVariantMap status;
    status["device_id"] = m_deviceId;
    status["device_name"] = m_deviceName;
    status["connected"] = m_connected;
    status["jamming"] = m_jamming;
    
    if (m_jamming) {
        QVariantMap pattern;
        pattern["id"] = m_currentPattern.id;
        pattern["name"] = m_currentPattern.name;
        pattern["frequency_start"] = m_currentPattern.frequencyStart;
        pattern["frequency_end"] = m_currentPattern.frequencyEnd;
        pattern["power"] = m_currentPattern.power;
        status["current_pattern"] = pattern;
    }
    
    return status;
}

bool JammingDevice::sendCommand(const QString& command, const QVariantMap& parameters) {
    // In production, would send command to hardware via serial/USB/network
    // For now, simulate command
    qDebug() << "Sending command to device" << m_deviceId << ":" << command << parameters;
    return true;
}

