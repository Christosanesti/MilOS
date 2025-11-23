#include <QtTest>
#include <QObject>
#include <QString>
#include "../include/milos/encryption/operation_manager.h"

/**
 * Unit tests for Encryption Operation Manager
 */
class OperationManagerTest : public QObject
{
    Q_OBJECT

public:
    OperationManagerTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testGetInstance();
    void testStartOperation();
    void testUpdateProgress();
    void testCompleteOperation();
    void testCancelOperation();
    void testGetOperationInfo();
    void testMultipleOperations();

private:
    EncryptionOperationManager* m_manager;
};

OperationManagerTest::OperationManagerTest()
    : m_manager(nullptr)
{
}

void OperationManagerTest::initTestCase()
{
    m_manager = &EncryptionOperationManager::getInstance();
}

void OperationManagerTest::cleanupTestCase()
{
    // Manager is singleton, no cleanup needed
}

void OperationManagerTest::testGetInstance()
{
    EncryptionOperationManager& instance1 = EncryptionOperationManager::getInstance();
    EncryptionOperationManager& instance2 = EncryptionOperationManager::getInstance();
    
    // Should return same instance
    QVERIFY(&instance1 == &instance2);
}

void OperationManagerTest::testStartOperation()
{
    std::string operationId = m_manager->startOperation(
        OperationType::FILE_ENCRYPTION,
        "Test encryption operation"
    );
    
    QVERIFY(!operationId.empty());
    
    OperationInfo info = m_manager->getOperationInfo(operationId);
    QCOMPARE(QString::fromStdString(info.operation_id), QString::fromStdString(operationId));
    QVERIFY(info.type == OperationType::FILE_ENCRYPTION);
    QVERIFY(info.status == OperationStatus::PENDING);
}

void OperationManagerTest::testUpdateProgress()
{
    std::string operationId = m_manager->startOperation(
        OperationType::FILE_ENCRYPTION,
        "Test progress update"
    );
    
    m_manager->updateProgress(operationId, 50, 100);
    
    OperationInfo info = m_manager->getOperationInfo(operationId);
    QVERIFY(info.status == OperationStatus::RUNNING);
    QVERIFY(info.bytes_processed == 50);
    QVERIFY(info.total_bytes == 100);
    QVERIFY(qAbs(info.progress_percentage - 50.0) < 0.1);
}

void OperationManagerTest::testCompleteOperation()
{
    std::string operationId = m_manager->startOperation(
        OperationType::FILE_ENCRYPTION,
        "Test completion"
    );
    
    m_manager->completeOperation(operationId, "/path/to/result");
    
    OperationInfo info = m_manager->getOperationInfo(operationId);
    QVERIFY(info.status == OperationStatus::COMPLETED);
    QVERIFY(qAbs(info.progress_percentage - 100.0) < 0.1);
    QCOMPARE(QString::fromStdString(info.result_path), QString("/path/to/result"));
}

void OperationManagerTest::testCancelOperation()
{
    std::string operationId = m_manager->startOperation(
        OperationType::FILE_ENCRYPTION,
        "Test cancellation"
    );
    
    bool cancelled = m_manager->cancelOperation(operationId);
    QVERIFY(cancelled);
    
    OperationInfo info = m_manager->getOperationInfo(operationId);
    QVERIFY(info.status == OperationStatus::CANCELLED);
}

void OperationManagerTest::testGetOperationInfo()
{
    std::string operationId = m_manager->startOperation(
        OperationType::KEY_DERIVATION,
        "Test get info"
    );
    
    OperationInfo info = m_manager->getOperationInfo(operationId);
    QCOMPARE(QString::fromStdString(info.operation_id), QString::fromStdString(operationId));
    QVERIFY(info.type == OperationType::KEY_DERIVATION);
}

void OperationManagerTest::testMultipleOperations()
{
    std::string op1 = m_manager->startOperation(OperationType::FILE_ENCRYPTION, "Operation 1");
    std::string op2 = m_manager->startOperation(OperationType::FILE_DECRYPTION, "Operation 2");
    std::string op3 = m_manager->startOperation(OperationType::DISK_ENCRYPTION, "Operation 3");
    
    QVERIFY(!op1.empty());
    QVERIFY(!op2.empty());
    QVERIFY(!op3.empty());
    QVERIFY(op1 != op2);
    QVERIFY(op2 != op3);
    
    // All should be independent
    m_manager->updateProgress(op1, 25, 100);
    m_manager->updateProgress(op2, 50, 100);
    m_manager->updateProgress(op3, 75, 100);
    
    OperationInfo info1 = m_manager->getOperationInfo(op1);
    OperationInfo info2 = m_manager->getOperationInfo(op2);
    OperationInfo info3 = m_manager->getOperationInfo(op3);
    
    QVERIFY(qAbs(info1.progress_percentage - 25.0) < 0.1);
    QVERIFY(qAbs(info2.progress_percentage - 50.0) < 0.1);
    QVERIFY(qAbs(info3.progress_percentage - 75.0) < 0.1);
}

QTEST_MAIN(OperationManagerTest)
#include "test_operation_manager.moc"

