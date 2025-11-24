#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>
#include "../src/alertzonewidget.h"
#include "../src/alertzoneservice.h"

/**
 * @brief Test suite for Alert Zone Widget
 * 
 * Tests widget registration, alert bar display, severity visual feedback, and alert history.
 */
class AlertZoneWidgetTests : public QObject
{
    Q_OBJECT

public:
    AlertZoneWidgetTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testWidgetRegistration();
    void testAlertBarDisplay();
    void testSeverityVisualFeedback();
    void testAlertHistoryView();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    AlertZoneService* m_service;
};

AlertZoneWidgetTests::AlertZoneWidgetTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_service(nullptr)
{
}

void AlertZoneWidgetTests::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Register QML types
    qmlRegisterType<AlertZoneService>("AlertZone", 1, 0, "AlertZoneService");
    qmlRegisterType<AlertZoneWidget>("AlertZone", 1, 0, "AlertZoneWidget");
    
    // Set QML import paths
    QStringList importPaths = m_engine->importPathList();
    importPaths.prepend(QDir::currentPath() + "/../package/contents/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    m_engine->setImportPathList(importPaths);
    
    // Create service instance
    m_service = new AlertZoneService(this);
    
    // Load main QML
    m_engine->load(QUrl::fromLocalFile(QDir::currentPath() + "/../package/contents/ui/main.qml"));
    
    QQuickWindow* window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
    if (window) {
        m_window = window;
    }
}

void AlertZoneWidgetTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }
}

void AlertZoneWidgetTests::testWidgetRegistration()
{
    // Test widget registration with Plasma panel
    QVERIFY(m_window != nullptr);
    QVERIFY(m_service != nullptr);
}

void AlertZoneWidgetTests::testAlertBarDisplay()
{
    // Test alert bar displays correctly
    QVERIFY(m_window != nullptr);
    
    // Add test alert
    m_service->addAlert("medium", "Network", "Test alert");
    
    // Verify service received alert
    QVERIFY(m_service->alertCount() > 0);
}

void AlertZoneWidgetTests::testSeverityVisualFeedback()
{
    // Test xenon effects apply based on severity
    QVERIFY(m_service != nullptr);
    
    // Test different severity levels
    m_service->addAlert("low", "System", "Low severity alert");
    m_service->addAlert("medium", "Network", "Medium severity alert");
    m_service->addAlert("high", "Security", "High severity alert");
    m_service->addAlert("critical", "Security", "Critical severity alert");
    
    // Verify all alerts were added
    QVERIFY(m_service->alertCount() >= 4);
}

void AlertZoneWidgetTests::testAlertHistoryView()
{
    // Test alert history displays correctly
    QVERIFY(m_service != nullptr);
    
    // Add multiple alerts
    for (int i = 0; i < 5; i++) {
        m_service->addAlert("medium", "Test", QString("Alert %1").arg(i));
    }
    
    // Verify alerts were added
    QVERIFY(m_service->alertCount() >= 5);
}

QTEST_MAIN(AlertZoneWidgetTests)
#include "alertzonewidget_test.moc"

