#include "Button.h"

Button::Button(QQuickItem *parent)
    : QQuickItem(parent)
    , m_text("")
    , m_variant("primary")
    , m_state("default")
    , m_checked(false)
    , m_enabled(true)
{
    setFlag(ItemIsFocusScope, true);
}

void Button::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

void Button::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void Button::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void Button::setChecked(bool checked)
{
    if (m_checked != checked) {
        m_checked = checked;
        emit checkedChanged();
    }
}

void Button::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

