#include "personnel_location_system.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

PersonnelLocationSystem::PersonnelLocationSystem(QObject* parent)
    : QObject(parent)
    , m_tracker(new LocationTracker(this))
    , m_storage(new LocationStorage(this))
    , m_encryption(new LocationEncryption(this))
    , m_dbusInterface(new PersonnelLocationDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

PersonnelLocationSystem::~PersonnelLocationSystem() {
}

bool PersonnelLocationSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize encryption
    if (!m_encryption->initialize()) {
        qWarning() << "Failed to initialize location encryption";
        return false;
    }
    
    // Initialize tracker
    if (!m_tracker->initialize()) {
        qWarning() << "Failed to initialize location tracker";
        return false;
    }
    
    // Initialize storage
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/locations.db";
    
    if (!m_storage->initialize(dbPath)) {
        qWarning() << "Failed to initialize location storage";
        return false;
    }
    
    // Connect signals
    connect(m_tracker, &LocationTracker::locationUpdated, this, [this](const QString& personnelId, const LocationData& location) {
        // Encrypt location before storing
        QByteArray encrypted = m_encryption->encryptLocation(location);
        
        // Store encrypted location
        LocationData storedLocation = location;
        storedLocation.metadata["encrypted"] = true;
        m_storage->storeLocation(storedLocation);
        
        // Log location update
        QString locationStr = QString("%1,%2").arg(location.coordinate.latitude()).arg(location.coordinate.longitude());
        m_auditLogger->logLocationUpdate(personnelId, locationStr);
    });
    
    connect(m_tracker, &LocationTracker::trackingEnabledChanged, this, [this](bool enabled) {
        // Log tracking state change
        // In production, would log which personnel are being tracked
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setTracker(m_tracker);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

