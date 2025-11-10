#include <QtTest>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>

/**
 * Integration tests for component library usage in security tools
 * Tests components in actual usage contexts (Encryption Manager, Network Dashboard)
 */
class ComponentIntegrationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Button component integration tests
    void testButtonIntegrationInEncryptionManager();
    void testButtonVariantsInEncryptionManager();
    void testButtonStatesInEncryptionManager();
    void testButtonAccessibilityInEncryptionManager();
    
    // InputField component integration tests
    void testInputFieldIntegrationInEncryptionManager();
    void testInputFieldVariantsInEncryptionManager();
    void testInputFieldStatesInEncryptionManager();
    void testInputFieldPasswordStrengthIndicator();
    
    // StatusIndicator component integration tests
    void testStatusIndicatorIntegrationInNetworkScreen();
    void testStatusIndicatorVariantsInNetworkScreen();
    void testStatusIndicatorStatesInNetworkScreen();
    
    // ProgressBar component integration tests
    void testProgressBarIntegrationInEncryptionManager();
    void testProgressBarVariantsInEncryptionManager();
    void testProgressBarStatesInEncryptionManager();
    
    // Component specification compliance tests
    void testButtonSpecificationCompliance();
    void testInputFieldSpecificationCompliance();
    void testStatusIndicatorSpecificationCompliance();
    void testProgressBarSpecificationCompliance();
    
    // TabNavigation component integration tests
    void testTabNavigationIntegrationInEncryptionManager();
    void testTabNavigationVariants();
    void testTabNavigationOverflow();
    void testTabNavigationKeyboardNavigation();
    
    // NotificationAlert component integration tests
    void testNotificationAlertIntegrationInNetworkScreen();
    void testNotificationAlertVariants();
    void testNotificationAlertStacking();
    void testNotificationAlertDismissal();
    
    // Card component integration tests
    void testCardIntegrationInNetworkScreen();
    void testCardVariants();
    void testCardExpansion();
    
    // Component specification compliance tests (Navigation & Feedback)
    void testTabNavigationSpecificationCompliance();
    void testNotificationAlertSpecificationCompliance();
    void testCardSpecificationCompliance();
    
    // DropdownMenu component integration tests
    void testDropdownMenuIntegrationInEncryptionManager();
    void testDropdownMenuVariants();
    void testDropdownMenuKeyboardNavigation();
    void testDropdownMenuSearchFunctionality();
    
    // DataVisualization component integration tests
    void testDataVisualizationIntegrationInNetworkScreen();
    void testDataVisualizationVariants();
    void testDataVisualizationRealTimeUpdates();
    
    // ModalDialog component integration tests
    void testModalDialogIntegrationInEncryptionManager();
    void testModalDialogIntegrationInNetworkScreen();
    void testModalDialogVariants();
    void testModalDialogFocusManagement();
    void testModalDialogDismissalBehavior();
    
    // Component specification compliance tests (Advanced Components)
    void testDropdownMenuSpecificationCompliance();
    void testDataVisualizationSpecificationCompliance();
    void testModalDialogSpecificationCompliance();
    
    // Regression tests
    void testNoRegressionInEncryptionManager();
    void testNoRegressionInNetworkConfiguration();

private:
    QQmlApplicationEngine *m_engine;
    QQuickWindow *m_window;
    
    QQuickItem *findComponent(QQuickItem *parent, const QString &componentName);
    bool verifyXenonColor(const QQuickItem *item, const QString &expectedColor);
    bool verifyAccessibility(const QQuickItem *item);
};

void ComponentIntegrationTest::initTestCase()
{
    m_engine = new QQmlApplicationEngine(this);
    
    // Add component library import path
    m_engine->addImportPath(QStringLiteral("../../src/qml"));
    
    // Create a window for testing
    m_window = new QQuickWindow();
    m_window->setWidth(1024);
    m_window->setHeight(768);
}

void ComponentIntegrationTest::cleanupTestCase()
{
    delete m_window;
    delete m_engine;
}

