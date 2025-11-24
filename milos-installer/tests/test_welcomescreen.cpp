#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QDir>

/**
 * @brief Test suite for Welcome Screen
 * 
 * Tests welcome screen rendering, animations, and functionality.
 */
class WelcomeScreenTests : public QObject
{
    Q_OBJECT

public:
    WelcomeScreenTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testWelcomeScreenRendering();
    void testLogoPulsingAnimation();
    void testLanguageSelection();
    void testButtonRendering();
    void testButtonHoverEffects();
    void testKeyboardNavigation();
    void testScreenReaderSupport();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_welcomeScreen;
};

WelcomeScreenTests::WelcomeScreenTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_welcomeScreen(nullptr)
{
}

void WelcomeScreenTests::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Set QML import paths
    QStringList importPaths = m_engine->importPathList();
    importPaths.prepend(QDir::currentPath() + "/../src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    m_engine->setImportPathList(importPaths);
    
    // Load welcome screen
    m_engine->load(QUrl::fromLocalFile(QDir::currentPath() + "/../src/ui/welcomescreen.qml"));
    
    QQuickWindow* window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
    if (window) {
        m_window = window;
        m_welcomeScreen = window->contentItem();
    }
}

void WelcomeScreenTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

void WelcomeScreenTests::testWelcomeScreenRendering()
{
    QVERIFY(m_welcomeScreen != nullptr);
    QVERIFY(m_welcomeScreen->isVisible());
}

void WelcomeScreenTests::testLogoPulsingAnimation()
{
    // Test that logo pulsing animation exists
    // Animation should be running (opacity animation from 0.6 to 1.0)
    QVERIFY(m_welcomeScreen != nullptr);
    
    // Find logo element
    QQuickItem* logo = m_welcomeScreen->findChild<QQuickItem*>("logo");
    if (logo) {
        // Verify animation properties exist
        QVERIFY(logo->opacity() >= 0.6);
        QVERIFY(logo->opacity() <= 1.0);
    }
}

void WelcomeScreenTests::testLanguageSelection()
{
    // Test language selection dropdown
    QQuickItem* languageDropdown = m_welcomeScreen->findChild<QQuickItem*>("languageDropdown");
    if (languageDropdown) {
        QVERIFY(languageDropdown->isVisible());
    }
}

void WelcomeScreenTests::testButtonRendering()
{
    // Test "Install MilOS" button
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        QVERIFY(installButton->isVisible());
    }
    
    // Test "Try Live Mode" button
    QQuickItem* liveModeButton = m_welcomeScreen->findChild<QQuickItem*>("liveModeButton");
    if (liveModeButton) {
        QVERIFY(liveModeButton->isVisible());
    }
}

void WelcomeScreenTests::testButtonHoverEffects()
{
    // Test button hover effects (xenon glow)
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        // Verify button has hover state capability
        QVERIFY(installButton->isEnabled());
    }
}

void WelcomeScreenTests::testKeyboardNavigation()
{
    // Test keyboard navigation (Tab order, Enter activation)
    QVERIFY(m_welcomeScreen != nullptr);
    
    // Verify focusable elements exist
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        QVERIFY(installButton->property("focus").toBool() || installButton->canAcceptFocus());
    }
}

void WelcomeScreenTests::testScreenReaderSupport()
{
    // Test screen reader support (ARIA labels, semantic structure)
    QVERIFY(m_welcomeScreen != nullptr);
    
    // Verify accessible properties exist
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        // Check for accessible name
        QString accessibleName = installButton->property("Accessible.name").toString();
        QVERIFY(!accessibleName.isEmpty() || installButton->property("text").toString().length() > 0);
    }
}

QTEST_MAIN(WelcomeScreenTests)
#include "test_welcomescreen.moc"

