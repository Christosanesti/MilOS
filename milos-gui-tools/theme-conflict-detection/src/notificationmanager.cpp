#include "notificationmanager.h"
#include "compatibilitydatabase.h"
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
{
}

void NotificationManager::showFallbackNotification(const QString &reason)
{
    QString title = "Fallback Styling Activated";
    QString message = "Xenon effects cannot render with the current theme configuration.\n\n";
    message += "Reason: " + reason + "\n\n";
    message += getFixInstructions(reason);
    
    emit notificationRequested(title, message, "warning");
    
    // Also try to show via system notification
    QDBusInterface notificationInterface("org.freedesktop.Notifications",
                                         "/org/freedesktop/Notifications",
                                         "org.freedesktop.Notifications",
                                         QDBusConnection::sessionBus());
    
    if (notificationInterface.isValid()) {
        QVariantMap hints;
        hints["urgency"] = 1; // Normal urgency
        hints["category"] = "system";
        
        notificationInterface.call("Notify",
                                  "MilOS Theme Manager",
                                  0,
                                  "dialog-warning",
                                  title,
                                  message,
                                  QStringList(),
                                  hints,
                                  10000); // 10 second timeout
    }
}

void NotificationManager::showCompatibilityNotification(const QString &themeName, const QString &status)
{
    QString title = "Theme Compatibility Update";
    QString message = QString("Theme '%1' compatibility status: %2").arg(themeName).arg(status);
    
    emit notificationRequested(title, message, "information");
}

QString NotificationManager::getFixInstructions(const QString &reason)
{
    QString instructions = "To fix this issue:\n";
    instructions += "1. Switch to a compatible theme\n";
    
    QStringList compatibleThemes = getCompatibleThemes();
    if (!compatibleThemes.isEmpty()) {
        instructions += "\nCompatible themes:\n";
        for (const QString &theme : compatibleThemes) {
            instructions += "  • " + theme + "\n";
        }
    }
    
    instructions += "\n2. Or update your graphics drivers\n";
    instructions += "3. Or disable xenon effects in System Settings";
    
    return instructions;
}

QStringList NotificationManager::getCompatibleThemes()
{
    CompatibilityDatabase db;
    db.loadDatabase();
    
    QVariantList compatible = db.getCompatibleThemes();
    QStringList themes;
    
    for (const QVariant &item : compatible) {
        QVariantMap theme = item.toMap();
        themes.append(theme.value("name").toString());
    }
    
    return themes;
}

