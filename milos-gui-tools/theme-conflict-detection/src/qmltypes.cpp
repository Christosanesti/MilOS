#include <QtQml>
#include "notificationmanager.h"
#include "compatibilitydatabase.h"
#include "themeconflictdetector.h"
#include "conflictdetectionrules.h"

void registerThemeConflictTypes()
{
    qmlRegisterType<NotificationManager>("org.milos.ThemeConflict", 1, 0, "NotificationManager");
    qmlRegisterType<CompatibilityDatabase>("org.milos.ThemeConflict", 1, 0, "CompatibilityDatabase");
    qmlRegisterType<ThemeConflictDetector>("org.milos.ThemeConflict", 1, 0, "ThemeConflictDetector");
    qmlRegisterType<ConflictDetectionRules>("org.milos.ThemeConflict", 1, 0, "ConflictDetectionRules");
}

Q_COREAPP_STARTUP_FUNCTION(registerThemeConflictTypes)

