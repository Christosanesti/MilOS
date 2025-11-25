#include "networkmanager.h"
#include <QVariantMap>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QNetworkInterface>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_scanning(false)
{
}

void NetworkManager::scanInterfaces()
{
    if (m_scanning) {
        return;
    }
    
    m_scanning = true;
    emit scanningChanged();
    
    // Implement actual network interface detection
    QVariantList interfaces;
    
    // Use QNetworkInterface to get system interfaces
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface &iface : allInterfaces) {
        // Skip loopback and inactive interfaces
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack) || 
            !iface.flags().testFlag(QNetworkInterface::IsUp)) {
            continue;
        }
        
        QVariantMap interfaceInfo;
        interfaceInfo["name"] = iface.name();
        interfaceInfo["type"] = iface.type() == QNetworkInterface::Ethernet ? "Ethernet" : 
                               (iface.type() == QNetworkInterface::Wifi ? "Wireless" : "Unknown");
        interfaceInfo["macAddress"] = iface.hardwareAddress();
        interfaceInfo["status"] = iface.flags().testFlag(QNetworkInterface::IsRunning) ? "connected" : "disconnected";
        
        // Get IP addresses
        QList<QNetworkAddressEntry> addresses = iface.addressEntries();
        if (!addresses.isEmpty()) {
            interfaceInfo["ipAddress"] = addresses.first().ip().toString();
            interfaceInfo["subnetMask"] = addresses.first().netmask().toString();
        }
        
        interfaces.append(interfaceInfo);
    }
    
    // Fallback: Check /sys/class/net for interfaces if QNetworkInterface doesn't work
    if (interfaces.isEmpty()) {
        QDir netDir("/sys/class/net");
        QStringList entries = netDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (const QString &entry : entries) {
            if (entry == "lo") continue; // Skip loopback
            
            QVariantMap interfaceInfo;
            interfaceInfo["name"] = entry;
            
            // Try to determine type from /sys/class/net/<interface>/type
            QFile typeFile(QString("/sys/class/net/%1/type").arg(entry));
            if (typeFile.open(QIODevice::ReadOnly)) {
                QTextStream stream(&typeFile);
                int type = stream.readAll().trimmed().toInt();
                interfaceInfo["type"] = (type == 1) ? "Ethernet" : "Unknown";
                typeFile.close();
            }
            
            // Get MAC address
            QFile addrFile(QString("/sys/class/net/%1/address").arg(entry));
            if (addrFile.open(QIODevice::ReadOnly)) {
                QTextStream stream(&addrFile);
                interfaceInfo["macAddress"] = stream.readAll().trimmed();
                addrFile.close();
            }
            
            // Check if interface is up
            QFile operstateFile(QString("/sys/class/net/%1/operstate").arg(entry));
            if (operstateFile.open(QIODevice::ReadOnly)) {
                QTextStream stream(&operstateFile);
                QString state = stream.readAll().trimmed();
                interfaceInfo["status"] = (state == "up") ? "connected" : "disconnected";
                operstateFile.close();
            } else {
                interfaceInfo["status"] = "unknown";
            }
            
            interfaces.append(interfaceInfo);
        }
    }
    
    m_networkInterfaces = interfaces;
    m_scanning = false;
    emit scanningChanged();
    emit networkInterfacesChanged();
}

void NetworkManager::configureDHCP(const QString &interface)
{
    // Implement DHCP configuration via systemd-networkd
    QProcess process;
    
    // Create systemd-networkd configuration file
    QString configDir = "/etc/systemd/network";
    QString configFile = QString("%1/10-%2.network").arg(configDir).arg(interface);
    
    // Ensure directory exists (in installer context, this would be in chroot)
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    // Write DHCP configuration
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "[Match]\n";
        stream << "Name=" << interface << "\n\n";
        stream << "[Network]\n";
        stream << "DHCP=yes\n";
        file.close();
        
        // Reload systemd-networkd (if running)
        process.start("systemctl", QStringList() << "reload" << "systemd-networkd");
        process.waitForFinished(2000);
        
        emit configurationComplete();
    } else {
        emit configurationError(QString("Failed to write configuration file: %1").arg(configFile));
    }
}

void NetworkManager::configureManual(const QString &interface, const QString &ip, const QString &subnet, const QString &gateway, const QString &dns)
{
    // Implement manual IP configuration via systemd-networkd
    QProcess process;
    
    // Create systemd-networkd configuration file
    QString configDir = "/etc/systemd/network";
    QString configFile = QString("%1/10-%2.network").arg(configDir).arg(interface);
    
    // Ensure directory exists
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    // Write static IP configuration
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "[Match]\n";
        stream << "Name=" << interface << "\n\n";
        stream << "[Network]\n";
        stream << "Address=" << ip << "/" << subnetToCidr(subnet) << "\n";
        if (!gateway.isEmpty()) {
            stream << "Gateway=" << gateway << "\n";
        }
        if (!dns.isEmpty()) {
            stream << "DNS=" << dns << "\n";
        }
        file.close();
        
        // Reload systemd-networkd (if running)
        process.start("systemctl", QStringList() << "reload" << "systemd-networkd");
        process.waitForFinished(2000);
        
        emit configurationComplete();
    } else {
        emit configurationError(QString("Failed to write configuration file: %1").arg(configFile));
    }
}

bool NetworkManager::testConnection()
{
    // Implement connection testing (ping gateway/DNS)
    QProcess process;
    
    // Try to ping a reliable host (8.8.8.8 - Google DNS)
    process.start("ping", QStringList() << "-c" << "3" << "-W" << "2" << "8.8.8.8");
    process.waitForFinished(5000);
    
    if (process.exitCode() == 0) {
        return true;
    }
    
    // Try pinging gateway if available
    // This would require getting the gateway from the interface configuration
    
    return false;
}

int NetworkManager::subnetToCidr(const QString &subnet)
{
    // Convert subnet mask to CIDR notation
    // Example: 255.255.255.0 -> 24
    QStringList parts = subnet.split(".");
    if (parts.size() != 4) {
        return 24; // Default
    }
    
    int cidr = 0;
    for (const QString &part : parts) {
        int octet = part.toInt();
        // Count set bits
        while (octet > 0) {
            cidr += (octet & 1);
            octet >>= 1;
        }
    }
    
    return cidr;
}

