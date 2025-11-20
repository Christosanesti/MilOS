#include "ethernet_enforcement.h"
#include <QNetworkInterface>
#include <QProcess>
#include <QDebug>

EthernetEnforcement::EthernetEnforcement(QObject* parent)
    : QObject(parent)
{
}

EthernetEnforcement::~EthernetEnforcement() {
}

bool EthernetEnforcement::initialize() {
    // Scan for authorized (ethernet) interfaces
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& iface : interfaces) {
        if (isEthernetInterface(iface.name())) {
            m_authorizedInterfaces.append(iface.name());
        }
    }
    
    return true;
}

bool EthernetEnforcement::isInterfaceAuthorized(const QString& interfaceName) const {
    return m_authorizedInterfaces.contains(interfaceName) || isEthernetInterface(interfaceName);
}

bool EthernetEnforcement::blockUnauthorizedInterfaces() {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& iface : interfaces) {
        if (!isInterfaceAuthorized(iface.name())) {
            if (isWiFiInterface(iface.name()) || isCellularInterface(iface.name())) {
                // Block interface
                QProcess process;
                process.start("ip", QStringList() << "link" << "set" << iface.name() << "down");
                if (process.waitForFinished(1000)) {
                    emit unauthorizedInterfaceDetected(iface.name());
                }
            }
        }
    }
    
    return true;
}

bool EthernetEnforcement::checkInternetConnectivity() const {
    // Check if internet is accessible
    // In production, would ping external servers
    QProcess process;
    process.start("ping", QStringList() << "-c" << "1" << "-W" << "1" << "8.8.8.8");
    if (process.waitForFinished(2000)) {
        return process.exitCode() == 0;
    }
    return false;
}

bool EthernetEnforcement::blockInternetConnectivity() {
    // Block internet connectivity by blocking default route
    // In production, would use firewall rules
    QProcess process;
    process.start("ip", QStringList() << "route" << "del" << "default");
    process.waitForFinished(1000);
    
    if (checkInternetConnectivity()) {
        emit internetConnectivityDetected();
        return false;
    }
    
    return true;
}

bool EthernetEnforcement::isEthernetInterface(const QString& interfaceName) const {
    return interfaceName.startsWith("eth") || interfaceName.startsWith("en");
}

bool EthernetEnforcement::isWiFiInterface(const QString& interfaceName) const {
    return interfaceName.startsWith("wlan") || interfaceName.startsWith("wifi") || interfaceName.startsWith("wl");
}

bool EthernetEnforcement::isCellularInterface(const QString& interfaceName) const {
    return interfaceName.startsWith("ppp") || interfaceName.startsWith("wwan") || interfaceName.startsWith("usb");
}