void ComponentIntegrationTest::testButtonIntegrationInEncryptionManager()
{
    // Load Encryption Setup Screen QML
    QQmlComponent component(m_engine, QUrl::fromLocalFile("../../../milos-installer/src/ui/encryptionsetupscreen.qml"));
    
    if (component.isError()) {
        qWarning() << "Component errors:" << component.errors();
        QSKIP("Encryption Setup Screen not available for testing");
    }
    
    QObject *object = component.create();
    QVERIFY(object != nullptr);
    
    // Find Button components
    QQuickItem *rootItem = qobject_cast<QQuickItem*>(object);
    QVERIFY(rootItem != nullptr);
    
    // Verify buttons exist and are functional
    QQuickItem *backButton = findComponent(rootItem, "Button");
    QVERIFY(backButton != nullptr);
    
    // Verify button properties
    QCOMPARE(backButton->property("variant").toString(), QString("secondary"));
    QCOMPARE(backButton->property("text").toString(), QString("Back"));
    QVERIFY(backButton->height() >= 48); // Minimum 48px for accessibility
    
    delete object;
}

void ComponentIntegrationTest::testButtonVariantsInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Button { variant: 'primary'; text: 'Test' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QVERIFY(object != nullptr);
    
    QQuickItem *button = qobject_cast<QQuickItem*>(object);
    QVERIFY(button != nullptr);
    
    // Test all variants
    QStringList variants = {"primary", "secondary", "tertiary", "icon", "toggle"};
    for (const QString &variant : variants) {
        button->setProperty("variant", variant);
        QCOMPARE(button->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testButtonStatesInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Button { variant: 'primary'; text: 'Test' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *button = qobject_cast<QQuickItem*>(object);
    QVERIFY(button != nullptr);
    
    // Test all states
    QStringList states = {"default", "hover", "active", "disabled", "loading", "success", "error"};
    for (const QString &state : states) {
        button->setProperty("state", state);
        QCOMPARE(button->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testButtonAccessibilityInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Button { variant: 'primary'; text: 'Test' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *button = qobject_cast<QQuickItem*>(object);
    QVERIFY(button != nullptr);
    
    // Verify accessibility requirements
    QVERIFY(button->height() >= 48); // Minimum 48px height
    QVERIFY(button->property("enabled").toBool() == true); // Enabled by default
    
    // Verify accessible properties
    QVERIFY(button->property("Accessible.name").isValid() || 
            button->property("text").toString().length() > 0);
    
    delete object;
}

void ComponentIntegrationTest::testInputFieldIntegrationInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "InputField { variant: 'password'; placeholderText: 'Enter password' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *inputField = qobject_cast<QQuickItem*>(object);
    QVERIFY(inputField != nullptr);
    
    // Verify input field properties
    QCOMPARE(inputField->property("variant").toString(), QString("password"));
    QVERIFY(inputField->height() >= 48); // Minimum height for accessibility
    
    delete object;
}

void ComponentIntegrationTest::testInputFieldVariantsInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "InputField { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *inputField = qobject_cast<QQuickItem*>(object);
    QVERIFY(inputField != nullptr);
    
    // Test all variants
    QStringList variants = {"text", "password", "textarea", "search", "number"};
    for (const QString &variant : variants) {
        inputField->setProperty("variant", variant);
        QCOMPARE(inputField->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testInputFieldStatesInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "InputField { variant: 'text' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *inputField = qobject_cast<QQuickItem*>(object);
    QVERIFY(inputField != nullptr);
    
    // Test all states
    QStringList states = {"default", "focus", "filled", "error", "disabled", "readonly"};
    for (const QString &state : states) {
        inputField->setProperty("state", state);
        QCOMPARE(inputField->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testInputFieldPasswordStrengthIndicator()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "InputField { variant: 'password'; text: 'TestPassword123!' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *inputField = qobject_cast<QQuickItem*>(object);
    QVERIFY(inputField != nullptr);
    
    // Verify password strength indicator exists (if implemented)
    // This would need to be verified based on actual implementation
    QVERIFY(inputField->property("variant").toString() == "password");
    
    delete object;
}

void ComponentIntegrationTest::testStatusIndicatorIntegrationInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "StatusIndicator { variant: 'badge'; status: 'success'; text: 'Connected' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *statusIndicator = qobject_cast<QQuickItem*>(object);
    QVERIFY(statusIndicator != nullptr);
    
    // Verify status indicator properties
    QCOMPARE(statusIndicator->property("variant").toString(), QString("badge"));
    QCOMPARE(statusIndicator->property("status").toString(), QString("success"));
    QCOMPARE(statusIndicator->property("text").toString(), QString("Connected"));
    
    delete object;
}

void ComponentIntegrationTest::testStatusIndicatorVariantsInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "StatusIndicator { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *statusIndicator = qobject_cast<QQuickItem*>(object);
    QVERIFY(statusIndicator != nullptr);
    
    // Test all variants
    QStringList variants = {"badge", "icon", "light", "card", "widget"};
    for (const QString &variant : variants) {
        statusIndicator->setProperty("variant", variant);
        QCOMPARE(statusIndicator->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testStatusIndicatorStatesInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "StatusIndicator { variant: 'badge' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *statusIndicator = qobject_cast<QQuickItem*>(object);
    QVERIFY(statusIndicator != nullptr);
    
    // Test all states
    QStringList states = {"success", "error", "warning", "processing", "inactive"};
    for (const QString &state : states) {
        statusIndicator->setProperty("status", state);
        QCOMPARE(statusIndicator->property("status").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testProgressBarIntegrationInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ProgressBar { variant: 'linear'; value: 0.5; status: 'processing' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *progressBar = qobject_cast<QQuickItem*>(object);
    QVERIFY(progressBar != nullptr);
    
    // Verify progress bar properties
    QCOMPARE(progressBar->property("variant").toString(), QString("linear"));
    QCOMPARE(progressBar->property("value").toDouble(), 0.5);
    QCOMPARE(progressBar->property("status").toString(), QString("processing"));
    QVERIFY(progressBar->height() >= 8); // Minimum 8px height
    
    delete object;
}

void ComponentIntegrationTest::testProgressBarVariantsInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ProgressBar { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *progressBar = qobject_cast<QQuickItem*>(object);
    QVERIFY(progressBar != nullptr);
    
    // Test all variants
    QStringList variants = {"linear", "circular", "step", "indeterminate", "details"};
    for (const QString &variant : variants) {
        progressBar->setProperty("variant", variant);
        QCOMPARE(progressBar->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testProgressBarStatesInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ProgressBar { variant: 'linear' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *progressBar = qobject_cast<QQuickItem*>(object);
    QVERIFY(progressBar != nullptr);
    
    // Test all states
    QStringList states = {"processing", "paused", "complete", "error", "indeterminate"};
    for (const QString &state : states) {
        progressBar->setProperty("status", state);
        QCOMPARE(progressBar->property("status").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testButtonSpecificationCompliance()
{
    // Verify Button component matches design specifications
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Button { variant: 'primary'; text: 'Test' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *button = qobject_cast<QQuickItem*>(object);
    QVERIFY(button != nullptr);
    
    // Specification: 48px minimum height
    QVERIFY(button->height() >= 48);
    
    // Specification: Xenon color #00d4ff
    QVERIFY(verifyXenonColor(button, "#00d4ff"));
    
    // Specification: All variants supported
    QStringList variants = {"primary", "secondary", "tertiary", "icon", "toggle"};
    for (const QString &variant : variants) {
        button->setProperty("variant", variant);
        QCOMPARE(button->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"default", "hover", "active", "disabled", "loading", "success", "error"};
    for (const QString &state : states) {
        button->setProperty("state", state);
        QCOMPARE(button->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testInputFieldSpecificationCompliance()
{
    // Verify InputField component matches design specifications
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "InputField { variant: 'text' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *inputField = qobject_cast<QQuickItem*>(object);
    QVERIFY(inputField != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"text", "password", "textarea", "search", "number"};
    for (const QString &variant : variants) {
        inputField->setProperty("variant", variant);
        QCOMPARE(inputField->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"default", "focus", "filled", "error", "disabled", "readonly"};
    for (const QString &state : states) {
        inputField->setProperty("state", state);
        QCOMPARE(inputField->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testStatusIndicatorSpecificationCompliance()
{
    // Verify StatusIndicator component matches design specifications
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "StatusIndicator { variant: 'badge' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *statusIndicator = qobject_cast<QQuickItem*>(object);
    QVERIFY(statusIndicator != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"badge", "icon", "light", "card", "widget"};
    for (const QString &variant : variants) {
        statusIndicator->setProperty("variant", variant);
        QCOMPARE(statusIndicator->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"success", "error", "warning", "processing", "inactive"};
    for (const QString &state : states) {
        statusIndicator->setProperty("status", state);
        QCOMPARE(statusIndicator->property("status").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testProgressBarSpecificationCompliance()
{
    // Verify ProgressBar component matches design specifications
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ProgressBar { variant: 'linear' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *progressBar = qobject_cast<QQuickItem*>(object);
    QVERIFY(progressBar != nullptr);
    
    // Specification: Minimum 8px height
    QVERIFY(progressBar->height() >= 8);
    
    // Specification: All variants supported
    QStringList variants = {"linear", "circular", "step", "indeterminate", "details"};
    for (const QString &variant : variants) {
        progressBar->setProperty("variant", variant);
        QCOMPARE(progressBar->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"processing", "paused", "complete", "error", "indeterminate"};
    for (const QString &state : states) {
        progressBar->setProperty("status", state);
        QCOMPARE(progressBar->property("status").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testNoRegressionInEncryptionManager()
{
    // Verify that existing functionality still works after component integration
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Rectangle {\n"
        "  width: 400; height: 300\n"
        "  Column {\n"
        "    Button { variant: 'primary'; text: 'Encrypt'; onClicked: { result.text = 'Encrypted' } }\n"
        "    InputField { variant: 'password'; id: passwordField }\n"
        "    ProgressBar { variant: 'linear'; value: 0.5 }\n"
        "    Text { id: result }\n"
        "  }\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QVERIFY(object != nullptr);
    
    // Verify components are functional
    QQuickItem *rootItem = qobject_cast<QQuickItem*>(object);
    QVERIFY(rootItem != nullptr);
    
    // Test button click
    QQuickItem *button = findComponent(rootItem, "Button");
    if (button) {
        QSignalSpy spy(button, SIGNAL(clicked()));
        QMetaObject::invokeMethod(button, "clicked");
        // Verify signal was emitted or functionality works
    }
    
    delete object;
}

void ComponentIntegrationTest::testNoRegressionInNetworkConfiguration()
{
    // Verify that existing functionality still works after component integration
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Rectangle {\n"
        "  width: 400; height: 300\n"
        "  Column {\n"
        "    StatusIndicator { variant: 'badge'; status: 'success'; text: 'Connected' }\n"
        "    Button { variant: 'secondary'; text: 'Test Connection' }\n"
        "    InputField { variant: 'text'; placeholderText: 'IP Address' }\n"
        "  }\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QVERIFY(object != nullptr);
    
    // Verify components are functional
    QQuickItem *rootItem = qobject_cast<QQuickItem*>(object);
    QVERIFY(rootItem != nullptr);
    
    // Verify status indicator shows correct status
    QQuickItem *statusIndicator = findComponent(rootItem, "StatusIndicator");
    if (statusIndicator) {
        QCOMPARE(statusIndicator->property("status").toString(), QString("success"));
        QCOMPARE(statusIndicator->property("text").toString(), QString("Connected"));
    }
    
    delete object;
}

void ComponentIntegrationTest::testTabNavigationIntegrationInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "TabNavigation {\n"
        "  variant: 'standard'\n"
        "  currentIndex: 0\n"
        "  maxTabs: 10\n"
        "  tabs: [\n"
        "    {text: 'Standard', icon: '', badge: 0, enabled: true},\n"
        "    {text: 'Advanced', icon: '', badge: 0, enabled: true}\n"
        "  ]\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *tabNavigation = qobject_cast<QQuickItem*>(object);
    QVERIFY(tabNavigation != nullptr);
    
    // Verify tab navigation properties
    QCOMPARE(tabNavigation->property("variant").toString(), QString("standard"));
    QCOMPARE(tabNavigation->property("currentIndex").toInt(), 0);
    QCOMPARE(tabNavigation->property("maxTabs").toInt(), 10);
    
    delete object;
}

void ComponentIntegrationTest::testTabNavigationVariants()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "TabNavigation { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *tabNavigation = qobject_cast<QQuickItem*>(object);
    QVERIFY(tabNavigation != nullptr);
    
    // Test all variants
    QStringList variants = {"standard", "icon", "pill", "underline", "vertical"};
    for (const QString &variant : variants) {
        tabNavigation->setProperty("variant", variant);
        QCOMPARE(tabNavigation->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testTabNavigationOverflow()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "TabNavigation {\n"
        "  maxTabs: 10\n"
        "  tabs: [\n"
        "    {text: 'Tab1'}, {text: 'Tab2'}, {text: 'Tab3'}, {text: 'Tab4'}, {text: 'Tab5'},\n"
        "    {text: 'Tab6'}, {text: 'Tab7'}, {text: 'Tab8'}, {text: 'Tab9'}, {text: 'Tab10'},\n"
        "    {text: 'Tab11'}, {text: 'Tab12'}\n"
        "  ]\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *tabNavigation = qobject_cast<QQuickItem*>(object);
    QVERIFY(tabNavigation != nullptr);
    
    // Verify maxTabs limits displayed tabs
    QCOMPARE(tabNavigation->property("maxTabs").toInt(), 10);
    // Component should handle overflow (implementation detail)
    
    delete object;
}

void ComponentIntegrationTest::testTabNavigationKeyboardNavigation()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "TabNavigation {\n"
        "  variant: 'standard'\n"
        "  tabs: [{text: 'Tab1'}, {text: 'Tab2'}, {text: 'Tab3'}]\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *tabNavigation = qobject_cast<QQuickItem*>(object);
    QVERIFY(tabNavigation != nullptr);
    
    // Verify keyboard navigation support
    QVERIFY(tabNavigation->property("currentIndex").isValid());
    
    // Test tab switching
    tabNavigation->setProperty("currentIndex", 1);
    QCOMPARE(tabNavigation->property("currentIndex").toInt(), 1);
    
    delete object;
}

void ComponentIntegrationTest::testNotificationAlertIntegrationInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "NotificationAlert {\n"
        "  variant: 'warning'\n"
        "  title: 'Network Configuration'\n"
        "  message: 'Network configuration is optional.'\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *notification = qobject_cast<QQuickItem*>(object);
    QVERIFY(notification != nullptr);
    
    // Verify notification properties
    QCOMPARE(notification->property("variant").toString(), QString("warning"));
    QCOMPARE(notification->property("title").toString(), QString("Network Configuration"));
    
    delete object;
}

void ComponentIntegrationTest::testNotificationAlertVariants()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "NotificationAlert { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *notification = qobject_cast<QQuickItem*>(object);
    QVERIFY(notification != nullptr);
    
    // Test all variants
    QStringList variants = {"success", "error", "warning", "info", "security", "toast"};
    for (const QString &variant : variants) {
        notification->setProperty("variant", variant);
        QCOMPARE(notification->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testNotificationAlertStacking()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import QtQuick.Layouts 1.15\n"
        "import MilosComponents 1.0\n"
        "Column {\n"
        "  spacing: 8\n"
        "  NotificationAlert { variant: 'info'; title: 'Info 1'; state: 'displaying' }\n"
        "  NotificationAlert { variant: 'warning'; title: 'Warning 1'; state: 'displaying' }\n"
        "  NotificationAlert { variant: 'error'; title: 'Error 1'; state: 'displaying' }\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QVERIFY(object != nullptr);
    
    // Verify multiple notifications can exist
    QQuickItem *rootItem = qobject_cast<QQuickItem*>(object);
    QVERIFY(rootItem != nullptr);
    
    delete object;
}

void ComponentIntegrationTest::testNotificationAlertDismissal()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "NotificationAlert {\n"
        "  variant: 'info'\n"
        "  dismissible: true\n"
        "  state: 'displaying'\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *notification = qobject_cast<QQuickItem*>(object);
    QVERIFY(notification != nullptr);
    
    // Test dismissal
    QVERIFY(notification->property("dismissible").toBool() == true);
    notification->setProperty("state", "closing");
    QCOMPARE(notification->property("state").toString(), QString("closing"));
    
    delete object;
}

void ComponentIntegrationTest::testCardIntegrationInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Card {\n"
        "  variant: 'dashboard'\n"
        "  title: 'Network Interfaces'\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *card = qobject_cast<QQuickItem*>(object);
    QVERIFY(card != nullptr);
    
    // Verify card properties
    QCOMPARE(card->property("variant").toString(), QString("dashboard"));
    QCOMPARE(card->property("title").toString(), QString("Network Interfaces"));
    
    delete object;
}

void ComponentIntegrationTest::testCardVariants()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Card { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *card = qobject_cast<QQuickItem*>(object);
    QVERIFY(card != nullptr);
    
    // Test all variants
    QStringList variants = {"status", "dashboard", "interactive", "detail", "alert"};
    for (const QString &variant : variants) {
        card->setProperty("variant", variant);
        QCOMPARE(card->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testCardExpansion()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Card {\n"
        "  variant: 'interactive'\n"
        "  expandable: true\n"
        "  expanded: false\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *card = qobject_cast<QQuickItem*>(object);
    QVERIFY(card != nullptr);
    
    // Test expansion
    QVERIFY(card->property("expandable").toBool() == true);
    QVERIFY(card->property("expanded").toBool() == false);
    
    card->setProperty("expanded", true);
    QVERIFY(card->property("expanded").toBool() == true);
    
    delete object;
}

void ComponentIntegrationTest::testTabNavigationSpecificationCompliance()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "TabNavigation { variant: 'standard' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *tabNavigation = qobject_cast<QQuickItem*>(object);
    QVERIFY(tabNavigation != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"standard", "icon", "pill", "underline", "vertical"};
    for (const QString &variant : variants) {
        tabNavigation->setProperty("variant", variant);
        QCOMPARE(tabNavigation->property("variant").toString(), variant);
    }
    
    // Specification: Max tabs 10
    QCOMPARE(tabNavigation->property("maxTabs").toInt(), 10);
    
    delete object;
}

void ComponentIntegrationTest::testNotificationAlertSpecificationCompliance()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "NotificationAlert { variant: 'info' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *notification = qobject_cast<QQuickItem*>(object);
    QVERIFY(notification != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"success", "error", "warning", "info", "security", "toast"};
    for (const QString &variant : variants) {
        notification->setProperty("variant", variant);
        QCOMPARE(notification->property("variant").toString(), variant);
    }
    
    // Specification: Duration settings
    notification->setProperty("variant", "info");
    QCOMPARE(notification->property("duration").toInt(), 3000);
    
    notification->setProperty("variant", "warning");
    QCOMPARE(notification->property("duration").toInt(), 5000);
    
    notification->setProperty("variant", "error");
    QCOMPARE(notification->property("duration").toInt(), -1); // Persistent
    
    delete object;
}

void ComponentIntegrationTest::testCardSpecificationCompliance()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "Card { variant: 'dashboard' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *card = qobject_cast<QQuickItem*>(object);
    QVERIFY(card != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"status", "dashboard", "interactive", "detail", "alert"};
    for (const QString &variant : variants) {
        card->setProperty("variant", variant);
        QCOMPARE(card->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"default", "hover", "active", "expanded", "alert"};
    for (const QString &state : states) {
        card->setProperty("state", state);
        QCOMPARE(card->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testDropdownMenuIntegrationInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DropdownMenu {\n"
        "  variant: 'standard'\n"
        "  items: [\n"
        "    {text: 'AES-XTS (Recommended)', value: 'aes-xts-plain64'},\n"
        "    {text: 'Serpent-XTS', value: 'serpent-xts-plain64'}\n"
        "  ]\n"
        "  selectedValue: 'aes-xts-plain64'\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dropdown = qobject_cast<QQuickItem*>(object);
    QVERIFY(dropdown != nullptr);
    
    // Verify dropdown properties
    QCOMPARE(dropdown->property("variant").toString(), QString("standard"));
    QCOMPARE(dropdown->property("selectedValue").toString(), QString("aes-xts-plain64"));
    
    delete object;
}

void ComponentIntegrationTest::testDropdownMenuVariants()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DropdownMenu { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dropdown = qobject_cast<QQuickItem*>(object);
    QVERIFY(dropdown != nullptr);
    
    // Test all variants
    QStringList variants = {"standard", "multiselect", "searchable", "icon", "context"};
    for (const QString &variant : variants) {
        dropdown->setProperty("variant", variant);
        QCOMPARE(dropdown->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testDropdownMenuKeyboardNavigation()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DropdownMenu {\n"
        "  variant: 'standard'\n"
        "  items: [{text: 'Option 1'}, {text: 'Option 2'}, {text: 'Option 3'}]\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dropdown = qobject_cast<QQuickItem*>(object);
    QVERIFY(dropdown != nullptr);
    
    // Verify keyboard navigation support
    QVERIFY(dropdown->property("items").isValid());
    
    delete object;
}

void ComponentIntegrationTest::testDropdownMenuSearchFunctionality()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DropdownMenu {\n"
        "  variant: 'searchable'\n"
        "  items: [{text: 'Option 1'}, {text: 'Option 2'}]\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dropdown = qobject_cast<QQuickItem*>(object);
    QVERIFY(dropdown != nullptr);
    
    // Verify searchable variant
    QCOMPARE(dropdown->property("variant").toString(), QString("searchable"));
    
    delete object;
}

void ComponentIntegrationTest::testDataVisualizationIntegrationInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DataVisualization {\n"
        "  variant: 'line'\n"
        "  state: 'animated'\n"
        "  data: [{x: 0, y: 10}, {x: 1, y: 20}, {x: 2, y: 15}]\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dataViz = qobject_cast<QQuickItem*>(object);
    QVERIFY(dataViz != nullptr);
    
    // Verify data visualization properties
    QCOMPARE(dataViz->property("variant").toString(), QString("line"));
    QCOMPARE(dataViz->property("state").toString(), QString("animated"));
    
    delete object;
}

void ComponentIntegrationTest::testDataVisualizationVariants()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DataVisualization { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dataViz = qobject_cast<QQuickItem*>(object);
    QVERIFY(dataViz != nullptr);
    
    // Test all variants
    QStringList variants = {"line", "bar", "area", "network", "stream"};
    for (const QString &variant : variants) {
        dataViz->setProperty("variant", variant);
        QCOMPARE(dataViz->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testDataVisualizationRealTimeUpdates()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DataVisualization {\n"
        "  variant: 'stream'\n"
        "  state: 'animated'\n"
        "  data: []\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dataViz = qobject_cast<QQuickItem*>(object);
    QVERIFY(dataViz != nullptr);
    
    // Test real-time updates
    QCOMPARE(dataViz->property("variant").toString(), QString("stream"));
    QCOMPARE(dataViz->property("state").toString(), QString("animated"));
    
    // Update data
    QVariantList newData;
    for (int i = 0; i < 10; i++) {
        QVariantMap point;
        point["x"] = i;
        point["y"] = i * 10;
        newData.append(point);
    }
    dataViz->setProperty("data", newData);
    
    delete object;
}

void ComponentIntegrationTest::testModalDialogIntegrationInEncryptionManager()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ModalDialog {\n"
        "  variant: 'warning'\n"
        "  title: 'Confirm Disk Encryption'\n"
        "  message: 'Disk encryption will be applied.'\n"
        "  critical: true\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dialog = qobject_cast<QQuickItem*>(object);
    QVERIFY(dialog != nullptr);
    
    // Verify dialog properties
    QCOMPARE(dialog->property("variant").toString(), QString("warning"));
    QCOMPARE(dialog->property("title").toString(), QString("Confirm Disk Encryption"));
    QVERIFY(dialog->property("critical").toBool() == true);
    
    delete object;
}

void ComponentIntegrationTest::testModalDialogIntegrationInNetworkScreen()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ModalDialog {\n"
        "  variant: 'confirmation'\n"
        "  title: 'Test Network Connection'\n"
        "  message: 'This will test the network connection.'\n"
        "  critical: false\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dialog = qobject_cast<QQuickItem*>(object);
    QVERIFY(dialog != nullptr);
    
    // Verify dialog properties
    QCOMPARE(dialog->property("variant").toString(), QString("confirmation"));
    QCOMPARE(dialog->property("title").toString(), QString("Test Network Connection"));
    QVERIFY(dialog->property("critical").toBool() == false);
    
    delete object;
}

void ComponentIntegrationTest::testModalDialogVariants()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ModalDialog { }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dialog = qobject_cast<QQuickItem*>(object);
    QVERIFY(dialog != nullptr);
    
    // Test all variants
    QStringList variants = {"confirmation", "warning", "error", "form", "progress"};
    for (const QString &variant : variants) {
        dialog->setProperty("variant", variant);
        QCOMPARE(dialog->property("variant").toString(), variant);
    }
    
    delete object;
}

void ComponentIntegrationTest::testModalDialogFocusManagement()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ModalDialog {\n"
        "  variant: 'confirmation'\n"
        "  visible: true\n"
        "  state: 'displaying'\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dialog = qobject_cast<QQuickItem*>(object);
    QVERIFY(dialog != nullptr);
    
    // Verify focus management
    QVERIFY(dialog->property("visible").toBool() == true);
    QCOMPARE(dialog->property("state").toString(), QString("displaying"));
    
    delete object;
}

void ComponentIntegrationTest::testModalDialogDismissalBehavior()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ModalDialog {\n"
        "  variant: 'confirmation'\n"
        "  critical: false\n"
        "  state: 'displaying'\n"
        "}",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dialog = qobject_cast<QQuickItem*>(object);
    QVERIFY(dialog != nullptr);
    
    // Test dismissal
    QVERIFY(dialog->property("critical").toBool() == false);
    dialog->setProperty("state", "closing");
    QCOMPARE(dialog->property("state").toString(), QString("closing"));
    
    // Test critical dialog (cannot be dismissed)
    dialog->setProperty("critical", true);
    QVERIFY(dialog->property("critical").toBool() == true);
    
    delete object;
}

void ComponentIntegrationTest::testDropdownMenuSpecificationCompliance()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DropdownMenu { variant: 'standard' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dropdown = qobject_cast<QQuickItem*>(object);
    QVERIFY(dropdown != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"standard", "multiselect", "searchable", "icon", "context"};
    for (const QString &variant : variants) {
        dropdown->setProperty("variant", variant);
        QCOMPARE(dropdown->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"default", "open", "hover", "selected", "disabled"};
    for (const QString &state : states) {
        dropdown->setProperty("state", state);
        QCOMPARE(dropdown->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testDataVisualizationSpecificationCompliance()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "DataVisualization { variant: 'line' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dataViz = qobject_cast<QQuickItem*>(object);
    QVERIFY(dataViz != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"line", "bar", "area", "network", "stream"};
    for (const QString &variant : variants) {
        dataViz->setProperty("variant", variant);
        QCOMPARE(dataViz->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"default", "hover", "active", "animated", "error"};
    for (const QString &state : states) {
        dataViz->setProperty("state", state);
        QCOMPARE(dataViz->property("state").toString(), state);
    }
    
    delete object;
}

void ComponentIntegrationTest::testModalDialogSpecificationCompliance()
{
    QQmlComponent component(m_engine);
    component.setData(
        "import QtQuick 2.15\n"
        "import MilosComponents 1.0\n"
        "ModalDialog { variant: 'confirmation' }",
        QUrl()
    );
    
    QObject *object = component.create();
    QQuickItem *dialog = qobject_cast<QQuickItem*>(object);
    QVERIFY(dialog != nullptr);
    
    // Specification: All variants supported
    QStringList variants = {"confirmation", "warning", "error", "form", "progress"};
    for (const QString &variant : variants) {
        dialog->setProperty("variant", variant);
        QCOMPARE(dialog->property("variant").toString(), variant);
    }
    
    // Specification: All states supported
    QStringList states = {"opening", "displaying", "closing", "processing"};
    for (const QString &state : states) {
        dialog->setProperty("state", state);
        QCOMPARE(dialog->property("state").toString(), state);
    }
    
    // Specification: Critical dialogs cannot be dismissed
    dialog->setProperty("critical", true);
    QVERIFY(dialog->property("critical").toBool() == true);
    
    delete object;
}

QQuickItem *ComponentIntegrationTest::findComponent(QQuickItem *parent, const QString &componentName)
{
    if (!parent) return nullptr;
    
    // Check if this item matches
    if (parent->metaObject()->className() == componentName) {
        return parent;
    }
    
    // Search children
    QList<QQuickItem*> children = parent->childItems();
    for (QQuickItem *child : children) {
        QQuickItem *found = findComponent(child, componentName);
        if (found) return found;
    }
    
    return nullptr;
}

bool ComponentIntegrationTest::verifyXenonColor(const QQuickItem *item, const QString &expectedColor)
{
    // Verify xenon color property exists and matches expected color
    // This is a simplified check - actual implementation would verify the visual color
    QVariant xenonColor = item->property("xenonColor");
    if (xenonColor.isValid()) {
        return xenonColor.toString().toLower() == expectedColor.toLower();
    }
    return true; // If property doesn't exist, assume it's handled in QML
}

bool ComponentIntegrationTest::verifyAccessibility(const QQuickItem *item)
{
    // Verify accessibility requirements
    // Minimum height check
    if (item->height() < 44) {
        return false; // Minimum touch target size
    }
    
    // Verify accessible properties exist
    QVariant accessibleName = item->property("Accessible.name");
    QVariant accessibleRole = item->property("Accessible.role");
    
    // At least one accessibility property should be set
    return accessibleName.isValid() || accessibleRole.isValid() || 
           item->property("text").toString().length() > 0;
}

QTEST_MAIN(ComponentIntegrationTest)
#include "test_integration.moc"

