#include <QtQml/qqml.h>
#include "alertzoneservice.h"
#include "alertzonewidget.h"

void registerAlertZoneTypes()
{
    qmlRegisterType<AlertZoneService>("AlertZone", 1, 0, "AlertZoneService");
    qmlRegisterType<AlertZoneWidget>("AlertZone", 1, 0, "AlertZoneWidget");
}

Q_COREAPP_STARTUP_FUNCTION(registerAlertZoneTypes)

