#include "InputField.h"

InputField::InputField(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("text")
    , m_state("default")
    , m_placeholder("")
    , m_label("")
    , m_value("")
    , m_errorMessage("")
    , m_enabled(true)
    , m_readOnly(false)
    , m_passwordStrength(0)
    , m_maxLength(-1)
{
}

void InputField::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void InputField::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void InputField::setPlaceholder(const QString &placeholder)
{
    if (m_placeholder != placeholder) {
        m_placeholder = placeholder;
        emit placeholderChanged();
    }
}

void InputField::setLabel(const QString &label)
{
    if (m_label != label) {
        m_label = label;
        emit labelChanged();
    }
}

void InputField::setValue(const QString &value)
{
    if (m_value != value) {
        m_value = value;
        emit valueChanged();
    }
}

void InputField::setErrorMessage(const QString &errorMessage)
{
    if (m_errorMessage != errorMessage) {
        m_errorMessage = errorMessage;
        emit errorMessageChanged();
    }
}

void InputField::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

void InputField::setReadOnly(bool readOnly)
{
    if (m_readOnly != readOnly) {
        m_readOnly = readOnly;
        emit readOnlyChanged();
    }
}

void InputField::setPasswordStrength(int strength)
{
    if (m_passwordStrength != strength) {
        m_passwordStrength = strength;
        emit passwordStrengthChanged();
    }
}

void InputField::setMaxLength(int maxLength)
{
    if (m_maxLength != maxLength) {
        m_maxLength = maxLength;
        emit maxLengthChanged();
    }
}

