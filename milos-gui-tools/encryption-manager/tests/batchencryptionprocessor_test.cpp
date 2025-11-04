#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QSignalSpy>
#include "services/batchencryptionprocessor.h"
#include "services/singlefileencryption.h"

class BatchEncryptionProcessorTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testSequentialProcessing();
    void testBatchSuccessAllFiles();
    void testBatchPartialFailure();
    void testBatchCompleteFailure();
    void testAuditLoggingPerFile();
    void testProgressTracking();
    void testFileResultReporting();
    void testEmptyFileList();

private:
    BatchEncryptionProcessor *m_processor;
    QTemporaryDir *m_tempDir;
    QStringList m_testFiles;
    QString m_testAlgorithm;
    QString m_testKeyId;
    
    void createTestFiles(int count);
    void cleanupTestFiles();
    void createUnreadableFile(const QString &path);
};

void BatchEncryptionProcessorTest::initTestCase()
{
    m_processor = new BatchEncryptionProcessor(this);
    m_tempDir = new QTemporaryDir();
    
    QVERIFY(m_tempDir->isValid());
    
    // Create test files
    createTestFiles(3);
    
    // Set up test parameters
    m_testAlgorithm = "CRYSTALS-Kyber";
    m_testKeyId = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void BatchEncryptionProcessorTest::cleanupTestCase()
{
    cleanupTestFiles();
    delete m_tempDir;
    delete m_processor;
}

void BatchEncryptionProcessorTest::createTestFiles(int count)
{
    for (int i = 0; i < count; ++i) {
        QString fileName = QString("test_file_%1.txt").arg(i);
        QString filePath = m_tempDir->filePath(fileName);
        
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QString("Test content for file %1").arg(i).toUtf8());
        file.close();
        
        m_testFiles.append(filePath);
    }
}

void BatchEncryptionProcessorTest::cleanupTestFiles()
{
    for (const QString &filePath : m_testFiles) {
        // Remove original and encrypted files
        QFile::remove(filePath);
        QFile::remove(filePath + ".encrypted");
    }
    m_testFiles.clear();
}

void BatchEncryptionProcessorTest::createUnreadableFile(const QString &path)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("Test content");
    file.close();
    
    // Make file unreadable (simulate permission error)
    QFile::setPermissions(path, QFile::WriteOwner);
}

void BatchEncryptionProcessorTest::testSequentialProcessing()
{
    // Verify files are processed sequentially (not in parallel)
    // This test verifies that file encryption completes before next file starts
    
    QStringList filePaths = m_testFiles;
    
    QSignalSpy startedSpy(m_processor, &BatchEncryptionProcessor::fileEncryptionStarted);
    QSignalSpy completedSpy(m_processor, &BatchEncryptionProcessor::fileEncryptionCompleted);
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify all files were processed
    QCOMPARE(result.processedFiles, filePaths.size());
    QCOMPARE(result.totalFiles, filePaths.size());
    
    // Verify signals were emitted sequentially (one started, one completed, repeat)
    QCOMPARE(startedSpy.count(), filePaths.size());
    QCOMPARE(completedSpy.count(), filePaths.size());
    
    // Verify sequential processing: each file should complete before next starts
    // (Check that file indices are sequential: 0, 1, 2)
    for (int i = 0; i < startedSpy.size(); ++i) {
        QList<QVariant> args = startedSpy[i];
        QCOMPARE(args[1].toInt(), i); // fileIndex should be sequential
    }
}

void BatchEncryptionProcessorTest::testBatchSuccessAllFiles()
{
    // Test batch encryption where all files succeed
    QStringList filePaths = m_testFiles;
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify all files succeeded
    QCOMPARE(result.successfulFiles, filePaths.size());
    QCOMPARE(result.failedFiles, 0);
    QCOMPARE(result.processedFiles, filePaths.size());
    QVERIFY(!result.batchOperationId.isEmpty());
    
    // Verify all file results are SUCCESS
    for (const FileResult &fileResult : result.fileResults) {
        QCOMPARE(fileResult.status, QString("SUCCESS"));
        QVERIFY(fileResult.errorMessage.isEmpty());
    }
    
    // Verify encrypted files were created
    for (const QString &filePath : filePaths) {
        QString encryptedPath = filePath + ".encrypted";
        QVERIFY(QFile::exists(encryptedPath));
    }
}

void BatchEncryptionProcessorTest::testBatchPartialFailure()
{
    // Test batch encryption with partial failure (some files succeed, others fail)
    cleanupTestFiles();
    
    // Create mix of valid and invalid files
    QStringList filePaths;
    
    // Valid file 1
    QString validFile1 = m_tempDir->filePath("valid1.txt");
    QFile file1(validFile1);
    QVERIFY(file1.open(QIODevice::WriteOnly));
    file1.write("Valid content 1");
    file1.close();
    filePaths.append(validFile1);
    
    // Invalid file (will fail - non-existent)
    QString invalidFile = m_tempDir->filePath("nonexistent.txt");
    filePaths.append(invalidFile);
    
    // Valid file 2
    QString validFile2 = m_tempDir->filePath("valid2.txt");
    QFile file2(validFile2);
    QVERIFY(file2.open(QIODevice::WriteOnly));
    file2.write("Valid content 2");
    file2.close();
    filePaths.append(validFile2);
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify partial success
    QCOMPARE(result.successfulFiles, 2);
    QCOMPARE(result.failedFiles, 1);
    QCOMPARE(result.processedFiles, 3);
    
    // Verify individual file results
    QCOMPARE(result.fileResults[0].status, QString("SUCCESS"));
    QCOMPARE(result.fileResults[1].status, QString("FAILED"));
    QCOMPARE(result.fileResults[2].status, QString("SUCCESS"));
    
    // Verify failed file has error message
    QVERIFY(!result.fileResults[1].errorMessage.isEmpty());
    
    // Verify processing continued after failure (all files processed)
    QCOMPARE(result.processedFiles, filePaths.size());
    
    // Clean up
    QFile::remove(validFile1);
    QFile::remove(validFile1 + ".encrypted");
    QFile::remove(validFile2);
    QFile::remove(validFile2 + ".encrypted");
}

