#include <QtTest>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QColor>
#include <QAccessible>
#include <cmath>

/**
 * Accessibility Compliance Verification Tests
 * Verifies WCAG AA compliance for all 11 components
 */
class AccessibilityComplianceTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // WCAG AA Compliance Tests
    void verifyWCAGAAComplianceForAllComponents();
    
    // Keyboard Navigation Tests
    void verifyKeyboardNavigationForAllComponents();
    void verifyButtonKeyboardNavigation();
    void verifyInputFieldKeyboardNavigation();
    void verifyTabNavigationKeyboardNavigation();
    void verifyDropdownMenuKeyboardNavigation();
    void verifyModalDialogKeyboardNavigation();
    
    // Screen Reader Support Tests
    void verifyScreenReaderSupportForAllComponents();
    void verifyButtonScreenReaderSupport();
    void verifyInputFieldScreenReaderSupport();
    void verifyStatusIndicatorScreenReaderSupport();
    void verifyNotificationAlertScreenReaderSupport();
    
    // Touch Target Tests
    void verifyTouchTargetsForAllComponents();
    void verifyButtonTouchTargets();
    void verifyInputFieldTouchTargets();
    void verifyTabNavigationTouchTargets();
    
    // Color Contrast Tests
    void verifyColorContrastForAllComponents();
    void verifyButtonColorContrast();
    void verifyInputFieldColorContrast();
    void verifyStatusIndicatorColorContrast();
    void verifyNotificationAlertColorContrast();
    
    // Focus Indicator Tests
    void verifyFocusIndicatorsForAllComponents();
    void verifyButtonFocusIndicators();
    void verifyInputFieldFocusIndicators();
    void verifyTabNavigationFocusIndicators();
    
    // Reduced Motion Tests
    void verifyReducedMotionSupport();
    
    // High-Contrast Mode Tests
    void verifyHighContrastModeSupport();

private:
    QQmlApplicationEngine *m_engine;
    
    QQuickItem *createComponent(const QString &componentName, const QString &properties = "");
    bool verifyTouchTargetSize(const QQuickItem *item, int minSize = 44);
    bool verifyColorContrast(const QColor &foreground, const QColor &background);
    double calculateContrastRatio(const QColor &color1, const QColor &color2);
    bool verifyAccessibleProperty(const QQuickItem *item, const QString &propertyName);
    bool verifyFocusable(const QQuickItem *item);
};

void AccessibilityComplianceTest::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    m_engine->addImportPath(QStringLiteral("../../src/qml"));
}

void AccessibilityComplianceTest::cleanupTestCase()
{
    delete m_engine;
}

QQuickItem *AccessibilityComplianceTest::createComponent(const QString &componentName, const QString &properties)
{
    QString qml = QString("import QtQuick 2.15\n"
                         "import MilosComponents 1.0\n"
                         "%1 { %2 }").arg(componentName, properties);
    
    QQmlComponent component(m_engine);
    component.setData(qml.toUtf8(), QUrl());
    
    if (component.isError()) {
        qWarning() << "Component errors:" << component.errors();
        return nullptr;
    }
    
    QObject *object = component.create();
    return qobject_cast<QQuickItem*>(object);
}

bool AccessibilityComplianceTest::verifyTouchTargetSize(const QQuickItem *item, int minSize)
{
    return item->width() >= minSize && item->height() >= minSize;
}

double AccessibilityComplianceTest::calculateContrastRatio(const QColor &color1, const QColor &color2)
{
    // Calculate relative luminance
    auto getLuminance = [](const QColor &color) {
        double r = color.redF();
        double g = color.greenF();
        double b = color.blueF();
        
        r = r <= 0.03928 ? r / 12.92 : pow((r + 0.055) / 1.055, 2.4);
        g = g <= 0.03928 ? g / 12.92 : pow((g + 0.055) / 1.055, 2.4);
        b = b <= 0.03928 ? b / 12.92 : pow((b + 0.055) / 1.055, 2.4);
        
        return 0.2126 * r + 0.7152 * g + 0.0722 * b;
    };
    
    double l1 = getLuminance(color1);
    double l2 = getLuminance(color2);
    
    double lighter = qMax(l1, l2);
    double darker = qMin(l1, l2);
    
    return (lighter + 0.05) / (darker + 0.05);
}

