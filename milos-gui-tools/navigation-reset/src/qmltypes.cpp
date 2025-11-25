#include <QtQml>
#include "resetui.h"

void registerNavigationResetTypes()
{
    qmlRegisterType<NavigationResetUI>("org.milos.NavigationReset", 1, 0, "NavigationResetUI");
}

Q_COREAPP_STARTUP_FUNCTION(registerNavigationResetTypes)

