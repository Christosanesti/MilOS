#include <QtTest>
#include <QObject>
#include <QString>
#include "../src/algorithms/sphincs_algorithm.h"

/**
 * Unit tests for SPHINCS+ Algorithm
 */
class SPHINCSAlgorithmTest : public QObject
{
    Q_OBJECT

public:
    SPHINCSAlgorithmTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testSPHINCS128fCreation();
    void testSPHINCS128sCreation();
    void testSPHINCS192fCreation();
    void testSPHINCS256fCreation();
    void testGetMetadata();
    void testSupportsCapability();
    void testGenerateKeyPair();
    void testSignVerify();
    void testIsHealthy();

private:
    bool m_liboqsAvailable;
};

SPHINCSAlgorithmTest::SPHINCSAlgorithmTest()
    : m_liboqsAvailable(false)
{
}

void SPHINCSAlgorithmTest::initTestCase()
{
#ifdef HAVE_LIBOQS
    m_liboqsAvailable = true;
#else
    m_liboqsAvailable = false;
    QSKIP("liboqs not available. Skipping SPHINCS+ algorithm tests.");
#endif
}

void SPHINCSAlgorithmTest::cleanupTestCase()
{
}

void SPHINCSAlgorithmTest::testSPHINCS128fCreation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128f-simple");
    QVERIFY(sphincs.isHealthy());
}

void SPHINCSAlgorithmTest::testSPHINCS128sCreation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128s-simple");
    QVERIFY(sphincs.isHealthy());
}

void SPHINCSAlgorithmTest::testSPHINCS192fCreation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-192f-simple");
    QVERIFY(sphincs.isHealthy());
}

void SPHINCSAlgorithmTest::testSPHINCS256fCreation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-256f-simple");
    QVERIFY(sphincs.isHealthy());
}

void SPHINCSAlgorithmTest::testGetMetadata()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128f-simple");
    AlgorithmMetadata metadata = sphincs.getMetadata();
    
    QCOMPARE(QString::fromStdString(metadata.algorithm_id), QString("SPHINCS+-SHA256-128f-simple"));
    QCOMPARE(QString::fromStdString(metadata.name), QString("SPHINCS+"));
    QVERIFY(metadata.is_nist_approved);
    QVERIFY(metadata.key_size > 0);
}

void SPHINCSAlgorithmTest::testSupportsCapability()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128f-simple");
    QVERIFY(sphincs.supportsCapability(AlgorithmCapability::DIGITAL_SIGNATURE));
    QVERIFY(!sphincs.supportsCapability(AlgorithmCapability::KEY_ENCAPSULATION));
}

void SPHINCSAlgorithmTest::testGenerateKeyPair()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128f-simple");
    KeyGenerationResult result = sphincs.generateKeyPair();
    
    QVERIFY(result.success);
    QVERIFY(!result.public_key.empty());
    QVERIFY(!result.private_key.empty());
    QCOMPARE(QString::fromStdString(result.algorithm_used), QString("SPHINCS+-SHA256-128f-simple"));
}

void SPHINCSAlgorithmTest::testSignVerify()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128f-simple");
    
    // Generate key pair
    KeyGenerationResult keyResult = sphincs.generateKeyPair();
    QVERIFY(keyResult.success);
    
    // Test data to sign
    std::vector<uint8_t> testData = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64}; // "Hello World"
    
    // Sign data
    SignatureResult signResult = sphincs.sign(testData, keyResult.private_key);
    QVERIFY(signResult.success);
    QVERIFY(!signResult.signature.empty());
    
    // Verify signature
    VerificationResult verifyResult = sphincs.verify(testData, signResult.signature, keyResult.public_key);
    QVERIFY(verifyResult.success);
    QVERIFY(verifyResult.is_valid);
}

void SPHINCSAlgorithmTest::testIsHealthy()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    SPHINCSAlgorithm sphincs("SPHINCS+-SHA256-128f-simple");
    QVERIFY(sphincs.isHealthy());
}

QTEST_MAIN(SPHINCSAlgorithmTest)
#include "test_sphincs_algorithm.moc"

