#include "ModalDialog.h"

ModalDialog::ModalDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("confirmation")
    , m_state("displaying")
    , m_title("")
    , m_message("")
    , m_critical(false)
    , m_visible(false)
{
    setFlag(ItemIsFocusScope, true);
}

void ModalDialog::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void ModalDialog::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void ModalDialog::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void ModalDialog::setMessage(const QString &message)
{
    if (m_message != message) {
        m_message = message;
        emit messageChanged();
    }
}

void ModalDialog::setCritical(bool critical)
{
    if (m_critical != critical) {
        m_critical = critical;
        emit criticalChanged();
    }
}

void ModalDialog::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        emit visibleChanged();
    }
}