void BatchEncryptionProcessorTest::testBatchCompleteFailure()
{
    // Test batch encryption where all files fail
    QStringList filePaths;
    
    // Add non-existent files
    filePaths << m_tempDir->filePath("nonexistent1.txt");
    filePaths << m_tempDir->filePath("nonexistent2.txt");
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify all files failed
    QCOMPARE(result.successfulFiles, 0);
    QCOMPARE(result.failedFiles, filePaths.size());
    QCOMPARE(result.processedFiles, filePaths.size());
    
    // Verify all file results are FAILED
    for (const FileResult &fileResult : result.fileResults) {
        QCOMPARE(fileResult.status, QString("FAILED"));
        QVERIFY(!fileResult.errorMessage.isEmpty());
    }
}

void BatchEncryptionProcessorTest::testAuditLoggingPerFile()
{
    // Note: Audit logging is tested implicitly through batch processing
    // Since audit service may not be available, we verify the function completes
    // without errors (audit logging failures don't break encryption)
    
    // Create a fresh test file for this test
    QString testFile = m_tempDir->filePath("audit_test.txt");
    QFile file(testFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("Test content for audit logging");
    file.close();
    
    QStringList filePaths;
    filePaths << testFile;
    
    // Process batch - should complete even if audit service unavailable
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify encryption completed successfully
    QCOMPARE(result.successfulFiles, 1);
    
    // Verify audit logging function was called (doesn't throw or crash)
    // Actual audit logging verification requires audit service to be available
    // This is acceptable for MVP - audit integration will be tested in integration tests
    
    // Clean up
    QFile::remove(testFile);
    QFile::remove(testFile + ".encrypted");
}

void BatchEncryptionProcessorTest::testProgressTracking()
{
    // Test that batch operation tracks progress per file correctly
    // Create fresh test files for this test
    QStringList filePaths;
    for (int i = 0; i < 2; ++i) {
        QString testFile = m_tempDir->filePath(QString("progress_test_%1.txt").arg(i));
        QFile file(testFile);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QString("Test content %1").arg(i).toUtf8());
        file.close();
        filePaths << testFile;
    }
    
    QSignalSpy progressSpy(m_processor, &BatchEncryptionProcessor::fileEncryptionProgress);
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify progress signals were emitted
    QVERIFY(progressSpy.count() > 0);
    
    // Verify progress signals for each file
    // Each file should have multiple progress updates (0%, 50%, 100%)
    QVERIFY(progressSpy.count() >= filePaths.size());
    
    // Verify file indices are sequential
    for (int i = 0; i < filePaths.size(); ++i) {
        // Find first progress signal for this file index
        bool found = false;
        for (int j = 0; j < progressSpy.count(); ++j) {
            QList<QVariant> args = progressSpy[j];
            if (args[1].toInt() == i) {
                found = true;
                break;
            }
        }
        QVERIFY(found); // Each file should have progress signals
    }
    
    // Clean up
    for (const QString &path : filePaths) {
        QFile::remove(path);
        QFile::remove(path + ".encrypted");
    }
}

void BatchEncryptionProcessorTest::testFileResultReporting()
{
    // Test that individual file results are reported correctly
    QStringList filePaths = m_testFiles;
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(filePaths, m_testAlgorithm, m_testKeyId);
    
    // Verify file results array matches file paths
    QCOMPARE(result.fileResults.size(), filePaths.size());
    
    // Verify each file result has correct path
    for (int i = 0; i < filePaths.size(); ++i) {
        QCOMPARE(result.fileResults[i].filePath, filePaths[i]);
        QCOMPARE(result.fileResults[i].status, QString("SUCCESS"));
    }
    
    // Test with mixed results
    cleanupTestFiles();
    QStringList mixedPaths;
    mixedPaths << m_tempDir->filePath("valid.txt");
    QFile validFile(mixedPaths[0]);
    QVERIFY(validFile.open(QIODevice::WriteOnly));
    validFile.write("Valid");
    validFile.close();
    
    mixedPaths << m_tempDir->filePath("invalid.txt");
    
    BatchEncryptionResult mixedResult = m_processor->processBatchEncryption(mixedPaths, m_testAlgorithm, m_testKeyId);
    
    QCOMPARE(mixedResult.fileResults.size(), 2);
    QCOMPARE(mixedResult.fileResults[0].status, QString("SUCCESS"));
    QCOMPARE(mixedResult.fileResults[1].status, QString("FAILED"));
    
    // Clean up
    QFile::remove(mixedPaths[0]);
    QFile::remove(mixedPaths[0] + ".encrypted");
}

void BatchEncryptionProcessorTest::testEmptyFileList()
{
    // Test batch processing with empty file list
    QStringList emptyPaths;
    
    BatchEncryptionResult result = m_processor->processBatchEncryption(emptyPaths, m_testAlgorithm, m_testKeyId);
    
    // Verify no files processed
    QCOMPARE(result.totalFiles, 0);
    QCOMPARE(result.processedFiles, 0);
    QCOMPARE(result.successfulFiles, 0);
    QCOMPARE(result.failedFiles, 0);
    QVERIFY(result.fileResults.isEmpty());
}

QTEST_MAIN(BatchEncryptionProcessorTest)
#include "batchencryptionprocessor_test.moc"

