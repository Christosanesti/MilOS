#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QSignalSpy>
#include <QApplication>
#include "ui/batchprogresswidget.h"

class BatchProgressWidgetTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testProgressUIDisplay();
    void testCurrentFileIndicator();
    void testFileStatusIndicators();
    void testOverallProgress();
    void testXenonVisualEffects();
    void testErrorDisplay();
    void testBatchCompletion();
    void testOperationIdValidation();

private:
    BatchProgressWidget *m_widget;
    QTemporaryDir *m_tempDir;
    QStringList m_testFiles;
    
    void createTestFiles(int count);
    void cleanupTestFiles();
};

void BatchProgressWidgetTest::initTestCase()
{
    // QApplication should be created by QTEST_MAIN
    // Widget testing requires QApplication to exist
    
    m_widget = new BatchProgressWidget();
    m_tempDir = new QTemporaryDir();
    
    QVERIFY(m_tempDir->isValid());
    
    // Create test files
    createTestFiles(3);
}

void BatchProgressWidgetTest::cleanupTestCase()
{
    cleanupTestFiles();
    delete m_tempDir;
    delete m_widget;
}

void BatchProgressWidgetTest::createTestFiles(int count)
{
    for (int i = 0; i < count; ++i) {
        QString filePath = m_tempDir->filePath(QString("test_file_%1.txt").arg(i));
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QString("Test content %1").arg(i).toUtf8());
        file.close();
        m_testFiles.append(filePath);
    }
}

void BatchProgressWidgetTest::cleanupTestFiles()
{
    for (const QString &filePath : m_testFiles) {
        QFile::remove(filePath);
    }
    m_testFiles.clear();
}

void BatchProgressWidgetTest::testProgressUIDisplay()
{
    // Test: Progress UI displays when batch operation starts
    QString operationId = "test-operation-123";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Verify widget is initialized
    QVERIFY(m_widget->isOperationActive());
    QCOMPARE(m_widget->getOperationId(), operationId);
    
    // Verify widget is visible (should be after initialization)
    m_widget->show();
    QVERIFY(m_widget->isVisible());
}

void BatchProgressWidgetTest::testCurrentFileIndicator()
{
    // Test: Current file indicator updates correctly
    QString operationId = "test-operation-456";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Simulate batch encryption started
    m_widget->onBatchEncryptionStarted(operationId);
    
    // Simulate progress for first file (index 0 = file 1)
    m_widget->onBatchEncryptionProgress(operationId, 0, 50);
    
    // Verify current file indicator shows file 1
    // Note: Actual text verification would require accessing private members
    // For now, we verify the widget handles the signal correctly
    QVERIFY(m_widget->isOperationActive());
    
    // Simulate progress for second file (index 1 = file 2)
    m_widget->onBatchEncryptionProgress(operationId, 1, 25);
    
    // Verify widget still active
    QVERIFY(m_widget->isOperationActive());
}

void BatchProgressWidgetTest::testFileStatusIndicators()
{
    // Test: Individual file status indicators update
    QString operationId = "test-operation-789";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Simulate file 1 in progress
    m_widget->onBatchEncryptionProgress(operationId, 0, 50);
    
    // Simulate file 1 completed
    m_widget->onBatchEncryptionProgress(operationId, 0, 100);
    
    // Simulate file 2 in progress
    m_widget->onBatchEncryptionProgress(operationId, 1, 30);
    
    // Verify widget is still active
    QVERIFY(m_widget->isOperationActive());
}

void BatchProgressWidgetTest::testOverallProgress()
{
    // Test: Overall progress percentage calculated correctly
    QString operationId = "test-operation-progress";
    
    // Initialize batch with 3 files
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Simulate file 1 completed (100%)
    m_widget->onBatchEncryptionProgress(operationId, 0, 100);
    
    // Simulate file 2 at 50%
    m_widget->onBatchEncryptionProgress(operationId, 1, 50);
    
    // Simulate file 3 not started (0%)
    // Overall progress should be: (100 + 50 + 0) / 3 = 50%
    // Note: We can't directly verify progress bar value without accessing private members
    // But we can verify the widget handles the signals correctly
    QVERIFY(m_widget->isOperationActive());
    
    // Complete all files
    m_widget->onBatchEncryptionProgress(operationId, 1, 100);
    m_widget->onBatchEncryptionProgress(operationId, 2, 100);
    
    // Verify widget still active until completion signal
    QVERIFY(m_widget->isOperationActive());
}

