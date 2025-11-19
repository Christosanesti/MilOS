#include "dbus_interface.h"
#include "segment_manager.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_segmentManager(nullptr)
    , m_firewallManager(nullptr)
{
}

DBusInterface::~DBusInterface() {
}

bool DBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    // Register D-Bus service
    if (!QDBusConnection::sessionBus().registerService("org.milos.NetworkSegmentation")) {
        std::cerr << "Failed to register D-Bus service" << std::endl;
        return false;
    }

    // Register object
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/NetworkSegmentation", this,
                                                      QDBusConnection::ExportAllContents)) {
        std::cerr << "Failed to register D-Bus object" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void DBusInterface::setSegmentManager(class SegmentManager* segmentManager) {
    m_segmentManager = segmentManager;
    
    if (segmentManager) {
        connect(segmentManager, &SegmentManager::segmentCreated,
                this, &DBusInterface::SegmentCreated);
        connect(segmentManager, &SegmentManager::segmentUpdated,
                this, &DBusInterface::SegmentUpdated);
        connect(segmentManager, &SegmentManager::segmentDeleted,
                this, &DBusInterface::SegmentDeleted);
    }
}

void DBusInterface::setFirewallManager(class FirewallManager* firewallManager) {
    m_firewallManager = firewallManager;
}

QString DBusInterface::CreateSegment(const QString& name, const QString& networkAddress, const QString& description) {
    if (!m_segmentManager) {
        return QString();
    }

    QString segmentId = m_segmentManager->createSegment(name, networkAddress, description);
    return segmentId;
}

bool DBusInterface::UpdateSegment(const QString& segmentId, const QString& name, const QString& networkAddress, const QString& description) {
    if (!m_segmentManager) {
        return false;
    }

    return m_segmentManager->updateSegment(segmentId, name, networkAddress, description);
}

bool DBusInterface::DeleteSegment(const QString& segmentId) {
    if (!m_segmentManager) {
        return false;
    }

    return m_segmentManager->deleteSegment(segmentId);
}

QString DBusInterface::GetSegment(const QString& segmentId) {
    if (!m_segmentManager) {
        return QString();
    }

    QVariantMap segment = m_segmentManager->getSegment(segmentId);
    if (segment.isEmpty()) {
        return QString();
    }

    QJsonObject obj;
    obj["segmentId"] = segment["segmentId"].toString();
    obj["name"] = segment["name"].toString();
    obj["description"] = segment["description"].toString();
    obj["networkAddress"] = segment["networkAddress"].toString();
    obj["isIsolated"] = segment["isIsolated"].toBool();
    obj["priority"] = segment["priority"].toInt();

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QStringList DBusInterface::ListSegments() {
    if (!m_segmentManager) {
        return QStringList();
    }

    QStringList segmentIds;
    QList<SegmentConfig> segments = m_segmentManager->segments();
    for (const SegmentConfig& segment : segments) {
        segmentIds.append(segment.segmentId);
    }
    return segmentIds;
}

QStringList DBusInterface::GetSegmentsByNetwork(const QString& networkAddress) {
    if (!m_segmentManager) {
        return QStringList();
    }

    return m_segmentManager->getSegmentsByNetwork(networkAddress);
}

bool DBusInterface::GenerateFirewallRules() {
    if (!m_firewallManager || !m_segmentManager) {
        return false;
    }

    return m_firewallManager->generateRulesFromSegments(m_segmentManager->segments());
}

QString DBusInterface::ValidateFirewallRules() {
    if (!m_firewallManager) {
        return QString("{\"isValid\":false,\"errors\":[\"Firewall manager not available\"]}");
    }

    return m_firewallManager->validateRules();
}

QString DBusInterface::PreviewFirewallRules() {
    if (!m_firewallManager) {
        return QString();
    }

    return m_firewallManager->previewRules();
}

bool DBusInterface::ApplyFirewallRules() {
    if (!m_firewallManager) {
        return false;
    }

    return m_firewallManager->applyRules();
}

bool DBusInterface::RollbackFirewallRules() {
    if (!m_firewallManager) {
        return false;
    }

    return m_firewallManager->rollbackRules();
}

