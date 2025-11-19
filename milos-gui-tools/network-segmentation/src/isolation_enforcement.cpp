#include "isolation_enforcement.h"
#include "network_isolation.h"
#include "security_zone_enforcement.h"
#include "access_control.h"
#include "segment_config.h"
#include <iostream>

IsolationEnforcement::IsolationEnforcement(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_networkIsolation(nullptr)
    , m_zoneEnforcement(nullptr)
    , m_accessControl(nullptr)
{
}

IsolationEnforcement::~IsolationEnforcement() {
}

bool IsolationEnforcement::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize network isolation
    m_networkIsolation = new NetworkIsolation(this);
    if (!m_networkIsolation->initialize()) {
        std::cerr << "Failed to initialize network isolation" << std::endl;
        return false;
    }

    // Initialize security zone enforcement
    m_zoneEnforcement = new SecurityZoneEnforcement(this);
    if (!m_zoneEnforcement->initialize()) {
        std::cerr << "Failed to initialize security zone enforcement" << std::endl;
        return false;
    }

    // Initialize access control
    m_accessControl = new AccessControl(this);
    if (!m_accessControl->initialize()) {
        std::cerr << "Failed to initialize access control" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool IsolationEnforcement::enforceIsolation(const SegmentConfig& segment) {
    if (!m_initialized) {
        return false;
    }

    // Enforce kernel-level network isolation
    if (!m_networkIsolation->isolateSegment(segment)) {
        std::cerr << "Failed to enforce network isolation for segment: " << segment.segmentId.toStdString() << std::endl;
        return false;
    }

    // If segment is isolated, enforce strict isolation
    if (segment.isIsolated) {
        // Create security zone for isolated segment
        SecurityZonePolicy zonePolicy;
        zonePolicy.name = QString("Isolated Zone: %1").arg(segment.name);
        zonePolicy.description = QString("Security zone for isolated segment %1").arg(segment.name);
        zonePolicy.allowedSegments = QStringList() << segment.segmentId;
        zonePolicy.enforceIsolation = true;
        zonePolicy.priority = 100;

        QString zoneId = m_zoneEnforcement->createZone(zonePolicy);
        if (!zoneId.isEmpty()) {
            m_zoneEnforcement->enforceZonePolicy(segment.segmentId, zoneId);
        }
    }

    emit isolationEnforced(segment.segmentId);
    return true;
}

bool IsolationEnforcement::removeIsolation(const QString& segmentId) {
    if (!m_initialized) {
        return false;
    }

    // Remove network isolation
    if (!m_networkIsolation->removeIsolation(segmentId)) {
        std::cerr << "Failed to remove network isolation for segment: " << segmentId.toStdString() << std::endl;
        return false;
    }

    // Remove zone enforcement
    QStringList zones = m_zoneEnforcement->getZonesForSegment(segmentId);
    for (const QString& zoneId : zones) {
        m_zoneEnforcement->deleteZone(zoneId);
    }

    emit isolationRemoved(segmentId);
    return true;
}

bool IsolationEnforcement::verifyIsolation(const QString& segmentId) const {
    if (!m_initialized) {
        return false;
    }

    return m_networkIsolation->verifyIsolation(segmentId);
}

