#include <QtTest>
#include <QObject>
#include <QString>
#include "../include/milos/encryption/disk_encryption.h"

/**
 * Unit tests for Disk Encryption Library
 * 
 * Note: These tests may require root privileges and actual disk devices
 * for full functionality. Some tests may be skipped if cryptsetup is not available.
 */
class DiskEncryptionTest : public QObject
{
    Q_OBJECT

public:
    DiskEncryptionTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialize();
    void testKeyDerivationIntegration();
    void testLUKS2Availability();

private:
    DiskEncryption* m_diskEncryption;
    bool m_cryptsetupAvailable;
};

DiskEncryptionTest::DiskEncryptionTest()
    : m_diskEncryption(nullptr)
    , m_cryptsetupAvailable(false)
{
}

void DiskEncryptionTest::initTestCase()
{
    m_diskEncryption = new DiskEncryption();
    
#ifdef HAVE_CRYPTSETUP
    m_cryptsetupAvailable = true;
#else
    m_cryptsetupAvailable = false;
    QSKIP("cryptsetup not available. Skipping disk encryption tests.");
#endif
}

void DiskEncryptionTest::cleanupTestCase()
{
    delete m_diskEncryption;
}

void DiskEncryptionTest::testInitialize()
{
    if (!m_cryptsetupAvailable) {
        QSKIP("cryptsetup not available");
    }
    
    bool success = m_diskEncryption->initialize("Kyber512");
    QVERIFY(success);
    QVERIFY(m_diskEncryption->isInitialized());
}

void DiskEncryptionTest::testKeyDerivationIntegration()
{
    if (!m_cryptsetupAvailable) {
        QSKIP("cryptsetup not available");
    }
    
    // Test that disk encryption can initialize with key derivation
    bool success = m_diskEncryption->initialize("Kyber512");
    QVERIFY(success);
    
    // Key derivation should be integrated (tested via formatDevice would require root)
    // This is a basic integration test
    QVERIFY(m_diskEncryption->isInitialized());
}

void DiskEncryptionTest::testLUKS2Availability()
{
    // Test that LUKS2 support is detected
    // Actual LUKS2 operations require root and real devices
    // This test just verifies the library can be initialized
    
    if (!m_cryptsetupAvailable) {
        QSKIP("cryptsetup not available");
    }
    
    bool success = m_diskEncryption->initialize("Kyber512");
    QVERIFY(success);
}

QTEST_MAIN(DiskEncryptionTest)
#include "test_disk_encryption.moc"

