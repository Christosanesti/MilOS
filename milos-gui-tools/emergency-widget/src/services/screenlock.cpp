#include "screenlock.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDebug>
#include <unistd.h>

ScreenLock::ScreenLock(QObject *parent)
    : QObject(parent)
{
}

void ScreenLock::lockScreen()
{
    // Implement screen lock via D-Bus
    // Try KWin first (KDE Plasma), then fallback to systemd-logind
    
    bool success = false;
    
    // Try KWin first (KDE Plasma)
    success = lockViaKWin();
    
    // Fallback to systemd-logind
    if (!success) {
        success = lockViaSystemdLogind();
    }
    
    if (success) {
        emit screenLocked();
    } else {
        emit error("Failed to lock screen. Please check system permissions.");
    }
}

bool ScreenLock::lockViaKWin()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.kglobalaccel",
                                  "/kglobalaccel",
                                  "org.kde.KGlobalAccel",
                                  bus);
    
    // Try KWin compositor interface
    if (!kwinInterface.isValid()) {
        QDBusInterface kwinCompositor("org.kde.KWin",
                                      "/org/kde/KWin",
                                      "org.kde.KWin",
                                      bus);
        if (kwinCompositor.isValid()) {
            QDBusReply<void> reply = kwinCompositor.call("lockScreen");
            if (reply.isValid()) {
                qDebug() << "Screen locked via KWin";
                return true;
            }
        }
    }
    
    // Alternative: Use org.kde.krunner interface
    QDBusInterface krunnerInterface("org.kde.kglobalaccel",
                                     "/component/kwin",
                                     "org.kde.kglobalaccel.Component",
                                     bus);
    if (krunnerInterface.isValid()) {
        // Try to invoke lock screen action
        QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.kglobalaccel",
                                                           "/component/kwin",
                                                           "org.kde.kglobalaccel.Component",
                                                           "invokeShortcut");
        msg << "Lock Session";
        QDBusReply<void> reply = bus.call(msg);
        if (reply.isValid()) {
            qDebug() << "Screen locked via KGlobalAccel";
            return true;
        }
    }
    
    return false;
}

bool ScreenLock::lockViaSystemdLogind()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    
    // Get current session
    QDBusInterface logindInterface("org.freedesktop.login1",
                                    "/org/freedesktop/login1",
                                    "org.freedesktop.login1.Manager",
                                    bus);
    
    if (!logindInterface.isValid()) {
        qWarning() << "systemd-logind D-Bus interface not available";
        return false;
    }
    
    // Get current session ID
    QDBusReply<QString> sessionReply = logindInterface.call("GetSessionByPID", static_cast<uint>(getpid()));
    if (!sessionReply.isValid()) {
        qWarning() << "Failed to get current session:" << sessionReply.error().message();
        return false;
    }
    
    QString sessionPath = sessionReply.value();
    
    // Lock the session
    QDBusInterface sessionInterface("org.freedesktop.login1",
                                     sessionPath,
                                     "org.freedesktop.login1.Session",
                                     bus);
    
    if (sessionInterface.isValid()) {
        QDBusReply<void> reply = sessionInterface.call("Lock");
        if (reply.isValid()) {
            qDebug() << "Screen locked via systemd-logind";
            return true;
        } else {
            qWarning() << "Failed to lock session:" << reply.error().message();
        }
    }
    
    return false;
}

