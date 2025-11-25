#include "conflictnotifier.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QStandardPaths>
#include "compatibilitydatabase.h"

ConflictNotifier::ConflictNotifier(QObject* parent)
    : QObject(parent)
{
}

void ConflictNotifier::showConflictNotification(const QString& themeId, 
                                               const QVariantList& conflicts,
                                               ConflictDetectionRules::SeverityLevel severity)
{
    QString title = "Theme Conflict Detected";
    QString message = QString("Theme '%1' has conflicts with MilOS xenon effects.\n\n").arg(themeId);

    // Add conflict details
    for (const QVariant& conflictVar : conflicts) {
        QVariantMap conflict = conflictVar.toMap();
        message += QString("• %1\n").arg(conflict.value("description").toString());
    }

    // Generate recommendations
    QStringList recommendations = generateRecommendations(conflicts);
    if (!recommendations.isEmpty()) {
        message += "\nRecommendations:\n";
        for (const QString& rec : recommendations) {
            message += QString("• %1\n").arg(rec);
        }
    }

    // Determine urgency based on severity
    int urgency = 1; // Normal
    if (severity == ConflictDetectionRules::Critical || severity == ConflictDetectionRules::High) {
        urgency = 2; // Critical
    }

    // Generate actions
    QStringList actions;
    actions << "switch_theme" << "Switch to Compatible Theme";
    actions << "ignore" << "Ignore";
    
    if (isCriticalConflict(conflicts)) {
        // Critical conflicts require acknowledgment
        actions << "acknowledge" << "Acknowledge";
    }

    sendKdeNotification(title, message, actions, urgency);
}

void ConflictNotifier::showNotificationWithRecommendations(const QString& themeId,
                                                            const QVariantList& conflicts,
                                                            const QStringList& recommendations)
{
    QString title = "Theme Conflict - Action Required";
    QString message = QString("Theme '%1' conflicts detected:\n\n").arg(themeId);

    for (const QVariant& conflictVar : conflicts) {
        QVariantMap conflict = conflictVar.toMap();
        message += QString("• %1 (%2)\n")
                      .arg(conflict.value("description").toString())
                      .arg(conflict.value("impact").toString());
    }

    if (!recommendations.isEmpty()) {
        message += "\nRecommended actions:\n";
        for (const QString& rec : recommendations) {
            message += QString("• %1\n").arg(rec);
        }
    }

    QStringList actions;
    actions << "switch_theme" << "Switch Theme";
    actions << "auto_switch" << "Auto-Switch to MilOS Default";
    actions << "ignore" << "Ignore (Low Severity Only)";

    int urgency = isCriticalConflict(conflicts) ? 2 : 1;
    sendKdeNotification(title, message, actions, urgency);
}

void ConflictNotifier::sendKdeNotification(const QString& title, const QString& message,
                                          const QStringList& actions, int urgency)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface notificationInterface("org.freedesktop.Notifications",
                                       "/org/freedesktop/Notifications",
                                       "org.freedesktop.Notifications",
                                       connection);

    if (!notificationInterface.isValid()) {
        qWarning() << "Cannot connect to notification service";
        return;
    }

    // Prepare notification
    QVariantMap hints;
    hints["urgency"] = urgency;
    hints["category"] = "device";
    hints["desktop-entry"] = "milos-theme-detector";
    
    // Xenon-accented styling hint
    hints["x-milos-xenon"] = true;

    // Send notification
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        "Notify"
    );

    QList<QVariant> args;
    args << "MilOS Theme Detector"; // app_name
    args << QVariant::fromValue<quint32>(0); // replaces_id
    args << "preferences-desktop-theme"; // app_icon
    args << title; // summary
    args << message; // body
    args << QVariant::fromValue<QStringList>(actions); // actions
    args << QVariant::fromValue<QVariantMap>(hints); // hints
    args << -1; // expire_timeout

    message.setArguments(args);

    QDBusReply<quint32> reply = connection.call(message);
    if (reply.isValid()) {
        quint32 notificationId = reply.value();
        qDebug() << "Notification sent, ID:" << notificationId;
    } else {
        qWarning() << "Failed to send notification:" << reply.error().message();
    }
}

QStringList ConflictNotifier::generateRecommendations(const QVariantList& conflicts)
{
    QStringList recommendations;

    // Check conflict types to generate recommendations
    bool hasXenonConflict = false;
    bool hasPerformanceConflict = false;
    bool hasColorConflict = false;

    for (const QVariant& conflictVar : conflicts) {
        QVariantMap conflict = conflictVar.toMap();
        int type = conflict.value("type").toInt();
        
        if (type == ConflictDetectionRules::XenonRendering) {
            hasXenonConflict = true;
        } else if (type == ConflictDetectionRules::Performance) {
            hasPerformanceConflict = true;
        } else if (type == ConflictDetectionRules::ColorScheme) {
            hasColorConflict = true;
        }
    }

    if (hasXenonConflict || hasPerformanceConflict) {
        recommendations << "Switch to MilOS default theme for optimal xenon effects";
        recommendations << "Use a theme with GPU acceleration enabled";
    }

    if (hasColorConflict) {
        recommendations << "Use a lighter color scheme for better xenon effect visibility";
    }

    // Get compatible themes from database
    CompatibilityDatabase db;
    QVariantList compatibleThemes = db.getCompatibleThemes();
    if (!compatibleThemes.isEmpty()) {
        recommendations << QString("Consider using: %1")
                           .arg(compatibleThemes.first().toMap().value("name").toString());
    }

    return recommendations;
}

bool ConflictNotifier::isCriticalConflict(const QVariantList& conflicts)
{
    for (const QVariant& conflictVar : conflicts) {
        QVariantMap conflict = conflictVar.toMap();
        int severity = conflict.value("severity").toInt();
        if (severity == ConflictDetectionRules::Critical) {
            return true;
        }
    }
    return false;
}

