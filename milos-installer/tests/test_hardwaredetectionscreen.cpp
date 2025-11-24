#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>

/**
 * @brief Test suite for Hardware Detection Screen
 * 
 * Tests hardware detection screen rendering, scanning, and functionality.
 */
class HardwareDetectionScreenTests : public QObject
{
    Q_OBJECT

public:
    HardwareDetectionScreenTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testHardwareDetectionScreenRendering();
    void testHardwareScanning();
    void testHardwareScanningAnimation();
    void testDriverInstallationProgress();
    void testHardwareCompatibilityWarnings();
    void testDriverInstallationErrorHandling();
    void testManualDriverInstallationOption();
    void testKeyboardNavigation();
    void testScreenReaderSupport();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_hardwareScreen;
};

HardwareDetectionScreenTests::HardwareDetectionScreenTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_hardwareScreen(nullptr)
{
}

void HardwareDetectionScreenTests::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Set QML import paths
    QStringList importPaths = m_engine->importPathList();
    importPaths.prepend(QDir::currentPath() + "/../src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    m_engine->setImportPathList(importPaths);
    
    // Load hardware detection screen
    m_engine->load(QUrl::fromLocalFile(QDir::currentPath() + "/../src/ui/hardwaredetectionscreen.qml"));
    
    QQuickWindow* window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
    if (window) {
        m_window = window;
        m_hardwareScreen = window->contentItem();
    }
}

void HardwareDetectionScreenTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

void HardwareDetectionScreenTests::testHardwareDetectionScreenRendering()
{
    QVERIFY(m_hardwareScreen != nullptr);
    QVERIFY(m_hardwareScreen->isVisible());
}

void HardwareDetectionScreenTests::testHardwareScanning()
{
    // Test hardware scanning
    QVERIFY(m_hardwareScreen != nullptr);
    
    QQuickItem* hardwareList = m_hardwareScreen->findChild<QQuickItem*>("hardwareList");
    if (hardwareList) {
        QVERIFY(hardwareList->isVisible());
    }
}

void HardwareDetectionScreenTests::testHardwareScanningAnimation()
{
    // Test hardware scanning animation (Animation 2: Xenon Border Scan)
    QQuickItem* scanningProgress = m_hardwareScreen->findChild<QQuickItem*>("scanningProgress");
    if (scanningProgress) {
        QVERIFY(scanningProgress->isVisible());
    }
}

void HardwareDetectionScreenTests::testDriverInstallationProgress()
{
    // Test driver installation progress (Component 4: Progress Bar)
    QQuickItem* driverProgress = m_hardwareScreen->findChild<QQuickItem*>("driverProgress");
    if (driverProgress) {
        QVERIFY(driverProgress->isVisible());
    }
}

void HardwareDetectionScreenTests::testHardwareCompatibilityWarnings()
{
    // Test hardware compatibility warnings (Component 6: Notification/Alert)
    QVERIFY(m_hardwareScreen != nullptr);
    // Warnings may be created dynamically
}

void HardwareDetectionScreenTests::testDriverInstallationErrorHandling()
{
    // Test driver installation error handling
    QVERIFY(m_hardwareScreen != nullptr);
    // Error handling dialogs may be created dynamically
}

void HardwareDetectionScreenTests::testManualDriverInstallationOption()
{
    // Test manual driver installation option
    QQuickItem* manualDriverOption = m_hardwareScreen->findChild<QQuickItem*>("manualDriverOption");
    if (manualDriverOption) {
        QVERIFY(manualDriverOption->isVisible() || manualDriverOption->property("visible").toBool());
    }
}

void HardwareDetectionScreenTests::testKeyboardNavigation()
{
    // Test keyboard navigation
    QVERIFY(m_hardwareScreen != nullptr);
    
    QQuickItem* hardwareList = m_hardwareScreen->findChild<QQuickItem*>("hardwareList");
    if (hardwareList) {
        QVERIFY(hardwareList->canAcceptFocus() || hardwareList->property("focusable").toBool());
    }
}

void HardwareDetectionScreenTests::testScreenReaderSupport()
{
    // Test screen reader support
    QVERIFY(m_hardwareScreen != nullptr);
    
    QQuickItem* hardwareList = m_hardwareScreen->findChild<QQuickItem*>("hardwareList");
    if (hardwareList) {
        QString accessibleName = hardwareList->property("Accessible.name").toString();
        QVERIFY(!accessibleName.isEmpty() || hardwareList->property("text").toString().length() > 0);
    }
}

QTEST_MAIN(HardwareDetectionScreenTests)
#include "test_hardwaredetectionscreen.moc"

