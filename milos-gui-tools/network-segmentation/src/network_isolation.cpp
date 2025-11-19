#include "network_isolation.h"
#include "segment_config.h"
#include <QProcess>
#include <QDebug>
#include <iostream>

NetworkIsolation::NetworkIsolation(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

NetworkIsolation::~NetworkIsolation() {
}

bool NetworkIsolation::initialize() {
    if (m_initialized) {
        return true;
    }

    // Check if we have required kernel features
    QProcess process;
    process.start("uname", QStringList() << "-r");
    process.waitForFinished();
    if (process.exitCode() != 0) {
        std::cerr << "Failed to check kernel version" << std::endl;
        return false;
    }

    // Check for network namespace support
    process.start("test", QStringList() << "-e" << "/proc/self/ns/net");
    process.waitForFinished();
    if (process.exitCode() != 0) {
        std::cerr << "Network namespaces not supported" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool NetworkIsolation::isolateSegment(const SegmentConfig& segment) {
    if (!m_initialized) {
        return false;
    }

    // Create network namespace for segment
    if (!createNetworkNamespace(segment.segmentId, segment.networkAddress)) {
        std::cerr << "Failed to create network namespace for segment: " << segment.segmentId.toStdString() << std::endl;
        return false;
    }

    // Configure namespace isolation
    if (!configureNamespaceIsolation(segment.segmentId, segment)) {
        std::cerr << "Failed to configure namespace isolation for segment: " << segment.segmentId.toStdString() << std::endl;
        removeNetworkNamespace(segment.segmentId);
        return false;
    }

    m_isolatedSegments.append(segment.segmentId);
    emit segmentIsolated(segment.segmentId);
    return true;
}

bool NetworkIsolation::removeIsolation(const QString& segmentId) {
    if (!m_initialized || !m_isolatedSegments.contains(segmentId)) {
        return false;
    }

    if (!removeNetworkNamespace(segmentId)) {
        std::cerr << "Failed to remove network namespace for segment: " << segmentId.toStdString() << std::endl;
        return false;
    }

    m_isolatedSegments.removeAll(segmentId);
    emit isolationRemoved(segmentId);
    return true;
}

bool NetworkIsolation::verifyIsolation(const QString& segmentId) const {
    return m_isolatedSegments.contains(segmentId);
}

QStringList NetworkIsolation::getIsolatedSegments() const {
    return m_isolatedSegments;
}

bool NetworkIsolation::createNetworkNamespace(const QString& segmentId, const QString& networkAddress) {
    // Create network namespace using ip netns
    // Note: This requires root privileges
    QProcess process;
    QString namespaceName = QString("milos-seg-%1").arg(segmentId);
    
    process.start("ip", QStringList() << "netns" << "add" << namespaceName);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        std::cerr << "Failed to create network namespace: " << process.readAllStandardError().toStdString() << std::endl;
        return false;
    }

    // Create veth pair for namespace
    QString vethHost = QString("veth-%1-host").arg(segmentId);
    QString vethNs = QString("veth-%1-ns").arg(segmentId);
    
    process.start("ip", QStringList() << "link" << "add" << vethHost << "type" << "veth" << "peer" << "name" << vethNs);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        std::cerr << "Failed to create veth pair: " << process.readAllStandardError().toStdString() << std::endl;
        removeNetworkNamespace(segmentId);
        return false;
    }

    // Move veth to namespace
    process.start("ip", QStringList() << "link" << "set" << vethNs << "netns" << namespaceName);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        std::cerr << "Failed to move veth to namespace: " << process.readAllStandardError().toStdString() << std::endl;
        removeNetworkNamespace(segmentId);
        return false;
    }

    // Configure network in namespace
    process.start("ip", QStringList() << "netns" << "exec" << namespaceName << "ip" << "addr" << "add" << networkAddress << "dev" << vethNs);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        std::cerr << "Failed to configure network in namespace: " << process.readAllStandardError().toStdString() << std::endl;
        removeNetworkNamespace(segmentId);
        return false;
    }

    process.start("ip", QStringList() << "netns" << "exec" << namespaceName << "ip" << "link" << "set" << vethNs << "up");
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        std::cerr << "Failed to bring up interface in namespace: " << process.readAllStandardError().toStdString() << std::endl;
        removeNetworkNamespace(segmentId);
        return false;
    }

    // Bring up host interface
    process.start("ip", QStringList() << "link" << "set" << vethHost << "up");
    process.waitForFinished();
    
    return true;
}

bool NetworkIsolation::removeNetworkNamespace(const QString& segmentId) {
    QString namespaceName = QString("milos-seg-%1").arg(segmentId);
    
    QProcess process;
    process.start("ip", QStringList() << "netns" << "del" << namespaceName);
    process.waitForFinished();
    
    // Remove veth pair
    QString vethHost = QString("veth-%1-host").arg(segmentId);
    process.start("ip", QStringList() << "link" << "del" << vethHost);
    process.waitForFinished();
    
    return true;
}

bool NetworkIsolation::configureNamespaceIsolation(const SegmentConfig& segment) {
    QString namespaceName = QString("milos-seg-%1").arg(segment.segmentId);
    
    // Configure firewall rules in namespace for isolation
    if (segment.isIsolated) {
        // Block all traffic except loopback
        QProcess process;
        process.start("ip", QStringList() << "netns" << "exec" << namespaceName << "iptables" << "-A" << "INPUT" << "-i" << "lo" << "-j" << "ACCEPT");
        process.waitForFinished();
        
        process.start("ip", QStringList() << "netns" << "exec" << namespaceName << "iptables" << "-A" << "INPUT" << "-j" << "DROP");
        process.waitForFinished();
        
        process.start("ip", QStringList() << "netns" << "exec" << namespaceName << "iptables" << "-A" << "OUTPUT" << "-o" << "lo" << "-j" << "ACCEPT");
        process.waitForFinished();
        
        process.start("ip", QStringList() << "netns" << "exec" << namespaceName << "iptables" << "-A" << "OUTPUT" << "-j" << "DROP");
        process.waitForFinished();
    }
    
    return true;
}

