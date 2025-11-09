#include "StatusIndicator.h"

StatusIndicator::StatusIndicator(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("badge")
    , m_state("active")
    , m_text("")
    , m_icon("")
{
}

void StatusIndicator::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void StatusIndicator::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void StatusIndicator::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

void StatusIndicator::setIcon(const QString &icon)
{
    if (m_icon != icon) {
        m_icon = icon;
        emit iconChanged();
    }
}

