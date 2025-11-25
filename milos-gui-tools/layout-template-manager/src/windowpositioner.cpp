#include "windowpositioner.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QScreen>
#include <QGuiApplication>

WindowPositioner::WindowPositioner(QObject* parent)
    : QObject(parent)
{
}

bool WindowPositioner::positionWindows(const QVariantMap& template)
{
    if (!template.contains("window_positions")) {
        emit error("Template does not contain window_positions");
        return false;
    }

    QVariantList windowPositions = template.value("window_positions").toList();
    bool allSuccess = true;

    for (const QVariant& windowVar : windowPositions) {
        QVariantMap window = windowVar.toMap();
        QString applicationName = window.value("application_name").toString();
        int monitorIndex = window.value("monitor_index", 0).toInt();
        int x = window.value("x_position", 0).toInt();
        int y = window.value("y_position", 0).toInt();
        int width = window.value("width", 800).toInt();
        int height = window.value("height", 600).toInt();
        bool maximized = window.value("maximized", false).toBool();

        bool success = positionWindow(applicationName, monitorIndex, x, y, width, height, maximized);
        emit windowPositioned(applicationName, success);
        
        if (!success) {
            allSuccess = false;
        }
    }

    return allSuccess;
}

bool WindowPositioner::positionWindow(const QString& applicationName, int monitorIndex,
                                     int x, int y, int width, int height, bool maximized)
{
    // Get monitor geometry to adjust coordinates
    QVariantMap monitorGeometry = getMonitorGeometry(monitorIndex);
    if (monitorGeometry.isEmpty()) {
        emit error(QString("Monitor %1 not found").arg(monitorIndex));
        return false;
    }

    int monitorX = monitorGeometry.value("x", 0).toInt();
    int monitorY = monitorGeometry.value("y", 0).toInt();

    // Adjust coordinates relative to monitor
    int absoluteX = monitorX + x;
    int absoluteY = monitorY + y;

    // Find window
    QString windowId = findWindow(applicationName);
    if (windowId.isEmpty()) {
        qWarning() << "Window not found for application:" << applicationName;
        // Try to launch application if not found
        // This would be handled by application launcher
        return false;
    }

    return positionWindowViaKWin(windowId, absoluteX, absoluteY, width, height, maximized);
}

QVariantMap WindowPositioner::getMonitorConfiguration()
{
    QVariantMap config;
    QVariantList monitors;

    QList<QScreen*> screens = QGuiApplication::screens();
    config["count"] = screens.size();

    for (int i = 0; i < screens.size(); ++i) {
        QScreen* screen = screens.at(i);
        QVariantMap monitor;
        monitor["index"] = i;
        monitor["name"] = screen->name();
        monitor["x"] = screen->geometry().x();
        monitor["y"] = screen->geometry().y();
        monitor["width"] = screen->geometry().width();
        monitor["height"] = screen->geometry().height();
        
        QVariantMap resolution;
        resolution["width"] = screen->geometry().width();
        resolution["height"] = screen->geometry().height();
        monitor["resolution"] = resolution;

        monitors.append(monitor);
    }

    config["monitors"] = monitors;
    return config;
}

QString WindowPositioner::findWindow(const QString& applicationName)
{
    // Use KWin scripting API to find window
    // This is a simplified implementation - actual KWin API would be more complex
    
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        qWarning() << "Cannot connect to KWin D-Bus interface";
        return QString();
    }

    // Call KWin API to list windows and find matching application
    // Note: Actual KWin API methods may differ
    QDBusReply<QStringList> reply = kwinInterface.call("getClientList");
    if (reply.isValid()) {
        QStringList windows = reply.value();
        for (const QString& window : windows) {
            // Get window properties to match application name
            // This is simplified - actual implementation would check window class/name
            if (window.contains(applicationName, Qt::CaseInsensitive)) {
                return window;
            }
        }
    }

    return QString();
}

QVariantMap WindowPositioner::getMonitorGeometry(int monitorIndex)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    
    if (monitorIndex < 0 || monitorIndex >= screens.size()) {
        return QVariantMap();
    }

    QScreen* screen = screens.at(monitorIndex);
    QVariantMap geometry;
    geometry["x"] = screen->geometry().x();
    geometry["y"] = screen->geometry().y();
    geometry["width"] = screen->geometry().width();
    geometry["height"] = screen->geometry().height();
    
    return geometry;
}

bool WindowPositioner::positionWindowViaKWin(const QString& windowId, int x, int y,
                                            int width, int height, bool maximized)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        qWarning() << "Cannot connect to KWin D-Bus interface";
        return false;
    }

    // Set window geometry
    // Note: Actual KWin API methods may differ - this is a simplified implementation
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "setClientGeometry"
    );
    message << windowId << x << y << width << height;

    QDBusReply<void> reply = connection.call(message);
    if (!reply.isValid()) {
        qWarning() << "Failed to position window:" << reply.error().message();
        return false;
    }

    // Set maximized state if needed
    if (maximized) {
        QDBusMessage maximizeMessage = QDBusMessage::createMethodCall(
            "org.kde.KWin",
            "/KWin",
            "org.kde.KWin",
            "setClientMaximized"
        );
        maximizeMessage << windowId << true;
        connection.call(maximizeMessage);
    }

    return true;
}

