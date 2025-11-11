#include <QtTest>
#include <QObject>
#include <QString>
#include <QTemporaryFile>
#include <QFile>
#include "../include/milos/encryption/file_encryption.h"

/**
 * Unit tests for File Encryption Library
 */
class FileEncryptionTest : public QObject
{
    Q_OBJECT

public:
    FileEncryptionTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialize();
    void testEncryptDecryptData();
    void testEncryptDecryptFile();

private:
    FileEncryption* m_fileEncryption;
};

FileEncryptionTest::FileEncryptionTest()
    : m_fileEncryption(nullptr)
{
}

void FileEncryptionTest::initTestCase()
{
    m_fileEncryption = new FileEncryption();
}

void FileEncryptionTest::cleanupTestCase()
{
    delete m_fileEncryption;
}

void FileEncryptionTest::testInitialize()
{
    bool success = m_fileEncryption->initialize("Kyber512");
    QVERIFY(success);
    QVERIFY(m_fileEncryption->isInitialized());
}

void FileEncryptionTest::testEncryptDecryptData()
{
    if (!m_fileEncryption->isInitialized()) {
        QVERIFY(m_fileEncryption->initialize());
    }

    std::vector<uint8_t> plaintext = {1, 2, 3, 4, 5};
    std::vector<uint8_t> encrypted = m_fileEncryption->encryptData(plaintext);
    
    QVERIFY(!encrypted.empty());
    
    std::vector<uint8_t> decrypted = m_fileEncryption->decryptData(encrypted);
    QVERIFY(!decrypted.empty());
    QCOMPARE(decrypted, plaintext);
}

void FileEncryptionTest::testEncryptDecryptFile()
{
    if (!m_fileEncryption->isInitialized()) {
        QVERIFY(m_fileEncryption->initialize());
    }

    // Create temporary files
    QTemporaryFile inputFile;
    QTemporaryFile encryptedFile;
    QTemporaryFile decryptedFile;

    QVERIFY(inputFile.open());
    QVERIFY(encryptedFile.open());
    QVERIFY(decryptedFile.open());

    QString inputPath = inputFile.fileName();
    QString encryptedPath = encryptedFile.fileName();
    QString decryptedPath = decryptedFile.fileName();

    inputFile.close();
    encryptedFile.close();
    decryptedFile.close();

    // Write test data
    QFile input(inputPath);
    QVERIFY(input.open(QIODevice::WriteOnly));
    input.write("Test file encryption data");
    input.close();

    // Encrypt file
    FileEncryptionResult encResult = m_fileEncryption->encryptFile(
        inputPath.toStdString(),
        encryptedPath.toStdString()
    );
    QVERIFY(encResult.success);

    // Decrypt file
    FileDecryptionResult decResult = m_fileEncryption->decryptFile(
        encryptedPath.toStdString(),
        decryptedPath.toStdString()
    );
    QVERIFY(decResult.success);

    // Verify decrypted content
    QFile decrypted(decryptedPath);
    QVERIFY(decrypted.open(QIODevice::ReadOnly));
    QByteArray decryptedData = decrypted.readAll();
    QCOMPARE(decryptedData, QByteArray("Test file encryption data"));
}

QTEST_MAIN(FileEncryptionTest)
#include "test_file_encryption.moc"

