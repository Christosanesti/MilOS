#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtCore/QUuid>
#include "services/encryptionmanagerservice.h"

class DBusEncryptionManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testEncryptFilesMethodRegistration();
    void testEncryptFilesValidInput();
    void testEncryptFilesTooManyFiles();
    void testEncryptFilesInvalidPath();
    void testEncryptFilesInvalidAlgorithm();
    void testEncryptFilesInvalidKey();
    void testEncryptFilesReturnsOperationId();
    void testBackwardCompatibility();
    void testEncryptFilesEmptyPathList();
    void testEncryptFilesSingleFile();

private:
    EncryptionManagerService *m_service;
    QTemporaryDir *m_tempDir;
    QStringList m_testFiles;
    QString m_testAlgorithm;
    QString m_testKeyId;
    
    void createTestFiles(int count);
    void cleanupTestFiles();
    bool isServiceRegistered();
};

void DBusEncryptionManagerTest::initTestCase()
{
    m_service = new EncryptionManagerService(this);
    m_tempDir = new QTemporaryDir();
    
    QVERIFY(m_tempDir->isValid());
    
    // Create test files
    createTestFiles(3);
    
    // Set up test parameters
    m_testAlgorithm = "CRYSTALS-Kyber";
    m_testKeyId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // Register service on D-Bus for testing
    QVERIFY(m_service->registerService("org.milos.EncryptionManager.Test",
                                       "/org/milos/EncryptionManager/Test"));
}

void DBusEncryptionManagerTest::cleanupTestCase()
{
    cleanupTestFiles();
    delete m_tempDir;
    delete m_service;
}

void DBusEncryptionManagerTest::createTestFiles(int count)
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

void DBusEncryptionManagerTest::cleanupTestFiles()
{
    for (const QString &filePath : m_testFiles) {
        QFile::remove(filePath);
    }
    m_testFiles.clear();
}

bool DBusEncryptionManagerTest::isServiceRegistered()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    return connection.isConnected();
}

void DBusEncryptionManagerTest::testEncryptFilesMethodRegistration()
{
    // Verify service is registered on D-Bus
    QVERIFY(isServiceRegistered());
    
    // Verify service object exists
    QVERIFY(m_service != nullptr);
    
    // Verify method exists (by checking meta-object)
    const QMetaObject *metaObj = m_service->metaObject();
    int methodIndex = metaObj->indexOfMethod("EncryptFiles(QStringList,QString,QString)");
    QVERIFY(methodIndex >= 0);
    
    // Verify EncryptFile method also exists (backward compatibility)
    int methodIndex2 = metaObj->indexOfMethod("EncryptFile(QString,QString,QString)");
    QVERIFY(methodIndex2 >= 0);
}

void DBusEncryptionManagerTest::testEncryptFilesValidInput()
{
    // Test with valid file paths (1-5 files)
    QStringList validPaths = m_testFiles.mid(0, 2); // Use first 2 files
    
    QString operationId = m_service->EncryptFiles(validPaths, m_testAlgorithm, m_testKeyId);
    
    // Verify operation ID is returned (non-empty UUID)
    QVERIFY(!operationId.isEmpty());
    
    // Verify operation ID is valid UUID format
    QUuid uuid(operationId);
    QVERIFY(!uuid.isNull());
    
    // Test with single file
    QStringList singlePath = QStringList() << m_testFiles.first();
    QString operationId2 = m_service->EncryptFiles(singlePath, m_testAlgorithm, m_testKeyId);
    QVERIFY(!operationId2.isEmpty());
    
    // Test with maximum files (5) - create temporary files
    QStringList maxPaths;
    for (int i = 0; i < 5; ++i) {
        QString fileName = QString("max_test_file_%1.txt").arg(i);
        QString filePath = m_tempDir->filePath(fileName);
        
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QString("Test content for max file %1").arg(i).toUtf8());
        file.close();
        
        maxPaths << filePath;
    }
    QString operationId3 = m_service->EncryptFiles(maxPaths, m_testAlgorithm, m_testKeyId);
    QVERIFY(!operationId3.isEmpty());
    
    // Clean up max test files
    for (const QString &path : maxPaths) {
        QFile::remove(path);
    }
}

void DBusEncryptionManagerTest::testEncryptFilesTooManyFiles()
{
    // Create 6 files (exceeds MAX_BATCH_FILES = 5)
    QStringList tooManyPaths;
    for (int i = 10; i < 16; ++i) {  // Use different file numbers to avoid conflicts
        QString fileName = QString("too_many_file_%1.txt").arg(i);
        QString filePath = m_tempDir->filePath(fileName);
        
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QString("Test content for file %1").arg(i).toUtf8());
        file.close();
        
        tooManyPaths << filePath;
    }
    
    // This should fail validation, but since we're testing the service directly
    // (not through D-Bus), we need to check the error handling
    // The service should return empty string or we should catch the error
    QString operationId = m_service->EncryptFiles(tooManyPaths, m_testAlgorithm, m_testKeyId);
    
    // Operation ID should be empty when validation fails
    // Note: In D-Bus, this would throw an error, but in direct call it returns empty
    QVERIFY(operationId.isEmpty());
    
    // Clean up
    for (const QString &path : tooManyPaths) {
        QFile::remove(path);
    }
}