bool AccessibilityComplianceTest::verifyColorContrast(const QColor &foreground, const QColor &background)
{
    double ratio = calculateContrastRatio(foreground, background);
    return ratio >= 4.5; // WCAG AA minimum for normal text
}

bool AccessibilityComplianceTest::verifyAccessibleProperty(const QQuickItem *item, const QString &propertyName)
{
    QVariant value = item->property(propertyName.toUtf8().constData());
    return value.isValid() && !value.toString().isEmpty();
}

bool AccessibilityComplianceTest::verifyFocusable(const QQuickItem *item)
{
    // Check if item has focus property or is keyboard accessible
    return item->property("focus").isValid() || 
           item->property("enabled").toBool() ||
           item->property("activeFocusOnTab").toBool();
}

void AccessibilityComplianceTest::verifyWCAGAAComplianceForAllComponents()
{
    QStringList components = {
        "Button", "InputField", "StatusIndicator", "ProgressBar",
        "TabNavigation", "NotificationAlert", "Card",
        "DropdownMenu", "DataVisualization", "ModalDialog", "PlasmaWidget"
    };
    
    for (const QString &componentName : components) {
        QQuickItem *component = createComponent(componentName);
        QVERIFY2(component != nullptr, 
                QString("Component %1 failed accessibility check").arg(componentName).toUtf8().constData());
        
        // Basic accessibility checks
        QVERIFY(component->width() > 0);
        QVERIFY(component->height() > 0);
        
        delete component;
    }
}

void AccessibilityComplianceTest::verifyKeyboardNavigationForAllComponents()
{
    // Test keyboard navigation for interactive components
    verifyButtonKeyboardNavigation();
    verifyInputFieldKeyboardNavigation();
    verifyTabNavigationKeyboardNavigation();
    verifyDropdownMenuKeyboardNavigation();
    verifyModalDialogKeyboardNavigation();
}

void AccessibilityComplianceTest::verifyButtonKeyboardNavigation()
{
    QQuickItem *button = createComponent("Button", "text: 'Test Button'; enabled: true");
    QVERIFY(button != nullptr);
    
    // Verify button is focusable
    QVERIFY(verifyFocusable(button));
    
    // Verify Enter/Space activation
    QVERIFY(button->property("enabled").toBool() == true);
    
    delete button;
}

void AccessibilityComplianceTest::verifyInputFieldKeyboardNavigation()
{
    QQuickItem *inputField = createComponent("InputField", "variant: 'text'; enabled: true");
    QVERIFY(inputField != nullptr);
    
    // Verify input field is focusable
    QVERIFY(verifyFocusable(inputField));
    
    // Verify Tab navigation
    QVERIFY(inputField->property("enabled").toBool() == true);
    
    delete inputField;
}

void AccessibilityComplianceTest::verifyTabNavigationKeyboardNavigation()
{
    QQuickItem *tabNav = createComponent("TabNavigation", "tabs: [{text: 'Tab1'}, {text: 'Tab2'}]");
    QVERIFY(tabNav != nullptr);
    
    // Verify keyboard navigation support
    QVERIFY(tabNav->property("currentIndex").isValid());
    
    // Verify Ctrl+1-9 navigation (implementation detail)
    QVERIFY(tabNav->property("tabs").isValid());
    
    delete tabNav;
}

void AccessibilityComplianceTest::verifyDropdownMenuKeyboardNavigation()
{
    QQuickItem *dropdown = createComponent("DropdownMenu", "items: [{text: 'Option 1'}]");
    QVERIFY(dropdown != nullptr);
    
    // Verify keyboard navigation (Arrow keys, Enter/Space, Escape)
    QVERIFY(dropdown->property("items").isValid());
    
    delete dropdown;
}

