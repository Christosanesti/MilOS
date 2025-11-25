#include "windowgrouper.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

WindowGrouper::WindowGrouper(QObject* parent)
    : QObject(parent)
{
}

bool WindowGrouper::groupWindows(const QVariantMap& template)
{
    // Check if template has window grouping information
    // This would be in a "window_groups" field in the template
    if (!template.contains("window_groups")) {
        // No grouping specified, return success
        return true;
    }

    QVariantList windowGroups = template.value("window_groups").toList();
    bool allSuccess = true;

    for (const QVariant& groupVar : windowGroups) {
        QVariantMap group = groupVar.toMap();
        QString groupId = group.value("group_id", QString()).toString();
        QString groupName = group.value("group_name", QString()).toString();
        QVariantList appsVar = group.value("applications").toList();
        QStringList applications;
        
        for (const QVariant& appVar : appsVar) {
            applications.append(appVar.toString());
        }

        QString groupTypeStr = group.value("group_type", "TAB_GROUP").toString();
        GroupType groupType = TAB_GROUP;
        if (groupTypeStr == "TILE_GROUP") {
            groupType = TILE_GROUP;
        } else if (groupTypeStr == "STACK_GROUP") {
            groupType = STACK_GROUP;
        }

        bool success = createGroup(groupId, groupName, applications, groupType);
        emit windowsGrouped(groupId, success);
        
        if (!success) {
            allSuccess = false;
        }
    }

    return allSuccess;
}

bool WindowGrouper::createGroup(const QString& groupId, const QString& groupName,
                                const QStringList& applications, GroupType groupType)
{
    QStringList windowIds = findWindows(applications);
    
    if (windowIds.isEmpty()) {
        emit error(QString("No windows found for group: %1").arg(groupName));
        return false;
    }

    return groupWindowsViaKWin(windowIds, groupType);
}

QStringList WindowGrouper::findWindows(const QStringList& applications)
{
    QStringList windowIds;

    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        qWarning() << "Cannot connect to KWin D-Bus interface";
        return windowIds;
    }

    // Get list of windows from KWin
    QDBusReply<QStringList> reply = kwinInterface.call("getClientList");
    if (!reply.isValid()) {
        qWarning() << "Failed to get window list:" << reply.error().message();
        return windowIds;
    }

    QStringList allWindows = reply.value();
    
    // Match windows to applications
    for (const QString& application : applications) {
        for (const QString& window : allWindows) {
            // Simplified matching - actual implementation would check window class/name
            if (window.contains(application, Qt::CaseInsensitive)) {
                windowIds.append(window);
                break;
            }
        }
    }

    return windowIds;
}

bool WindowGrouper::groupWindowsViaKWin(const QStringList& windowIds, GroupType groupType)
{
    if (windowIds.size() < 2) {
        // Need at least 2 windows to group
        return false;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface kwinInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!kwinInterface.isValid()) {
        qWarning() << "Cannot connect to KWin D-Bus interface";
        return false;
    }

    // Group windows using KWin API
    // Note: Actual KWin API methods may differ - this is a simplified implementation
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "groupWindows"
    );
    
    QVariantList args;
    args << QVariant::fromValue(windowIds);
    args << QVariant::fromValue(static_cast<int>(groupType));
    message.setArguments(args);

    QDBusReply<void> reply = connection.call(message);
    if (!reply.isValid()) {
        qWarning() << "Failed to group windows:" << reply.error().message();
        return false;
    }

    return true;
}