void DBusEncryptionManagerTest::testEncryptFilesInvalidPath()
{
    // Test with non-existent file
    QStringList invalidPaths;
    invalidPaths << "/nonexistent/file/path.txt";
    
    QString operationId = m_service->EncryptFiles(invalidPaths, m_testAlgorithm, m_testKeyId);
    
    // Should return empty string when validation fails
    QVERIFY(operationId.isEmpty());
    
    // Test with empty path
    QStringList emptyPath;
    emptyPath << "";
    QString operationId2 = m_service->EncryptFiles(emptyPath, m_testAlgorithm, m_testKeyId);
    QVERIFY(operationId2.isEmpty());
}

void DBusEncryptionManagerTest::testEncryptFilesInvalidAlgorithm()
{
    // Test with empty algorithm
    QStringList validPaths = m_testFiles.mid(0, 1);
    
    QString operationId = m_service->EncryptFiles(validPaths, "", m_testKeyId);
    QVERIFY(operationId.isEmpty());
}

void DBusEncryptionManagerTest::testEncryptFilesInvalidKey()
{
    // Test with empty key_id
    QStringList validPaths = m_testFiles.mid(0, 1);
    
    QString operationId = m_service->EncryptFiles(validPaths, m_testAlgorithm, "");
    QVERIFY(operationId.isEmpty());
    
    // Test with invalid UUID format
    QString invalidKeyId = "not-a-valid-uuid";
    QString operationId2 = m_service->EncryptFiles(validPaths, m_testAlgorithm, invalidKeyId);
    QVERIFY(operationId2.isEmpty());
}

void DBusEncryptionManagerTest::testEncryptFilesReturnsOperationId()
{
    // Test that valid input returns a valid operation ID
    QStringList validPaths = m_testFiles.mid(0, 1);
    
    QString operationId = m_service->EncryptFiles(validPaths, m_testAlgorithm, m_testKeyId);
    
    // Verify operation ID is not empty
    QVERIFY(!operationId.isEmpty());
    
    // Verify operation ID is valid UUID format (without braces)
    QUuid uuid(operationId);
    QVERIFY(!uuid.isNull());
    
    // Verify operation ID format (no braces, as per implementation)
    QVERIFY(!operationId.startsWith('{'));
    QVERIFY(!operationId.endsWith('}'));
    
    // Verify each call returns unique operation ID
    QString operationId2 = m_service->EncryptFiles(validPaths, m_testAlgorithm, m_testKeyId);
    QVERIFY(operationId != operationId2);
}

void DBusEncryptionManagerTest::testBackwardCompatibility()
{
    // Test that existing EncryptFile method still works
    QString singlePath = m_testFiles.first();
    
    QString operationId = m_service->EncryptFile(singlePath, m_testAlgorithm, m_testKeyId);
    
    // Verify operation ID is returned
    QVERIFY(!operationId.isEmpty());
    
    // Verify operation ID is valid UUID
    QUuid uuid(operationId);
    QVERIFY(!uuid.isNull());
    
    // Test that invalid input to EncryptFile still fails appropriately
    QString invalidPath = "/nonexistent/file.txt";
    QString operationId2 = m_service->EncryptFile(invalidPath, m_testAlgorithm, m_testKeyId);
    QVERIFY(operationId2.isEmpty());
}

void DBusEncryptionManagerTest::testEncryptFilesEmptyPathList()
{
    // Test with empty path list
    QStringList emptyPaths;
    
    QString operationId = m_service->EncryptFiles(emptyPaths, m_testAlgorithm, m_testKeyId);
    
    // Should return empty string when validation fails
    QVERIFY(operationId.isEmpty());
}

void DBusEncryptionManagerTest::testEncryptFilesSingleFile()
{
    // Test batch method with single file (boundary case)
    QStringList singlePath = QStringList() << m_testFiles.first();
    
    QString operationId = m_service->EncryptFiles(singlePath, m_testAlgorithm, m_testKeyId);
    
    // Should succeed and return operation ID
    QVERIFY(!operationId.isEmpty());
    
    QUuid uuid(operationId);
    QVERIFY(!uuid.isNull());
}

QTEST_MAIN(DBusEncryptionManagerTest)
#include "dbus_encryptionmanager_test.moc"

