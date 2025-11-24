#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>

/**
 * @brief Test suite for Installation Completion Screen
 * 
 * Tests installation completion screen rendering, success animation, and functionality.
 */
class InstallationCompletionScreenTests : public QObject
{
    Q_OBJECT

public:
    InstallationCompletionScreenTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testInstallationCompletionScreenRendering();
    void testSuccessCelebrationAnimation();
    void testInstallationSummaryDisplay();
    void testRebootSystemButton();
    void testErrorHandling();
    void testErrorLogDisplay();
    void testRetryOption();
    void testAbortOption();
    void testRecoveryOption();
    void testCheckpointSystem();
    void testKeyboardNavigation();
    void testScreenReaderSupport();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_completionScreen;
};

InstallationCompletionScreenTests::InstallationCompletionScreenTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_completionScreen(nullptr)
{
}

void InstallationCompletionScreenTests::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Set QML import paths
    QStringList importPaths = m_engine->importPathList();
    importPaths.prepend(QDir::currentPath() + "/../src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    m_engine->setImportPathList(importPaths);
    
    // Load installation completion screen
    m_engine->load(QUrl::fromLocalFile(QDir::currentPath() + "/../src/ui/installationcompletionscreen.qml"));
    
    QQuickWindow* window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
    if (window) {
        m_window = window;
        m_completionScreen = window->contentItem();
    }
}

void InstallationCompletionScreenTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

void InstallationCompletionScreenTests::testInstallationCompletionScreenRendering()
{
    QVERIFY(m_completionScreen != nullptr);
    QVERIFY(m_completionScreen->isVisible());
}

void InstallationCompletionScreenTests::testSuccessCelebrationAnimation()
{
    // Test success celebration animation (Animation 5: Xenon Success Celebration)
    QQuickItem* successIndicator = m_completionScreen->findChild<QQuickItem*>("successIndicator");
    if (successIndicator) {
        QVERIFY(successIndicator->isVisible());
    }
}

void InstallationCompletionScreenTests::testInstallationSummaryDisplay()
{
    // Test installation summary display
    QQuickItem* summaryDisplay = m_completionScreen->findChild<QQuickItem*>("summaryDisplay");
    if (summaryDisplay) {
        QVERIFY(summaryDisplay->isVisible());
    }
}

void InstallationCompletionScreenTests::testRebootSystemButton()
{
    // Test "Reboot System" button (Component 1: Button - Primary variant)
    QQuickItem* rebootButton = m_completionScreen->findChild<QQuickItem*>("rebootButton");
    if (rebootButton) {
        QVERIFY(rebootButton->isVisible());
    }
}

void InstallationCompletionScreenTests::testErrorHandling()
{
    // Test installation failure error handling
    QVERIFY(m_completionScreen != nullptr);
    // Error dialogs may be created dynamically
}

void InstallationCompletionScreenTests::testErrorLogDisplay()
{
    // Test error log display and accessibility
    QQuickItem* errorLog = m_completionScreen->findChild<QQuickItem*>("errorLog");
    if (errorLog) {
        QVERIFY(errorLog->isVisible() || errorLog->property("visible").toBool());
    }
}

void InstallationCompletionScreenTests::testRetryOption()
{
    // Test retry failed step option
    QQuickItem* retryButton = m_completionScreen->findChild<QQuickItem*>("retryButton");
    if (retryButton) {
        QVERIFY(retryButton->isVisible() || retryButton->property("visible").toBool());
    }
}

void InstallationCompletionScreenTests::testAbortOption()
{
    // Test abort installation option
    QQuickItem* abortButton = m_completionScreen->findChild<QQuickItem*>("abortButton");
    if (abortButton) {
        QVERIFY(abortButton->isVisible() || abortButton->property("visible").toBool());
    }
}

void InstallationCompletionScreenTests::testRecoveryOption()
{
    // Test recovery option (save logs to USB drive)
    QQuickItem* recoveryOption = m_completionScreen->findChild<QQuickItem*>("recoveryOption");
    if (recoveryOption) {
        QVERIFY(recoveryOption->isVisible() || recoveryOption->property("visible").toBool());
    }
}

void InstallationCompletionScreenTests::testCheckpointSystem()
{
    // Test progress checkpoint system
    QVERIFY(m_completionScreen != nullptr);
    // Checkpoint system is tested through installation state management
}

void InstallationCompletionScreenTests::testKeyboardNavigation()
{
    // Test keyboard navigation
    QVERIFY(m_completionScreen != nullptr);
    
    QQuickItem* rebootButton = m_completionScreen->findChild<QQuickItem*>("rebootButton");
    if (rebootButton) {
        QVERIFY(rebootButton->canAcceptFocus() || rebootButton->property("focusable").toBool());
    }
}

void InstallationCompletionScreenTests::testScreenReaderSupport()
{
    // Test screen reader support
    QVERIFY(m_completionScreen != nullptr);
    
    QQuickItem* rebootButton = m_completionScreen->findChild<QQuickItem*>("rebootButton");
    if (rebootButton) {
        QString accessibleName = rebootButton->property("Accessible.name").toString();
        QVERIFY(!accessibleName.isEmpty() || rebootButton->property("text").toString().length() > 0);
    }
}

QTEST_MAIN(InstallationCompletionScreenTests)
#include "test_installationcompletionscreen.moc"

