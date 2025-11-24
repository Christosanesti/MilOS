#include <QtTest>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include "../../src/services/installerstate.h"

/**
 * @brief Test suite for Installer State Management
 * 
 * Tests installation state persistence and recovery.
 */
class InstallerStateTests : public QObject
{
    Q_OBJECT

public:
    InstallerStateTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testStatePersistence();
    void testStateRecovery();
    void testCheckpointSystem();
    void testStateValidation();

private:
    QString m_testStateFile;
    InstallerState* m_installerState;
};

InstallerStateTests::InstallerStateTests()
    : m_installerState(nullptr)
{
    m_testStateFile = QDir::tempPath() + "/milos-installer-test-state.json";
}

void InstallerStateTests::initTestCase()
{
    // Clean up any existing test state file
    if (QFile::exists(m_testStateFile)) {
        QFile::remove(m_testStateFile);
    }
    
    m_installerState = new InstallerState(this);
    m_installerState->setStateFile(m_testStateFile);
}

void InstallerStateTests::cleanupTestCase()
{
    if (m_installerState) {
        delete m_installerState;
        m_installerState = nullptr;
    }
    
    // Clean up test state file
    if (QFile::exists(m_testStateFile)) {
        QFile::remove(m_testStateFile);
    }
}

void InstallerStateTests::testStatePersistence()
{
    QVERIFY(m_installerState != nullptr);
    
    // Set test state
    m_installerState->setCurrentStep(1);
    m_installerState->setLanguage("en_US");
    
    // Save state
    bool saved = m_installerState->saveState();
    QVERIFY(saved);
    
    // Verify file exists
    QVERIFY(QFile::exists(m_testStateFile));
}

void InstallerStateTests::testStateRecovery()
{
    QVERIFY(m_installerState != nullptr);
    
    // Load state
    bool loaded = m_installerState->loadState();
    QVERIFY(loaded);
    
    // Verify state values
    QCOMPARE(m_installerState->currentStep(), 1);
    QCOMPARE(m_installerState->language(), QString("en_US"));
}

void InstallerStateTests::testCheckpointSystem()
{
    QVERIFY(m_installerState != nullptr);
    
    // Create checkpoint
    m_installerState->setCurrentStep(2);
    bool checkpointed = m_installerState->saveState();
    QVERIFY(checkpointed);
    
    // Verify checkpoint saved
    QVERIFY(QFile::exists(m_testStateFile));
}

void InstallerStateTests::testStateValidation()
{
    QVERIFY(m_installerState != nullptr);
    
    // Test state validation
    // State should be valid if file exists and contains valid JSON
    bool valid = m_installerState->loadState();
    QVERIFY(valid || !QFile::exists(m_testStateFile));
}

QTEST_MAIN(InstallerStateTests)
#include "test_installerstate.moc"

