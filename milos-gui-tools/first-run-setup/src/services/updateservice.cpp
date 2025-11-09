#include "updateservice.h"

UpdateService::UpdateService(QObject *parent)
    : QObject(parent)
    , m_status("checking")
    , m_checking(false)
{
}

void UpdateService::checkForUpdates()
{
    if (m_checking) {
        return;
    }
    
    m_checking = true;
    m_status = "checking";
    emit checkingChanged();
    emit statusChanged();
    
    // TODO: Implement actual update check
    // This would use pacman to check for available updates
    
    m_status = "updates_available";
    m_checking = false;
    emit checkingChanged();
    emit statusChanged();
}

bool UpdateService::hasUpdates()
{
    return m_status == "updates_available";
}

