#include "dbus_interface.h"
#include "status_aggregator.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_statusAggregator(nullptr)
{
}

DBusInterface::~DBusInterface() {
}

bool DBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    // Register D-Bus service
    if (!QDBusConnection::sessionBus().registerService("org.milos.SecurityStatusDashboard")) {
        std::cerr << "Failed to register D-Bus service" << std::endl;
        return false;
    }

    // Register object
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SecurityStatusDashboard", this,
                                                      QDBusConnection::ExportAllContents)) {
        std::cerr << "Failed to register D-Bus object" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void DBusInterface::setStatusAggregator(class StatusAggregator* statusAggregator) {
    m_statusAggregator = statusAggregator;
    
    if (statusAggregator) {
        connect(statusAggregator, &StatusAggregator::statusRefreshed,
                this, &DBusInterface::StatusRefreshed);
    }
}

QString DBusInterface::GetOverallStatus() {
    if (!m_statusAggregator) {
        return QString("{}");
    }

    QVariantMap status = m_statusAggregator->overallStatus();
    QJsonObject obj = QJsonObject::fromVariantMap(status);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetSecurityPosture() {
    if (!m_statusAggregator) {
        return QString("{}");
    }

    QVariantMap posture = m_statusAggregator->getSecurityPosture();
    QJsonObject obj = QJsonObject::fromVariantMap(posture);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetHardeningStatus() {
    if (!m_statusAggregator) {
        return QString("{}");
    }

    QVariantMap hardening = m_statusAggregator->getHardeningStatus();
    QJsonObject obj = QJsonObject::fromVariantMap(hardening);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetEncryptionStatus() {
    if (!m_statusAggregator) {
        return QString("{}");
    }

    QVariantMap encryption = m_statusAggregator->getEncryptionStatus();
    QJsonObject obj = QJsonObject::fromVariantMap(encryption);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetNetworkSecurityStatus() {
    if (!m_statusAggregator) {
        return QString("{}");
    }

    QVariantMap network = m_statusAggregator->getNetworkSecurityStatus();
    QJsonObject obj = QJsonObject::fromVariantMap(network);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetUpdateStatus() {
    if (!m_statusAggregator) {
        return QString("{}");
    }

    QVariantMap updates = m_statusAggregator->getUpdateStatus();
    QJsonObject obj = QJsonObject::fromVariantMap(updates);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

bool DBusInterface::RefreshStatus() {
    if (!m_statusAggregator) {
        return false;
    }

    m_statusAggregator->refreshAll();
    return true;
}

