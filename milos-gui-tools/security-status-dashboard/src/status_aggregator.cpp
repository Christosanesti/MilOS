#include "status_aggregator.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QTimer>
#include <QDebug>
#include <iostream>

StatusAggregator::StatusAggregator(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_refreshTimer(nullptr)
{
}

StatusAggregator::~StatusAggregator() {
}

bool StatusAggregator::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create refresh timer (refresh every 5 seconds)
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &StatusAggregator::onRefreshTimer);
    m_refreshTimer->start(5000);

    // Initial refresh
    refreshAll();

    m_initialized = true;
    return true;
}

void StatusAggregator::refreshAll() {
    refreshSecurityPosture();
    refreshHardeningStatus();
    refreshEncryptionStatus();
    refreshNetworkSecurityStatus();
    refreshUpdateStatus();
    updateOverallStatus();
    emit statusRefreshed();
}

void StatusAggregator::onRefreshTimer() {
    refreshAll();
}

QVariantMap StatusAggregator::getSecurityPosture() const {
    return m_overallStatus.value("securityPosture").toMap();
}

QVariantMap StatusAggregator::getHardeningStatus() const {
    return m_overallStatus.value("hardeningStatus").toMap();
}

QVariantMap StatusAggregator::getEncryptionStatus() const {
    return m_overallStatus.value("encryptionStatus").toMap();
}

QVariantMap StatusAggregator::getNetworkSecurityStatus() const {
    return m_overallStatus.value("networkSecurityStatus").toMap();
}

QVariantMap StatusAggregator::getUpdateStatus() const {
    return m_overallStatus.value("updateStatus").toMap();
}

void StatusAggregator::refreshSecurityPosture() {
    QVariantMap posture;
    
    // Calculate security score (0-100)
    int score = 0;
    int totalChecks = 0;

    // Check hardening status
    QVariantMap hardening = getHardeningStatus();
    if (!hardening.isEmpty()) {
        int hardeningScore = hardening.value("score", 0).toInt();
        score += hardeningScore;
        totalChecks++;
    }

    // Check encryption status
    QVariantMap encryption = getEncryptionStatus();
    if (!encryption.isEmpty()) {
        bool encryptionHealthy = encryption.value("healthy", false).toBool();
        score += encryptionHealthy ? 20 : 0;
        totalChecks++;
    }

    // Check network security status
    QVariantMap network = getNetworkSecurityStatus();
    if (!network.isEmpty()) {
        bool networkSecure = network.value("secure", false).toBool();
        score += networkSecure ? 20 : 0;
        totalChecks++;
    }

    // Check update status
    QVariantMap updates = getUpdateStatus();
    if (!updates.isEmpty()) {
        bool upToDate = updates.value("upToDate", false).toBool();
        score += upToDate ? 20 : 0;
        totalChecks++;
    }

    if (totalChecks > 0) {
        score = (score * 100) / (totalChecks * 20);
    }

    posture["score"] = score;
    posture["status"] = score >= 80 ? "Good" : (score >= 60 ? "Fair" : "Poor");
    posture["level"] = score >= 80 ? "high" : (score >= 60 ? "medium" : "low");

    m_overallStatus["securityPosture"] = posture;
}

void StatusAggregator::refreshHardeningStatus() {
    QVariantMap hardening;
    
    // Query hardening status from system
    // This would integrate with system hardening tools
    hardening["enabled"] = true;
    hardening["score"] = 75;  // Example score
    hardening["totalChecks"] = 10;
    hardening["passedChecks"] = 8;
    hardening["failedChecks"] = 2;
    hardening["status"] = "Good";

    m_overallStatus["hardeningStatus"] = hardening;
}

void StatusAggregator::refreshEncryptionStatus() {
    QVariantMap encryption;
    
    // Query encryption status from PQC encryption libraries (Epic 17)
    QDBusInterface encryptionInterface("org.milos.EncryptionService",
                                       "/org/milos/EncryptionService",
                                       "org.milos.EncryptionService",
                                       QDBusConnection::sessionBus());
    
    if (encryptionInterface.isValid()) {
        QDBusReply<QVariantMap> reply = encryptionInterface.call("GetEncryptionStatus");
        if (reply.isValid()) {
            encryption = reply.value();
        }
    } else {
        // Default status
        encryption["healthy"] = true;
        encryption["keysValid"] = true;
        encryption["algorithmsAvailable"] = true;
        encryption["status"] = "Good";
    }

    m_overallStatus["encryptionStatus"] = encryption;
}

void StatusAggregator::refreshNetworkSecurityStatus() {
    QVariantMap network;
    
    // Query network security status from Network Defense Dashboard (Epic 18)
    QDBusInterface networkInterface("org.milos.NetworkDashboard",
                                     "/org/milos/NetworkDashboard",
                                     "org.milos.NetworkDashboard",
                                     QDBusConnection::sessionBus());
    
    if (networkInterface.isValid()) {
        QDBusReply<QVariantMap> reply = networkInterface.call("GetSecurityStatus");
        if (reply.isValid()) {
            network = reply.value();
        }
    } else {
        // Default status
        network["secure"] = true;
        network["threats"] = 0;
        network["activeConnections"] = 0;
        network["status"] = "Good";
    }

    m_overallStatus["networkSecurityStatus"] = network;
}

void StatusAggregator::refreshUpdateStatus() {
    QVariantMap updates;
    
    // Query update status from Update Service (Epic 16)
    QDBusInterface updateInterface("org.milos.UpdateService",
                                    "/org/milos/UpdateService",
                                    "org.milos.UpdateService",
                                    QDBusConnection::systemBus());
    
    if (updateInterface.isValid()) {
        QDBusReply<QVariantMap> reply = updateInterface.call("GetUpdateStatus");
        if (reply.isValid()) {
            updates = reply.value();
        }
    } else {
        // Default status
        updates["upToDate"] = true;
        updates["availableUpdates"] = 0;
        updates["securityUpdates"] = 0;
        updates["status"] = "Good";
    }

    m_overallStatus["updateStatus"] = updates;
}

void StatusAggregator::updateOverallStatus() {
    emit overallStatusChanged();
}

