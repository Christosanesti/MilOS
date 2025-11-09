#include <QtTest>
#include <QObject>
#include <QQuickItem>
#include "../src/components/Button.h"

class ButtonTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testButtonCreation();
    void testButtonVariants();
    void testButtonStates();
    void testButtonEnabled();
    void testButtonChecked();
    void testButtonText();

private:
    Button *m_button;
};

void ButtonTest::initTestCase()
{
    m_button = new Button();
}

void ButtonTest::cleanupTestCase()
{
    delete m_button;
}

void ButtonTest::testButtonCreation()
{
    QVERIFY(m_button != nullptr);
    QVERIFY(m_button->variant() == "primary");
    QVERIFY(m_button->state() == "default");
    QVERIFY(m_button->enabled() == true);
    QVERIFY(m_button->checked() == false);
}

void ButtonTest::testButtonVariants()
{
    // Test all variants
    QStringList variants = {"primary", "secondary", "tertiary", "icon", "toggle"};
    
    for (const QString &variant : variants) {
        m_button->setVariant(variant);
        QVERIFY(m_button->variant() == variant);
    }
}

void ButtonTest::testButtonStates()
{
    // Test all states
    QStringList states = {"default", "hover", "active", "disabled", "loading", "success", "error"};
    
    for (const QString &state : states) {
        m_button->setState(state);
        QVERIFY(m_button->state() == state);
    }
}

void ButtonTest::testButtonEnabled()
{
    m_button->setEnabled(true);
    QVERIFY(m_button->enabled() == true);
    
    m_button->setEnabled(false);
    QVERIFY(m_button->enabled() == false);
}

void ButtonTest::testButtonChecked()
{
    m_button->setVariant("toggle");
    m_button->setChecked(false);
    QVERIFY(m_button->checked() == false);
    
    m_button->setChecked(true);
    QVERIFY(m_button->checked() == true);
}

void ButtonTest::testButtonText()
{
    QString testText = "Test Button";
    m_button->setText(testText);
    QVERIFY(m_button->text() == testText);
}

QTEST_MAIN(ButtonTest)
#include "test_button.moc"

