#include <QtQml>
#include "styling.h"
#include "control.h"
#include "indicators.h"

void registerPerformanceModeTypes()
{
    qmlRegisterType<PerformanceStyling>("org.milos.PerformanceMode", 1, 0, "PerformanceStyling");
    qmlRegisterType<PerformanceControl>("org.milos.PerformanceMode", 1, 0, "PerformanceControl");
    qmlRegisterType<PerformanceIndicators>("org.milos.PerformanceMode", 1, 0, "PerformanceIndicators");
}

Q_COREAPP_STARTUP_FUNCTION(registerPerformanceModeTypes)

