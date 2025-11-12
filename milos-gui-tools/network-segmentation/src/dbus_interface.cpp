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