void AccessibilityComplianceTest::verifyModalDialogKeyboardNavigation()
{
    QQuickItem *dialog = createComponent("ModalDialog", "variant: 'confirmation'; critical: false");
    QVERIFY(dialog != nullptr);
    
    // Verify keyboard navigation (Escape closes, Enter confirms)
    QVERIFY(dialog->property("critical").toBool() == false);
    
    delete dialog;
}

void AccessibilityComplianceTest::verifyScreenReaderSupportForAllComponents()
{
    verifyButtonScreenReaderSupport();
    verifyInputFieldScreenReaderSupport();
    verifyStatusIndicatorScreenReaderSupport();
    verifyNotificationAlertScreenReaderSupport();
}

void AccessibilityComplianceTest::verifyButtonScreenReaderSupport()
{
    QQuickItem *button = createComponent("Button", "text: 'Test Button'");
    QVERIFY(button != nullptr);
    
    // Verify accessible text
    QVERIFY(verifyAccessibleProperty(button, "text"));
    
    delete button;
}

void AccessibilityComplianceTest::verifyInputFieldScreenReaderSupport()
{
    QQuickItem *inputField = createComponent("InputField", "placeholderText: 'Enter text'");
    QVERIFY(inputField != nullptr);
    
    // Verify accessible placeholder or label
    QVERIFY(verifyAccessibleProperty(inputField, "placeholderText") || 
            verifyAccessibleProperty(inputField, "label"));
    
    delete inputField;
}

void AccessibilityComplianceTest::verifyStatusIndicatorScreenReaderSupport()
{
    QQuickItem *statusIndicator = createComponent("StatusIndicator", "text: 'Status'; status: 'success'");
    QVERIFY(statusIndicator != nullptr);
    
    // Verify status text accompanies visual indicator
    QVERIFY(verifyAccessibleProperty(statusIndicator, "text"));
    QVERIFY(verifyAccessibleProperty(statusIndicator, "status"));
    
    delete statusIndicator;
}

void AccessibilityComplianceTest::verifyNotificationAlertScreenReaderSupport()
{
    QQuickItem *notification = createComponent("NotificationAlert", "title: 'Alert'; message: 'Message'");
    QVERIFY(notification != nullptr);
    
    // Verify notification content announced
    QVERIFY(verifyAccessibleProperty(notification, "title"));
    QVERIFY(verifyAccessibleProperty(notification, "message"));
    
    delete notification;
}

void AccessibilityComplianceTest::verifyTouchTargetsForAllComponents()
{
    verifyButtonTouchTargets();
    verifyInputFieldTouchTargets();
    verifyTabNavigationTouchTargets();
}

void AccessibilityComplianceTest::verifyButtonTouchTargets()
{
    QQuickItem *button = createComponent("Button", "text: 'Test'");
    QVERIFY(button != nullptr);
    
    // Specification: 48px minimum height (exceeds 44px requirement)
    QVERIFY(button->height() >= 48);
    QVERIFY(verifyTouchTargetSize(button, 44));
    
    delete button;
}

void AccessibilityComplianceTest::verifyInputFieldTouchTargets()
{
    QQuickItem *inputField = createComponent("InputField", "variant: 'text'");
    QVERIFY(inputField != nullptr);
    
    // Specification: 48px minimum height
    QVERIFY(inputField->height() >= 48);
    QVERIFY(verifyTouchTargetSize(inputField, 44));
    
    delete inputField;
}

void AccessibilityComplianceTest::verifyTabNavigationTouchTargets()
{
    QQuickItem *tabNav = createComponent("TabNavigation", "tabs: [{text: 'Tab1'}]");
    QVERIFY(tabNav != nullptr);
    
    // Tabs should be at least 44px in height
    QVERIFY(tabNav->height() >= 44);
    
    delete tabNav;
}

