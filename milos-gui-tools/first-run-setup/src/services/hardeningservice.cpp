#include "hardeningservice.h"

HardeningService::HardeningService(QObject *parent)
    : QObject(parent)
    , m_level("basic")
    , m_progress(0.0)
{
}

void HardeningService::checkStatus()
{
    // TODO: Implement actual hardening status check
    // This would check hardening configuration and calculate progress
    
    m_level = "standard";
    m_progress = 30.0;
    emit levelChanged();
    emit progressChanged();
}

QString HardeningService::getLevel()
{
    return m_level;
}

