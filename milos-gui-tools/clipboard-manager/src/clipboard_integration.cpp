#include "clipboard_integration.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QDebug>

ClipboardIntegration::ClipboardIntegration(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_available(false)
{
}

ClipboardIntegration::~ClipboardIntegration() {
}

bool ClipboardIntegration::initialize() {
    if (m_initialized) {
        return true;
    }

    // Check if QGuiApplication is available
    QGuiApplication* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    if (!app) {
        qWarning() << "QGuiApplication not available for clipboard integration";
        return false;
    }

    m_available = true;
    m_initialized = true;
    return true;
}

QString ClipboardIntegration::getClipboardText() const {
    if (!m_available) {
        return QString();
    }

    QGuiApplication* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    if (!app) {
        return QString();
    }

    QClipboard* clipboard = app->clipboard();
    if (!clipboard) {
        return QString();
    }

    return clipboard->text();
}

bool ClipboardIntegration::setClipboardText(const QString& text) {
    if (!m_available) {
        return false;
    }

    QGuiApplication* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    if (!app) {
        return false;
    }

    QClipboard* clipboard = app->clipboard();
    if (!clipboard) {
        return false;
    }

    clipboard->setText(text);
    return true;
}

bool ClipboardIntegration::clearClipboard() {
    if (!m_available) {
        return false;
    }

    return setClipboardText(QString());
}

