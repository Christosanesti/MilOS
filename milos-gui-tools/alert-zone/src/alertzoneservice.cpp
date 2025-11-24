#include "alertzoneservice.h"
#include <QDebug>

AlertZoneService::AlertZoneService(QObject* parent)
    : QObject(parent)
    , m_alertCount(0)
{
}

AlertZoneService::~AlertZoneService()
{
}

void AlertZoneService::addAlert(const QString& severity, const QString& category, const QString& message)
{
    m_alertCount++;
    emit alertReceived(severity, category, message);
    emit alertCountChanged(m_alertCount);
}

void AlertZoneService::clearAlerts()
{
    m_alertCount = 0;
    emit alertsCleared();
    emit alertCountChanged(m_alertCount);
}

