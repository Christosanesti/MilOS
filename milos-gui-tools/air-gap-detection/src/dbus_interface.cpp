#include "dbus_interface.h"
#include "air_gap_detector.h"
#include "air_gap_enforcement.h"
#include <QDebug>
#include <QDBusConnection>

AirGapDBusInterface::AirGapDBusInterface(QObject* parent)
    : QObject(parent)
    , m_detector(nullptr)
    , m_enforcement(nullptr)
{
}

AirGapDBusInterface::~AirGapDBusInterface() {
}

bool AirGapDBusInterface::initialize() {
    // Register D-Bus service
    if (!QDBusConnection::sessionBus().registerService("org.milos.AirGap")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    // Register object
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/AirGap", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void AirGapDBusInterface::setDetector(AirGapDetector* detector) {
    m_detector = detector;
}

void AirGapDBusInterface::setEnforcement(AirGapEnforcement* enforcement) {
    m_enforcement = enforcement;
}

QString AirGapDBusInterface::GetStatus() {
    if (!m_detector) {
        return "Unknown";
    }
    
    switch (m_detector->status()) {
        case AirGapStatus::Connected:
            return "Connected";
        case AirGapStatus::AirGapped:
            return "AirGapped";
        default:
            return "Unknown";
    }
}

bool AirGapDBusInterface::IsAirGapped() {
    if (!m_detector) {
        return false;
    }
    
    return m_detector->isAirGapped();
}

void AirGapDBusInterface::CheckNow() {
    if (m_detector) {
        m_detector->checkNow();
    }
}

QVariantMap AirGapDBusInterface::GetNetworkInterfacesStatus() {
    if (!m_detector) {
        return QVariantMap();
    }
    
    return m_detector->getNetworkInterfacesStatus();
}

QString AirGapDBusInterface::GetPolicyLevel() {
    if (!m_enforcement) {
        return "Normal";
    }
    
    switch (m_enforcement->currentPolicyLevel()) {
        case SecurityPolicyLevel::Normal:
            return "Normal";
        case SecurityPolicyLevel::Enhanced:
            return "Enhanced";
        case SecurityPolicyLevel::Maximum:
            return "Maximum";
    }
    
    return "Normal";
}

bool AirGapDBusInterface::IsOperationAllowed(const QString& operation) {
    if (!m_enforcement) {
        return true;
    }
    
    return m_enforcement->isOperationAllowed(operation);
}

QStringList AirGapDBusInterface::GetBlockedOperations() {
    if (!m_enforcement) {
        return QStringList();
    }
    
    return m_enforcement->getBlockedOperations();
}

