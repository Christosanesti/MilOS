#include "layoutrestorer.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QUuid>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

LayoutRestorer::LayoutRestorer(QObject* parent)
    : QObject(parent)
    , m_savedLayoutId("")
{
}

QVariantMap LayoutRestorer::saveCurrentLayout()
{
    QVariantMap layoutData;
    layoutData["layout_id"] = generateLayoutId();
    layoutData["saved_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    layoutData["window_positions"] = captureWindowPositions();

    // Save to file
    QString storagePath = getLayoutStoragePath();
    QDir().mkpath(storagePath);

    QString filePath = storagePath + "/" + layoutData["layout_id"].toString() + ".json";
    QFile file(filePath);
    
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(QJsonObject::fromVariantMap(layoutData));
        file.write(doc.toJson());
        file.close();
        
        m_savedLayoutId = layoutData["layout_id"].toString();
        emit layoutSaved(m_savedLayoutId);
    } else {
        emit error("Failed to save layout to file");
        return QVariantMap();
    }

    return layoutData;
}

bool LayoutRestorer::restoreLayout(const QVariantMap& layoutData)
{
    if (layoutData.isEmpty()) {
        emit error("Layout data is empty");
        return false;
    }

    if (!layoutData.contains("window_positions")) {
        emit error("Layout data does not contain window positions");
        return false;
    }

    bool success = applyWindowPositions(layoutData.value("window_positions").toMap());
    
    if (success) {
        emit layoutRestored(true);
    } else {
        emit layoutRestored(false);
        emit error("Failed to restore window positions");
    }

    return success;
}

bool LayoutRestorer::hasSavedLayout() const
{
    if (m_savedLayoutId.isEmpty()) {
        return false;
    }

    QString storagePath = getLayoutStoragePath();
    QString filePath = storagePath + "/" + m_savedLayoutId + ".json";
    return QFile::exists(filePath);
}

QString LayoutRestorer::getLayoutStoragePath() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/layout-snapshots";
    return configDir;
}

QString LayoutRestorer::generateLayoutId() const
{
    return QString("layout_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8));
}

QVariantMap LayoutRestorer::captureWindowPositions()
{
    QVariantMap positions;

    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        qWarning() << "Cannot connect to KWin D-Bus interface";
        return positions;
    }

    // Get list of windows
    QDBusReply<QStringList> reply = kwinInterface.call("getClientList");
    if (!reply.isValid()) {
        qWarning() << "Failed to get window list:" << reply.error().message();
        return positions;
    }

    QStringList windows = reply.value();
    
    // Capture position for each window
    for (const QString& windowId : windows) {
        // Get window geometry from KWin
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.kde.KWin",
            "/KWin",
            "org.kde.KWin",
            "getClientGeometry"
        );
        message << windowId;

        QDBusReply<QVariantList> geometryReply = connection.call(message);
        if (geometryReply.isValid()) {
            QVariantList geometry = geometryReply.value();
            if (geometry.size() >= 4) {
                QVariantMap windowPos;
                windowPos["window_id"] = windowId;
                windowPos["x"] = geometry.at(0);
                windowPos["y"] = geometry.at(1);
                windowPos["width"] = geometry.at(2);
                windowPos["height"] = geometry.at(3);
                
                // Get maximized state
                QDBusMessage maxMessage = QDBusMessage::createMethodCall(
                    "org.kde.KWin",
                    "/KWin",
                    "org.kde.KWin",
                    "isClientMaximized"
                );
                maxMessage << windowId;
                QDBusReply<bool> maxReply = connection.call(maxMessage);
                if (maxReply.isValid()) {
                    windowPos["maximized"] = maxReply.value();
                }

                positions[windowId] = windowPos;
            }
        }
    }

    return positions;
}

bool LayoutRestorer::applyWindowPositions(const QVariantMap& positions)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        qWarning() << "Cannot connect to KWin D-Bus interface";
        return false;
    }

    bool allSuccess = true;

    for (auto it = positions.begin(); it != positions.end(); ++it) {
        QString windowId = it.key();
        QVariantMap windowPos = it.value().toMap();

        int x = windowPos.value("x", 0).toInt();
        int y = windowPos.value("y", 0).toInt();
        int width = windowPos.value("width", 800).toInt();
        int height = windowPos.value("height", 600).toInt();
        bool maximized = windowPos.value("maximized", false).toBool();

        // Set window geometry
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.kde.KWin",
            "/KWin",
            "org.kde.KWin",
            "setClientGeometry"
        );
        message << windowId << x << y << width << height;

        QDBusReply<void> reply = connection.call(message);
        if (!reply.isValid()) {
            qWarning() << "Failed to restore window position:" << reply.error().message();
            allSuccess = false;
            continue;
        }

        // Set maximized state if needed
        if (maximized) {
            QDBusMessage maxMessage = QDBusMessage::createMethodCall(
                "org.kde.KWin",
                "/KWin",
                "org.kde.KWin",
                "setClientMaximized"
            );
            maxMessage << windowId << true;
            connection.call(maxMessage);
        }
    }

    return allSuccess;
}

