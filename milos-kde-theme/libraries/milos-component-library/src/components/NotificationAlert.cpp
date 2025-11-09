#include "NotificationAlert.h"

NotificationAlert::NotificationAlert(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("info")
    , m_state("default")
    , m_title("")
    , m_message("")
    , m_dismissible(true)
    , m_requiresAcknowledgment(false)
    , m_duration(3000)
{
}

void NotificationAlert::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void NotificationAlert::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void NotificationAlert::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void NotificationAlert::setMessage(const QString &message)
{
    if (m_message != message) {
        m_message = message;
        emit messageChanged();
    }
}

void NotificationAlert::setDismissible(bool dismissible)
{
    if (m_dismissible != dismissible) {
        m_dismissible = dismissible;
        emit dismissibleChanged();
    }
}

void NotificationAlert::setRequiresAcknowledgment(bool required)
{
    if (m_requiresAcknowledgment != required) {
        m_requiresAcknowledgment = required;
        emit requiresAcknowledgmentChanged();
    }
}

void NotificationAlert::setDuration(int duration)
{
    if (m_duration != duration) {
        m_duration = duration;
        emit durationChanged();
    }
}

