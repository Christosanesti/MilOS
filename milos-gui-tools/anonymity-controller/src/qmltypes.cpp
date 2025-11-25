#include <QtQml>
#include "anonymitymanager.h"

void registerAnonymityControllerTypes()
{
    qmlRegisterType<AnonymityManager>("org.milos.AnonymityController", 1, 0, "AnonymityManager");
}

// Manual registration (not using Q_COREAPP_STARTUP_FUNCTION for better control)

