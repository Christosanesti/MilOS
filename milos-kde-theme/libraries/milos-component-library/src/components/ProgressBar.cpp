#include "ProgressBar.h"

ProgressBar::ProgressBar(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("linear")
    , m_state("inprogress")
    , m_value(0.0)
    , m_text("")
    , m_details("")
    , m_stepCount(5)
    , m_currentStep(0)
    , m_estimatedTime("")
{
}

void ProgressBar::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void ProgressBar::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
    }
}

void ProgressBar::setValue(qreal value)
{
    qreal clampedValue = qBound(0.0, value, 1.0);
    if (m_value != clampedValue) {
        m_value = clampedValue;
        emit valueChanged();
    }
}

void ProgressBar::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

void ProgressBar::setDetails(const QString &details)
{
    if (m_details != details) {
        m_details = details;
        emit detailsChanged();
    }
}

void ProgressBar::setStepCount(int count)
{
    if (m_stepCount != count) {
        m_stepCount = count;
        emit stepCountChanged();
    }
}

void ProgressBar::setCurrentStep(int step)
{
    if (m_currentStep != step) {
        m_currentStep = step;
        emit currentStepChanged();
    }
}

void ProgressBar::setEstimatedTime(const QString &time)
{
    if (m_estimatedTime != time) {
        m_estimatedTime = time;
        emit estimatedTimeChanged();
    }
}

