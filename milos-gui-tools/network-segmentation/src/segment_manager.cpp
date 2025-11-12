#include "segment_manager.h"
#include "segment_config.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonObject>
#include <QVariantMap>
#include <QDebug>
#include <iostream>

SegmentManager::SegmentManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_configManager(nullptr)
{
}

SegmentManager::~SegmentManager() {
}

bool SegmentManager::initialize(const QString& dbPath) {
    if (m_initialized) {
        return true;
    }

    QString actualDbPath = dbPath;
    if (actualDbPath.isEmpty()) {
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        actualDbPath = dataDir + "/network_segments.db";
    }

    m_configManager = new SegmentConfigManager(this);
    connect(m_configManager, &SegmentConfigManager::segmentCreated,
            this, &SegmentManager::onSegmentCreated);
    connect(m_configManager, &SegmentConfigManager::segmentUpdated,
            this, &SegmentManager::onSegmentUpdated);
    connect(m_configManager, &SegmentConfigManager::segmentDeleted,
            this, &SegmentManager::onSegmentDeleted);

    if (!m_configManager->initialize(actualDbPath)) {
        std::cerr << "Failed to initialize segment config manager" << std::endl;
        return false;
    }

    updateSegmentsList();

    m_initialized = true;
    return true;
}

QString SegmentManager::createSegment(const QString& name, const QString& networkAddress, const QString& description) {
    if (!m_initialized || !m_configManager) {
        return QString();
    }

    SegmentConfig config;
    config.name = name;
    config.networkAddress = networkAddress;
    config.description = description;

    return m_configManager->createSegment(config);
}

bool SegmentManager::updateSegment(const QString& segmentId, const QString& name, const QString& networkAddress, const QString& description) {
    if (!m_initialized || !m_configManager) {
        return false;
    }

    SegmentConfig config = m_configManager->getSegment(segmentId);
    if (config.segmentId.isEmpty()) {
        return false;
    }

    config.name = name;
    config.networkAddress = networkAddress;
    config.description = description;

    return m_configManager->updateSegment(segmentId, config);
}

bool SegmentManager::deleteSegment(const QString& segmentId) {
    if (!m_initialized || !m_configManager) {
        return false;
    }

    return m_configManager->deleteSegment(segmentId);
}

QVariantMap SegmentManager::getSegment(const QString& segmentId) const {
    if (!m_configManager) {
        return QVariantMap();
    }

    SegmentConfig config = m_configManager->getSegment(segmentId);
    if (config.segmentId.isEmpty()) {
        return QVariantMap();
    }

    QVariantMap map;
    map["segmentId"] = config.segmentId;
    map["name"] = config.name;
    map["description"] = config.description;
    map["networkAddress"] = config.networkAddress;
    map["isIsolated"] = config.isIsolated;
    map["priority"] = config.priority;

    return map;
}

QStringList SegmentManager::getSegmentsByNetwork(const QString& networkAddress) const {
    if (!m_configManager) {
        return QStringList();
    }

    QList<SegmentConfig> segments = m_configManager->getSegmentsByNetwork(networkAddress);
    QStringList segmentIds;
    for (const SegmentConfig& config : segments) {
        segmentIds.append(config.segmentId);
    }
    return segmentIds;
}

void SegmentManager::onSegmentCreated(const QString& segmentId) {
    updateSegmentsList();
    emit segmentCreated(segmentId);
}

void SegmentManager::onSegmentUpdated(const QString& segmentId) {
    updateSegmentsList();
    emit segmentUpdated(segmentId);
}

void SegmentManager::onSegmentDeleted(const QString& segmentId) {
    updateSegmentsList();
    emit segmentDeleted(segmentId);
}

void SegmentManager::updateSegmentsList() {
    if (!m_configManager) {
        return;
    }

    m_segments = m_configManager->listSegments();
    emit segmentsChanged();
}

