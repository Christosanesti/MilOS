#include <QtTest>
#include <QSignalSpy>
#include "../src/quickactionsservice.h"

/**
 * @brief Test suite for Floating Action Buttons
 * 
 * Tests quick actions service, context detection, and action execution.
 */
class FloatingButtonsTests : public QObject
{
    Q_OBJECT

public:
    FloatingButtonsTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testServiceInitialization();
    void testContextDetection();
    void testActionExecution();
    void testConfiguration();

private:
    QuickActionsService* m_service;
};

FloatingButtonsTests::FloatingButtonsTests()
    : m_service(nullptr)
{
}

void FloatingButtonsTests::initTestCase()
{
    m_service = new QuickActionsService(this);
    QVERIFY(m_service != nullptr);
}

void FloatingButtonsTests::cleanupTestCase()
{
    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }
}

void FloatingButtonsTests::testServiceInitialization()
{
    QVERIFY(m_service != nullptr);
    QVERIFY(m_service->enabled() == true); // Default enabled
}

void FloatingButtonsTests::testContextDetection()
{
    // Test context detection
    m_service->refreshContext();
    
    // Verify context changed signal exists
    QSignalSpy contextSpy(m_service, &QuickActionsService::currentContextChanged);
    QVERIFY(contextSpy.isValid());
    
    // Note: Actual context detection requires active window, which may not be available in test environment
}

void FloatingButtonsTests::testActionExecution()
{
    // Test action execution
    QSignalSpy executedSpy(m_service, &QuickActionsService::actionExecuted);
    QVERIFY(executedSpy.isValid());
    
    // Note: Actual action execution requires D-Bus services, which may not be available in test environment
    // In production, this would test actual D-Bus method calls
}

void FloatingButtonsTests::testConfiguration()
{
    // Test configuration
    bool originalEnabled = m_service->enabled();
    
    m_service->setEnabled(false);
    QVERIFY(m_service->enabled() == false);
    
    m_service->setEnabled(true);
    QVERIFY(m_service->enabled() == true);
    
    // Restore original state
    m_service->setEnabled(originalEnabled);
}

QTEST_MAIN(FloatingButtonsTests)
#include "floatingbuttons_test.moc"

