#include "emergency_eject.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>

EmergencyEject::EmergencyEject(QObject* parent)
    : QObject(parent)
    , m_ejecting(false)
{
}

EmergencyEject::~EmergencyEject() {
}

bool EmergencyEject::initialize() {
    return true;
}

bool EmergencyEject::executeEject(const QString& confirmationCode) {
    if (m_ejecting) {
        return false;
    }

    if (confirmationCode != "EMERGENCY_EJECT") {
        emit ejectFailed("Invalid confirmation code");
        return false;
    }

    m_ejecting = true;
    emit ejectInitiated();

    // Delete all data
    if (!deleteAllData()) {
        m_ejecting = false;
        emit ejectFailed("Failed to delete all data");
        return false;
    }

    // Dismantle system
    if (!dismantleSystem()) {
        m_ejecting = false;
        emit ejectFailed("Failed to dismantle system");
        return false;
    }

    // Verify deletion
    if (!verifyDeletion()) {
        m_ejecting = false;
        emit ejectFailed("Failed to verify deletion");
        return false;
    }

    m_ejecting = false;
    emit ejectCompleted();

    return true;
}

bool EmergencyEject::isEjecting() const {
    return m_ejecting;
}

bool EmergencyEject::deleteAllData() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    
    if (appDataDir.exists()) {
        return appDataDir.removeRecursively();
    }
    
    return true;
}

bool EmergencyEject::dismantleSystem() {
    // In production, would stop all services, close connections, etc.
    return true;
}

bool EmergencyEject::verifyDeletion() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    
    return !appDataDir.exists();
}

