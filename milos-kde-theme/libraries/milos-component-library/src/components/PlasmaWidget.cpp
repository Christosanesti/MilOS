#include "PlasmaWidget.h"

PlasmaWidget::PlasmaWidget(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("security-posture")
    , m_state("default")
    , m_status("secure")
    , m_title("")
    , m_subtitle("")
    , m_expanded(false)
{
}

void PlasmaWidget::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void PlasmaWidget::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void PlasmaWidget::setStatus(const QString &status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void PlasmaWidget::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void PlasmaWidget::setSubtitle(const QString &subtitle)
{
    if (m_subtitle != subtitle) {
        m_subtitle = subtitle;
        emit subtitleChanged();
    }
}

void PlasmaWidget::setExpanded(bool expanded)
{
    if (m_expanded != expanded) {
        m_expanded = expanded;
        emit expandedChanged();
    }
}

