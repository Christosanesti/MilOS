#include "network_manager.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>
#include <QProcess>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
    , m_vpnConnected(false)
{
}

NetworkManager::~NetworkManager() {
}

bool NetworkManager::initialize() {
    scanNetworkInterfaces();
    return true;
}

QList<NetworkInterfaceInfo> NetworkManager::getNetworkInterfaces() const {
    return m_interfaces;
}

QList<NetworkInterfaceInfo> NetworkManager::getAuthorizedInterfaces() const {
    QList<NetworkInterfaceInfo> authorized;
    for (const NetworkInterfaceInfo& iface : m_interfaces) {
        if (iface.isAuthorized && iface.isActive) {
            authorized.append(iface);
        }
    }
    return authorized;
}

QList<QString> NetworkManager::detectSegmentation() const {
    QList<QString> subnets;
    
    for (const NetworkInterfaceInfo& iface : m_interfaces) {
        if (iface.isActive && !iface.subnet.isEmpty()) {
            if (!subnets.contains(iface.subnet)) {
                subnets.append(iface.subnet);
            }
        }
    }
    
    return subnets;
}

bool NetworkManager::connectVPN(const QVariantMap& vpnConfig) {
    // In production, would use VPN library or system commands
    // For now, placeholder
    m_vpnConnected = true;
    emit vpnConnected();
    return true;
}

bool NetworkManager::disconnectVPN() {
    m_vpnConnected = false;
    emit vpnDisconnected();
    return true;
}

bool NetworkManager::createBridge(const QVariantMap& bridgeConfig) {
    // In production, would use bridge utilities
    // For now, placeholder
    return true;
}

bool NetworkManager::removeBridge(const QString& bridgeName) {
    // In production, would use bridge utilities
    // For now, placeholder
    return true;
}

void NetworkManager::scanNetworkInterfaces() {
    m_interfaces.clear();
    
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }
        
        NetworkInterfaceInfo info;
        info.name = iface.name();
        info.isActive = iface.isUp();
        
        // Determine interface type
        if (iface.name().startsWith("eth") || iface.name().startsWith("en")) {
            info.type = NetworkInterfaceType::Ethernet;
            info.isAuthorized = true;
        } else if (iface.name().startsWith("wlan") || iface.name().startsWith("wifi")) {
            info.type = NetworkInterfaceType::WiFi;
            info.isAuthorized = false;
        } else if (iface.name().startsWith("ppp") || iface.name().startsWith("wwan")) {
            info.type = NetworkInterfaceType::Cellular;
            info.isAuthorized = false;
        } else if (iface.name().startsWith("tun") || iface.name().startsWith("tap")) {
            info.type = NetworkInterfaceType::VPN;
            info.isAuthorized = true;
        } else if (iface.name().startsWith("br")) {
            info.type = NetworkInterfaceType::Bridge;
            info.isAuthorized = true;
        } else {
            info.type = NetworkInterfaceType::Ethernet;  // Default
            info.isAuthorized = false;
        }
        
        // Get address and subnet
        QList<QNetworkAddressEntry> entries = iface.addressEntries();
        if (!entries.isEmpty()) {
            info.address = entries.first().ip();
            info.subnet = entries.first().ip().toString() + "/" + QString::number(entries.first().prefixLength());
        }
        
        m_interfaces.append(info);
        emit interfaceDetected(info.name);
    }
}

bool NetworkManager::isEthernetInterface(const QString& interfaceName) const {
    return interfaceName.startsWith("eth") || interfaceName.startsWith("en");
}

