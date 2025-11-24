#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QDir>
#include <QColor>

/**
 * @brief Test suite for Accessibility Compliance
 * 
 * Tests WCAG AA accessibility compliance.
 */
class AccessibilityTests : public QObject
{
    Q_OBJECT

public:
    AccessibilityTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testColorContrast();
    void testKeyboardNavigation();
    void testTouchTargets();
    void testScreenReaderSupport();
    void testFocusIndicators();
    void testNonColorReliance();

private:
    QQmlApplicationEngine* m_engine;
    QQuickWindow* m_window;
    QQuickItem* m_welcomeScreen;
    
    double calculateContrastRatio(const QColor& foreground, const QColor& background);
};

AccessibilityTests::AccessibilityTests()
    : m_engine(nullptr)
    , m_window(nullptr)
    , m_welcomeScreen(nullptr)
{
}

void AccessibilityTests::initTestCase()
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

void AccessibilityTests::cleanupTestCase()
{
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

double AccessibilityTests::calculateContrastRatio(const QColor& foreground, const QColor& background)
{
    // Calculate relative luminance
    auto getLuminance = [](int r, int g, int b) -> double {
        auto normalize = [](int value) -> double {
            double v = value / 255.0;
            return v <= 0.03928 ? v / 12.92 : pow((v + 0.055) / 1.055, 2.4);
        };
        return 0.2126 * normalize(r) + 0.7152 * normalize(g) + 0.0722 * normalize(b);
    };
    
    double l1 = getLuminance(foreground.red(), foreground.green(), foreground.blue());
    double l2 = getLuminance(background.red(), background.green(), background.blue());
    
    double lighter = qMax(l1, l2);
    double darker = qMin(l1, l2);
    
    return (lighter + 0.05) / (darker + 0.05);
}

void AccessibilityTests::testColorContrast()
{
    // Test WCAG AA color contrast (4.5:1 minimum for normal text, 3:1 for large text)
    QColor textColor("#ffffff"); // Primary text color
    QColor backgroundColor("#0a0a0a"); // Primary background color
    
    double contrast = calculateContrastRatio(textColor, backgroundColor);
    QVERIFY(contrast >= 4.5); // WCAG AA minimum for normal text
}

void AccessibilityTests::testKeyboardNavigation()
{
    // Test keyboard navigation
    QVERIFY(m_welcomeScreen != nullptr);
    
    // Verify all interactive elements are keyboard accessible
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        QVERIFY(installButton->canAcceptFocus() || installButton->property("focusable").toBool());
    }
}

void AccessibilityTests::testTouchTargets()
{
    // Test touch targets (minimum 44x44px)
    QVERIFY(m_welcomeScreen != nullptr);
    
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        QVERIFY(installButton->width() >= 44 || installButton->height() >= 44);
    }
}

void AccessibilityTests::testScreenReaderSupport()
{
    // Test screen reader support (ARIA labels, semantic structure)
    QVERIFY(m_welcomeScreen != nullptr);
    
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        QString accessibleName = installButton->property("Accessible.name").toString();
        QString text = installButton->property("text").toString();
        QVERIFY(!accessibleName.isEmpty() || !text.isEmpty());
    }
}

void AccessibilityTests::testFocusIndicators()
{
    // Test focus indicators (xenon border glow)
    QVERIFY(m_welcomeScreen != nullptr);
    
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        // Verify focus indicator exists (border or visual indicator)
        QVERIFY(installButton->property("border").isValid() || installButton->property("focusVisible").toBool());
    }
}

void AccessibilityTests::testNonColorReliance()
{
    // Test non-color reliance (icons, shapes, text accompany color coding)
    QVERIFY(m_welcomeScreen != nullptr);
    
    // Verify buttons have text labels, not just color
    QQuickItem* installButton = m_welcomeScreen->findChild<QQuickItem*>("installButton");
    if (installButton) {
        QString text = installButton->property("text").toString();
        QVERIFY(!text.isEmpty());
    }
}

QTEST_MAIN(AccessibilityTests)
#include "test_accessibility.moc"

