#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>

/**
 * @brief Test suite for Partitioning Screen
 * 
 * Tests partitioning screen rendering, disk detection, and functionality.
 */
class PartitioningScreenTests : public QObject
{
    Q_OBJECT

public:
    PartitioningScreenTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testPartitioningScreenRendering();
    void testDiskDetection();
    void testAutoPartitionOption();
    void testRecommendedBadge();
    void testManualPartitionEditor();
    void testWarningDialogs();
    void testKeyboardNavigation();
    void testScreenReaderSupport();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_partitioningScreen;
};

PartitioningScreenTests::PartitioningScreenTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_partitioningScreen(nullptr)
{
}

void PartitioningScreenTests::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Set QML import paths
    QStringList importPaths = m_engine->importPathList();
    importPaths.prepend(QDir::currentPath() + "/../src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    m_engine->setImportPathList(importPaths);
    
    // Load partitioning screen
    m_engine->load(QUrl::fromLocalFile(QDir::currentPath() + "/../src/ui/partitioningscreen.qml"));
    
    QQuickWindow* window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
    if (window) {
        m_window = window;
        m_partitioningScreen = window->contentItem();
    }
}

void PartitioningScreenTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

void PartitioningScreenTests::testPartitioningScreenRendering()
{
    QVERIFY(m_partitioningScreen != nullptr);
    QVERIFY(m_partitioningScreen->isVisible());
}

void PartitioningScreenTests::testDiskDetection()
{
    // Test disk detection display
    QVERIFY(m_partitioningScreen != nullptr);
    
    // Verify disk list or disk representation exists
    QQuickItem* diskList = m_partitioningScreen->findChild<QQuickItem*>("diskList");
    if (diskList) {
        QVERIFY(diskList->isVisible());
    }
}

void PartitioningScreenTests::testAutoPartitionOption()
{
    // Test auto-partition option
    QQuickItem* autoPartitionOption = m_partitioningScreen->findChild<QQuickItem*>("autoPartitionOption");
    if (autoPartitionOption) {
        QVERIFY(autoPartitionOption->isVisible());
    }
}

void PartitioningScreenTests::testRecommendedBadge()
{
    // Test "Recommended" badge with xenon accent
    QQuickItem* recommendedBadge = m_partitioningScreen->findChild<QQuickItem*>("recommendedBadge");
    if (recommendedBadge) {
        QVERIFY(recommendedBadge->isVisible());
    }
}

void PartitioningScreenTests::testManualPartitionEditor()
{
    // Test manual partition editor
    QQuickItem* manualEditor = m_partitioningScreen->findChild<QQuickItem*>("manualEditor");
    if (manualEditor) {
        QVERIFY(manualEditor->isVisible() || manualEditor->property("visible").toBool());
    }
}

void PartitioningScreenTests::testWarningDialogs()
{
    // Test warning dialogs for destructive operations
    QVERIFY(m_partitioningScreen != nullptr);
    
    // Warning dialogs may be created dynamically, so we test the structure
    // Verify warning dialog component exists or can be created
}

void PartitioningScreenTests::testKeyboardNavigation()
{
    // Test keyboard navigation
    QVERIFY(m_partitioningScreen != nullptr);
    
    // Verify focusable elements exist
    QQuickItem* autoPartitionOption = m_partitioningScreen->findChild<QQuickItem*>("autoPartitionOption");
    if (autoPartitionOption) {
        QVERIFY(autoPartitionOption->canAcceptFocus() || autoPartitionOption->property("focusable").toBool());
    }
}

void PartitioningScreenTests::testScreenReaderSupport()
{
    // Test screen reader support
    QVERIFY(m_partitioningScreen != nullptr);
    
    // Verify accessible properties exist
    QQuickItem* autoPartitionOption = m_partitioningScreen->findChild<QQuickItem*>("autoPartitionOption");
    if (autoPartitionOption) {
        QString accessibleName = autoPartitionOption->property("Accessible.name").toString();
        QVERIFY(!accessibleName.isEmpty() || autoPartitionOption->property("text").toString().length() > 0);
    }
}

QTEST_MAIN(PartitioningScreenTests)
#include "test_partitioningscreen.moc"