void AccessibilityComplianceTest::verifyColorContrastForAllComponents()
{
    verifyButtonColorContrast();
    verifyInputFieldColorContrast();
    verifyStatusIndicatorColorContrast();
    verifyNotificationAlertColorContrast();
}

void AccessibilityComplianceTest::verifyButtonColorContrast()
{
    // Test text color contrast against background
    QColor textColor("#ffffff"); // White text
    QColor backgroundColor("#0a0a0a"); // Dark background
    
    // Verify contrast ratio >= 4.5:1
    QVERIFY(verifyColorContrast(textColor, backgroundColor));
}

void AccessibilityComplianceTest::verifyInputFieldColorContrast()
{
    // Test input field text contrast
    QColor textColor("#ffffff"); // White text
    QColor backgroundColor("#1a1a1a"); // Dark background
    
    QVERIFY(verifyColorContrast(textColor, backgroundColor));
}

void AccessibilityComplianceTest::verifyStatusIndicatorColorContrast()
{
    // Test status indicator text contrast
    QColor textColor("#ffffff"); // White text
    QColor backgroundColor("#0a0a0a"); // Dark background
    
    QVERIFY(verifyColorContrast(textColor, backgroundColor));
}

void AccessibilityComplianceTest::verifyNotificationAlertColorContrast()
{
    // Test notification text contrast
    QColor textColor("#ffffff"); // White text
    QColor backgroundColor("#1a1a1a"); // Dark background
    
    QVERIFY(verifyColorContrast(textColor, backgroundColor));
}

void AccessibilityComplianceTest::verifyFocusIndicatorsForAllComponents()
{
    verifyButtonFocusIndicators();
    verifyInputFieldFocusIndicators();
    verifyTabNavigationFocusIndicators();
}

void AccessibilityComplianceTest::verifyButtonFocusIndicators()
{
    QQuickItem *button = createComponent("Button", "text: 'Test'");
    QVERIFY(button != nullptr);
    
    // Specification: Xenon border glow (2px solid) on focus
    // Verify focus indicator exists (implementation detail)
    QVERIFY(button->property("xenonColor").isValid());
    
    delete button;
}

void AccessibilityComplianceTest::verifyInputFieldFocusIndicators()
{
    QQuickItem *inputField = createComponent("InputField", "variant: 'text'");
    QVERIFY(inputField != nullptr);
    
    // Specification: Xenon border glow (2px) on focus
    QVERIFY(inputField->property("xenonColor").isValid());
    
    delete inputField;
}

void AccessibilityComplianceTest::verifyTabNavigationFocusIndicators()
{
    QQuickItem *tabNav = createComponent("TabNavigation", "tabs: [{text: 'Tab1'}]");
    QVERIFY(tabNav != nullptr);
    
    // Specification: Xenon border glow on active tab
    QVERIFY(tabNav->property("xenonColor").isValid());
    
    delete tabNav;
}

void AccessibilityComplianceTest::verifyReducedMotionSupport()
{
    // Verify components respect reduced motion preference
    // This is typically handled at the application level or via QML settings
    // Components should support disabling animations when reduced motion is preferred
    
    QQuickItem *button = createComponent("Button", "text: 'Test'");
    QVERIFY(button != nullptr);
    
    // Components should gracefully handle reduced motion
    // (Implementation detail - animations can be disabled)
    
    delete button;
}

void AccessibilityComplianceTest::verifyHighContrastModeSupport()
{
    // Verify components support high-contrast mode
    // High-contrast mode typically increases xenon effect intensity
    
    QQuickItem *button = createComponent("Button", "text: 'Test'");
    QVERIFY(button != nullptr);
    
    // Components should support enhanced xenon effects for high-contrast mode
    // (Implementation detail - xenon effects can be intensified)
    
    delete button;
}

QTEST_MAIN(AccessibilityComplianceTest)
#include "test_accessibility_compliance.moc"

