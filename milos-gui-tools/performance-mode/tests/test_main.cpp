#include <QtTest>
#include "styling.h"
#include "control.h"
#include "indicators.h"

class PerformanceModeTest : public QObject
{
    Q_OBJECT

private slots:
    void testStyling();
    void testControl();
    void testIndicators();
};

void PerformanceModeTest::testStyling()
{
    PerformanceStyling styling;
    
    // Test mode properties
    QCOMPARE(styling.getXenonIntensity("standard"), 1.0);
    QCOMPARE(styling.getXenonIntensity("performance"), 0.5);
    QCOMPARE(styling.getXenonIntensity("low-power"), 0.2);
    
    QCOMPARE(styling.areParticlesEnabled("standard"), true);
    QCOMPARE(styling.areParticlesEnabled("performance"), false);
    QCOMPARE(styling.areParticlesEnabled("low-power"), false);
}

void PerformanceModeTest::testControl()
{
    PerformanceControl control;
    
    // Test mode setting
    control.setCurrentMode("performance");
    QCOMPARE(control.currentMode(), QString("performance"));
    
    // Test application override
    control.setApplicationMode("test-app", "low-power");
    QCOMPARE(control.getApplicationMode("test-app"), QString("low-power"));
    
    // Test override removal
    control.removeApplicationOverride("test-app");
    QCOMPARE(control.getApplicationMode("test-app"), QString("performance"));
}

void PerformanceModeTest::testIndicators()
{
    PerformanceIndicators indicators;
    
    // Test indicator visibility
    indicators.showIndicator();
    QCOMPARE(indicators.indicatorVisible(), true);
    
    indicators.hideIndicator();
    QCOMPARE(indicators.indicatorVisible(), false);
    
    // Test mode update
    indicators.updateMode("performance");
    QCOMPARE(indicators.currentMode(), QString("performance"));
}

QTEST_MAIN(PerformanceModeTest)
#include "test_main.moc"