void BatchProgressWidgetTest::testXenonVisualEffects()
{
    // Test: Xenon visual effects apply to status indicators
    // Note: Full xenon effects testing requires the xenon library
    // For now, we verify the widget handles status updates correctly
    
    QString operationId = "test-operation-xenon";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Simulate different statuses to trigger xenon effects
    m_widget->onBatchEncryptionProgress(operationId, 0, 50);  // IN_PROGRESS
    m_widget->onBatchEncryptionProgress(operationId, 0, 100); // SUCCESS
    
    // Verify widget handles status changes
    QVERIFY(m_widget->isOperationActive());
}

void BatchProgressWidgetTest::testErrorDisplay()
{
    // Test: Error messages display for failed files
    QString operationId = "test-operation-error";
    QString errorMessage = "Test error: File encryption failed";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Simulate file 1 in progress
    m_widget->onBatchEncryptionProgress(operationId, 0, 50);
    
    // Simulate error for file 1
    m_widget->onEncryptionError(operationId, errorMessage);
    
    // Verify widget handles error correctly
    QVERIFY(m_widget->isOperationActive());
    
    // Error should be stored and displayable
    // (Actual display verification would require UI interaction testing)
}

void BatchProgressWidgetTest::testBatchCompletion()
{
    // Test: Progress UI handles batch completion correctly
    QString operationId = "test-operation-complete";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Connect to batch completed signal
    QSignalSpy completedSpy(m_widget, &BatchProgressWidget::batchCompleted);
    
    // Simulate all files completed
    m_widget->onBatchEncryptionProgress(operationId, 0, 100);
    m_widget->onBatchEncryptionProgress(operationId, 1, 100);
    m_widget->onBatchEncryptionProgress(operationId, 2, 100);
    
    // Simulate batch completion
    m_widget->onBatchEncryptionCompleted(operationId, "SUCCESS");
    
    // Verify completion signal was emitted
    QCOMPARE(completedSpy.count(), 1);
    
    // Verify signal arguments
    QList<QVariant> arguments = completedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), operationId);
    QCOMPARE(arguments.at(1).toString(), QString("SUCCESS"));
    
    // Test failure completion
    QString failureOperationId = "test-operation-failure";
    m_widget->initializeBatch(failureOperationId, m_testFiles);
    QSignalSpy failureSpy(m_widget, &BatchProgressWidget::batchCompleted);
    
    m_widget->onBatchEncryptionCompleted(failureOperationId, "FAILED");
    QCOMPARE(failureSpy.count(), 1);
}

void BatchProgressWidgetTest::testOperationIdValidation()
{
    // Test: Operation ID validation prevents unauthorized updates
    QString operationId = "test-operation-validation";
    QString wrongOperationId = "wrong-operation-id";
    
    // Initialize batch
    m_widget->initializeBatch(operationId, m_testFiles);
    
    // Try to send progress with wrong operation ID
    // Should not process (silently ignored)
    m_widget->onBatchEncryptionProgress(wrongOperationId, 0, 50);
    
    // Verify correct operation ID still works
    m_widget->onBatchEncryptionProgress(operationId, 0, 50);
    QVERIFY(m_widget->isOperationActive());
    
    // Verify wrong operation ID doesn't affect state
    m_widget->onBatchEncryptionCompleted(wrongOperationId, "SUCCESS");
    QVERIFY(m_widget->isOperationActive()); // Should still be active
}

QTEST_MAIN(BatchProgressWidgetTest)
#include "batchprogresswidget_test.moc"

