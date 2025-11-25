#include "statusindicators.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>
#include <QNetworkInterface>

StatusIndicators::StatusIndicators(QObject* parent)
    : QObject(parent)
    , m_networkActive(false)
    , m_secureModeActive(false)
    , m_networkStatus("Unknown")
    , m_secureModeStatus("Inactive")
{
    updateAll();
}

StatusIndicators::~StatusIndicators()
{
}

void StatusIndicators::updateNetworkStatus()
{
    bool wasActive = m_networkActive;
    m_networkActive = checkNetworkStatus();
    
    if (m_networkActive) {
        m_networkStatus = "Active";
    } else {
        m_networkStatus = "Inactive";
    }
    
    if (wasActive != m_networkActive) {
        emit networkActiveChanged();
        emit networkStatusChanged();
    }
}

void StatusIndicators::updateSecureModeStatus()
{
    bool wasActive = m_secureModeActive;
    m_secureModeActive = checkSecureModeStatus();
    
    if (m_secureModeActive) {
        m_secureModeStatus = "Active";
    } else {
        m_secureModeStatus = "Inactive";
    }
    
    if (wasActive != m_secureModeActive) {
        emit secureModeActiveChanged();
        emit secureModeStatusChanged();
    }
}

void StatusIndicators::updateAll()
{
    updateNetworkStatus();
    updateSecureModeStatus();
}

bool StatusIndicators::checkNetworkStatus()
{
    // Check if any network interface is active
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            return true;
        }
    }
    return false;
}

bool StatusIndicators::checkSecureModeStatus()
{
    // Check secure mode state file
    QString stateFile = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/milos-secure-mode.state";
    return QFile::exists(stateFile);
}

