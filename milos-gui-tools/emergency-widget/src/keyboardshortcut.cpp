#include "keyboardshortcut.h"
#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QGraphicsEffect>
#include <QPropertyAnimation>

KeyboardShortcut::KeyboardShortcut(QObject* parent)
    : QObject(parent)
    , m_shortcut("Alt+E")
    , m_enabled(true)
    , m_qtShortcut(nullptr)
{
}

KeyboardShortcut::~KeyboardShortcut()
{
    unregisterShortcut();
}

void KeyboardShortcut::setShortcut(const QString& shortcut)
{
    if (m_shortcut != shortcut) {
        m_shortcut = shortcut;
        emit shortcutChanged();
        if (m_enabled) {
            registerShortcut();
        }
    }
}

void KeyboardShortcut::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (enabled) {
            registerShortcut();
        } else {
            unregisterShortcut();
        }
        emit enabledChanged();
    }
}

void KeyboardShortcut::registerShortcut()
{
    unregisterShortcut();
    
    if (!m_enabled || m_shortcut.isEmpty()) {
        return;
    }
    
    // For KDE Plasma, we would use KGlobalAccel
    // For now, use QShortcut which works within the application
    // In production, integrate with KGlobalAccel for system-wide shortcuts
    
    QKeySequence keySeq(m_shortcut);
    if (keySeq.isEmpty()) {
        emit error("Invalid keyboard shortcut: " + m_shortcut);
        return;
    }
    
    // Get the main widget (Plasmoid)
    QWidget* mainWidget = qApp->activeWindow();
    if (!mainWidget) {
        // Try to find any widget
        QWidgetList widgets = qApp->allWidgets();
        if (!widgets.isEmpty()) {
            mainWidget = widgets.first();
        }
    }
    
    if (mainWidget) {
        m_qtShortcut = new QShortcut(keySeq, mainWidget);
        connect(m_qtShortcut, &QShortcut::activated, this, &KeyboardShortcut::onShortcutActivated);
        qDebug() << "Registered keyboard shortcut:" << m_shortcut;
    } else {
        emit error("Cannot register shortcut: No widget available");
    }
}

void KeyboardShortcut::unregisterShortcut()
{
    if (m_qtShortcut) {
        delete m_qtShortcut;
        m_qtShortcut = nullptr;
    }
}

void KeyboardShortcut::onShortcutActivated()
{
    createXenonFlash();
    emit activated();
}

void KeyboardShortcut::createXenonFlash()
{
    // Create xenon flash feedback
    // This would typically flash the widget or show a visual indicator
    qDebug() << "Keyboard shortcut activated - xenon flash feedback";
    // In QML, this would trigger a visual flash animation
}

