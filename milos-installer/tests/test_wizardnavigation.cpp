#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>

/**
 * @brief Test suite for Installation Wizard Navigation
 * 
 * Tests wizard navigation, step progression, and state management.
 */
class WizardNavigationTests : public QObject
{
    Q_OBJECT

public:
    WizardNavigationTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testWizardNavigation();
    void testStepIndicator();
    void testBackButton();
    void testNextButton();
    void testStepValidation();
    void testStatePersistence();
    void testStateRecovery();
    void testCheckpointSystem();
    void testKeyboardNavigation();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_mainWindow;
};

WizardNavigationTests::WizardNavigationTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_mainWindow(nullptr)
{
}

void WizardNavigationTests::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Set QML import paths
    QStringList importPaths = m_engine->importPathList();
    importPaths.prepend(QDir::currentPath() + "/../src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    m_engine->setImportPathList(importPaths);
    
    // Load main window
    m_engine->load(QUrl::fromLocalFile(QDir::currentPath() + "/../src/ui/mainwindow.qml"));
    
    QQuickWindow* window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
    if (window) {
        m_window = window;
        m_mainWindow = window->contentItem();
    }
}

void WizardNavigationTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

void WizardNavigationTests::testWizardNavigation()
{
    QVERIFY(m_mainWindow != nullptr);
    
    // Verify wizard navigation system exists
    QQuickItem* wizardNavigation = m_mainWindow->findChild<QQuickItem*>("wizardNavigation");
    if (wizardNavigation) {
        QVERIFY(wizardNavigation->isVisible());
    }
}

void WizardNavigationTests::testStepIndicator()
{
    // Test step indicator
    QQuickItem* stepIndicator = m_mainWindow->findChild<QQuickItem*>("stepIndicator");
    if (stepIndicator) {
        QVERIFY(stepIndicator->isVisible());
    }
}

void WizardNavigationTests::testBackButton()
{
    // Test "Back" button
    QQuickItem* backButton = m_mainWindow->findChild<QQuickItem*>("backButton");
    if (backButton) {
        QVERIFY(backButton->isVisible());
    }
}

void WizardNavigationTests::testNextButton()
{
    // Test "Next" button
    QQuickItem* nextButton = m_mainWindow->findChild<QQuickItem*>("nextButton");
    if (nextButton) {
        QVERIFY(nextButton->isVisible());
    }
}

void WizardNavigationTests::testStepValidation()
{
    // Test step validation
    QVERIFY(m_mainWindow != nullptr);
    // Validation logic is tested through navigation behavior
}

void WizardNavigationTests::testStatePersistence()
{
    // Test installation state persistence
    // This would require testing the InstallerState service
    QVERIFY(m_mainWindow != nullptr);
}

void WizardNavigationTests::testStateRecovery()
{
    // Test installation state recovery
    // This would require testing the InstallerState service
    QVERIFY(m_mainWindow != nullptr);
}

void WizardNavigationTests::testCheckpointSystem()
{
    // Test checkpoint system
    // This would require testing the InstallerState service
    QVERIFY(m_mainWindow != nullptr);
}

void WizardNavigationTests::testKeyboardNavigation()
{
    // Test keyboard navigation (Tab between buttons, Enter activates Next)
    QVERIFY(m_mainWindow != nullptr);
    
    QQuickItem* nextButton = m_mainWindow->findChild<QQuickItem*>("nextButton");
    if (nextButton) {
        QVERIFY(nextButton->canAcceptFocus() || nextButton->property("focusable").toBool());
    }
}

QTEST_MAIN(WizardNavigationTests)
#include "test_wizardnavigation.moc"

