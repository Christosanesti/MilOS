#include "air_gap_detector.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QTcpSocket>
#include <QDebug>
#include <QUrl>

AirGapDetector::AirGapDetector(QObject* parent)
    : QObject(parent)
    , m_status(AirGapStatus::Unknown)
    , m_checkTimer(new QTimer(this))
{
    connect(m_checkTimer, &QTimer::timeout, this, &AirGapDetector::performCheck);
}

AirGapDetector::~AirGapDetector() {
}

bool AirGapDetector::initialize(int checkInterval) {
    m_checkTimer->setInterval(checkInterval);
    performCheck();  // Initial check
    return true;
}

void AirGapDetector::checkNow() {
    performCheck();
}

void AirGapDetector::performCheck() {
    m_lastCheckTime = QDateTime::currentDateTime();
    
    bool connected = hasNetworkConnectivity();
    AirGapStatus newStatus = connected ? AirGapStatus::Connected : AirGapStatus::AirGapped;
    
    if (newStatus != m_status) {
        AirGapStatus oldStatus = m_status;
        m_status = newStatus;
        
        emit statusChanged(m_status);
        
        if (newStatus == AirGapStatus::AirGapped) {
            emit airGapped();
        } else if (newStatus == AirGapStatus::Connected && oldStatus == AirGapStatus::AirGapped) {
            emit connectivityRestored();
        }
    }
}

bool AirGapDetector::hasNetworkConnectivity() const {
    // Check if there are active network interfaces
    if (!hasActiveNetworkInterface()) {
        return false;
    }
    
    // Try to reach external host (optional, can be disabled for strict air-gap detection)
    // For now, we'll use interface status as primary indicator
    return true;
}

bool AirGapDetector::hasActiveNetworkInterface() const {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& iface : interfaces) {
        // Skip loopback
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }
        
        // Check if interface is up and has IP address
        if (iface.isUp() && iface.flags().testFlag(QNetworkInterface::IsRunning)) {
            QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for (const QNetworkAddressEntry& entry : entries) {
                QHostAddress addr = entry.ip();
                // Check for valid IPv4 or IPv6 address (not loopback)
                if (!addr.isLoopback() && (addr.protocol() == QAbstractSocket::IPv4Protocol || 
                                           addr.protocol() == QAbstractSocket::IPv6Protocol)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool AirGapDetector::canReachExternalHost() const {
    // Optional: Try to connect to external host
    // This can be used for more strict air-gap detection
    // For now, we'll rely on interface status
    return false;
}

QVariantMap AirGapDetector::getNetworkInterfacesStatus() const {
    QVariantMap status;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& iface : interfaces) {
        QString name = iface.name();
        bool isUp = iface.isUp() && iface.flags().testFlag(QNetworkInterface::IsRunning);
        
        QVariantMap ifaceInfo;
        ifaceInfo["up"] = isUp;
        ifaceInfo["hasAddress"] = !iface.addressEntries().isEmpty();
        
        QStringList addresses;
        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            addresses.append(entry.ip().toString());
        }
        ifaceInfo["addresses"] = addresses;
        
        status[name] = ifaceInfo;
    }
    
    return status;
}

