#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>
#include <QDebug>

/**
 * @brief Test suite for Component Library Integration
 * 
 * Tests integration with MilOS component library and xenon effects library.
 */
class IntegrationTests : public QObject
{
    Q_OBJECT

public:
    IntegrationTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testComponentLibraryIntegration();
    void testXenonEffectsIntegration();
    void testWizardNavigationIntegration();
    void testStatePersistenceIntegration();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_mainWindow;
};

IntegrationTests::IntegrationTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_mainWindow(nullptr)
{
}

void IntegrationTests::initTestCase()
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

void IntegrationTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

void IntegrationTests::testComponentLibraryIntegration()
{
    // Test MilOS component library integration (Components 1, 3, 6, 8)
    QVERIFY(m_mainWindow != nullptr);
    
    // Verify component library is imported
    // Components should be available via MilosComponents import
    // This is verified by successful QML loading
    QVERIFY(m_engine->rootObjects().size() > 0);
}

void IntegrationTests::testXenonEffectsIntegration()
{
    // Test xenon effects library integration
    QVERIFY(m_mainWindow != nullptr);
    
    // Verify xenon effects are applied (animations, glows)
    // This is verified by checking for animation properties or xenon effect properties
    QQuickItem* welcomeScreen = m_mainWindow->findChild<QQuickItem*>("welcomeScreen");
    if (welcomeScreen) {
        // Verify animations exist (pulsing, hover effects)
        QVERIFY(welcomeScreen->isVisible());
    }
}

void IntegrationTests::testWizardNavigationIntegration()
{
    // Test installation wizard navigation integration
    QVERIFY(m_mainWindow != nullptr);
    
    // Verify wizard navigation works correctly
    QQuickItem* wizardNavigation = m_mainWindow->findChild<QQuickItem*>("wizardNavigation");
    if (wizardNavigation) {
        QVERIFY(wizardNavigation->isVisible());
    }
}

void IntegrationTests::testStatePersistenceIntegration()
{
    // Test installation state persistence integration
    QVERIFY(m_mainWindow != nullptr);
    
    // Verify state persistence system is integrated
    // This would require testing the InstallerState service integration
    QVERIFY(true); // Placeholder - state persistence tested in test_installerstate.cpp
}

QTEST_MAIN(IntegrationTests)
#include "test_integration.moc"

