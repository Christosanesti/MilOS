#include <QtTest>
#include <QObject>
#include <QString>
#include "../src/algorithms/dilithium_algorithm.h"

/**
 * Unit tests for CRYSTALS-Dilithium Algorithm
 */
class DilithiumAlgorithmTest : public QObject
{
    Q_OBJECT

public:
    DilithiumAlgorithmTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testDilithium2Creation();
    void testDilithium3Creation();
    void testDilithium5Creation();
    void testGetMetadata();
    void testSupportsCapability();
    void testGenerateKeyPair();
    void testSignVerify();
    void testIsHealthy();

private:
    bool m_liboqsAvailable;
};

DilithiumAlgorithmTest::DilithiumAlgorithmTest()
    : m_liboqsAvailable(false)
{
}

void DilithiumAlgorithmTest::initTestCase()
{
#ifdef HAVE_LIBOQS
    m_liboqsAvailable = true;
#else
    m_liboqsAvailable = false;
    QSKIP("liboqs not available. Skipping Dilithium algorithm tests.");
#endif
}

void DilithiumAlgorithmTest::cleanupTestCase()
{
}

void DilithiumAlgorithmTest::testDilithium2Creation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium2");
    QVERIFY(dilithium.isHealthy());
}

void DilithiumAlgorithmTest::testDilithium3Creation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium3");
    QVERIFY(dilithium.isHealthy());
}

void DilithiumAlgorithmTest::testDilithium5Creation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium5");
    QVERIFY(dilithium.isHealthy());
}

void DilithiumAlgorithmTest::testGetMetadata()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium2");
    AlgorithmMetadata metadata = dilithium.getMetadata();
    
    QCOMPARE(QString::fromStdString(metadata.algorithm_id), QString("Dilithium2"));
    QCOMPARE(QString::fromStdString(metadata.name), QString("CRYSTALS-Dilithium"));
    QVERIFY(metadata.is_nist_approved);
    QVERIFY(metadata.key_size > 0);
}

void DilithiumAlgorithmTest::testSupportsCapability()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium2");
    QVERIFY(dilithium.supportsCapability(AlgorithmCapability::DIGITAL_SIGNATURE));
    QVERIFY(!dilithium.supportsCapability(AlgorithmCapability::KEY_ENCAPSULATION));
}

void DilithiumAlgorithmTest::testGenerateKeyPair()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium2");
    KeyGenerationResult result = dilithium.generateKeyPair();
    
    QVERIFY(result.success);
    QVERIFY(!result.public_key.empty());
    QVERIFY(!result.private_key.empty());
    QCOMPARE(QString::fromStdString(result.algorithm_used), QString("Dilithium2"));
}

void DilithiumAlgorithmTest::testSignVerify()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium2");
    
    // Generate key pair
    KeyGenerationResult keyResult = dilithium.generateKeyPair();
    QVERIFY(keyResult.success);
    
    // Test data to sign
    std::vector<uint8_t> testData = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64}; // "Hello World"
    
    // Sign data
    SignatureResult signResult = dilithium.sign(testData, keyResult.private_key);
    QVERIFY(signResult.success);
    QVERIFY(!signResult.signature.empty());
    
    // Verify signature
    VerificationResult verifyResult = dilithium.verify(testData, signResult.signature, keyResult.public_key);
    QVERIFY(verifyResult.success);
    QVERIFY(verifyResult.is_valid);
}

void DilithiumAlgorithmTest::testIsHealthy()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    DilithiumAlgorithm dilithium("Dilithium2");
    QVERIFY(dilithium.isHealthy());
}

QTEST_MAIN(DilithiumAlgorithmTest)
#include "test_dilithium_algorithm.moc"

