#include "Card.h"

Card::Card(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("dashboard")
    , m_state("default")
    , m_title("")
    , m_subtitle("")
    , m_expandable(false)
    , m_expanded(false)
    , m_statusColor(QColor("#00cc66"))
{
}

void Card::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void Card::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void Card::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void Card::setSubtitle(const QString &subtitle)
{
    if (m_subtitle != subtitle) {
        m_subtitle = subtitle;
        emit subtitleChanged();
    }
}

void Card::setExpandable(bool expandable)
{
    if (m_expandable != expandable) {
        m_expandable = expandable;
        emit expandableChanged();
    }
}

void Card::setExpanded(bool expanded)
{
    if (m_expanded != expanded) {
        m_expanded = expanded;
        emit expandedChanged();
    }
}

void Card::setStatusColor(const QColor &color)
{
    if (m_statusColor != color) {
        m_statusColor = color;
        emit statusColorChanged();
    }
}

