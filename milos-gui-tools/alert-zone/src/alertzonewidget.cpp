#include "alertzonewidget.h"

AlertZoneWidget::AlertZoneWidget(QQuickItem* parent)
    : QQuickItem(parent)
    , m_alertCount(0)
{
}

AlertZoneWidget::~AlertZoneWidget()
{
}

void AlertZoneWidget::addAlert(const QString& severity, const QString& category, const QString& message)
{
    m_alertCount++;
    emit alertCountChanged(m_alertCount);
}

void AlertZoneWidget::clearAlerts()
{
    m_alertCount = 0;
    emit alertCountChanged(m_alertCount);
}

