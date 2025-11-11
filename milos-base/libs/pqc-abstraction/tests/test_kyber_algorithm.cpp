#include <QtTest>
#include <QObject>
#include <QString>
#include "../src/algorithms/kyber_algorithm.h"

/**
 * Unit tests for CRYSTALS-Kyber Algorithm
 */
class KyberAlgorithmTest : public QObject
{
    Q_OBJECT

public:
    KyberAlgorithmTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testKyber512Creation();
    void testKyber768Creation();
    void testKyber1024Creation();
    void testGetMetadata();
    void testSupportsCapability();
    void testGenerateKeyPair();
    void testEncapsulateDecapsulateKey();
    void testIsHealthy();

private:
    bool m_liboqsAvailable;
};

KyberAlgorithmTest::KyberAlgorithmTest()
    : m_liboqsAvailable(false)
{
}

void KyberAlgorithmTest::initTestCase()
{
#ifdef HAVE_LIBOQS
    m_liboqsAvailable = true;
#else
    m_liboqsAvailable = false;
    QSKIP("liboqs not available. Skipping Kyber algorithm tests.");
#endif
}

void KyberAlgorithmTest::cleanupTestCase()
{
}

void KyberAlgorithmTest::testKyber512Creation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber512");
    QVERIFY(kyber.isHealthy());
}

void KyberAlgorithmTest::testKyber768Creation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber768");
    QVERIFY(kyber.isHealthy());
}

void KyberAlgorithmTest::testKyber1024Creation()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber1024");
    QVERIFY(kyber.isHealthy());
}

void KyberAlgorithmTest::testGetMetadata()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber512");
    AlgorithmMetadata metadata = kyber.getMetadata();
    
    QCOMPARE(QString::fromStdString(metadata.algorithm_id), QString("Kyber512"));
    QCOMPARE(QString::fromStdString(metadata.name), QString("CRYSTALS-Kyber"));
    QVERIFY(metadata.is_nist_approved);
    QVERIFY(metadata.key_size > 0);
}

void KyberAlgorithmTest::testSupportsCapability()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber512");
    QVERIFY(kyber.supportsCapability(AlgorithmCapability::KEY_ENCAPSULATION));
    QVERIFY(!kyber.supportsCapability(AlgorithmCapability::DIGITAL_SIGNATURE));
}

void KyberAlgorithmTest::testGenerateKeyPair()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber512");
    KeyGenerationResult result = kyber.generateKeyPair();
    
    QVERIFY(result.success);
    QVERIFY(!result.public_key.empty());
    QVERIFY(!result.private_key.empty());
    QCOMPARE(QString::fromStdString(result.algorithm_used), QString("Kyber512"));
}

void KyberAlgorithmTest::testEncapsulateDecapsulateKey()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber512");
    
    // Generate key pair
    KeyGenerationResult keyResult = kyber.generateKeyPair();
    QVERIFY(keyResult.success);
    
    // Encapsulate key
    EncryptionResult encapResult = kyber.encapsulateKey(keyResult.public_key);
    QVERIFY(encapResult.success);
    QVERIFY(!encapResult.ciphertext.empty());
    QVERIFY(!encapResult.key.empty());
    
    // Decapsulate key
    DecryptionResult decapResult = kyber.decapsulateKey(encapResult.ciphertext, keyResult.private_key);
    QVERIFY(decapResult.success);
    QVERIFY(!decapResult.key.empty());
    
    // Verify shared secrets match
    QCOMPARE(encapResult.key, decapResult.key);
}

void KyberAlgorithmTest::testIsHealthy()
{
    if (!m_liboqsAvailable) {
        QSKIP("liboqs not available");
    }

    KyberAlgorithm kyber("Kyber512");
    QVERIFY(kyber.isHealthy());
}

QTEST_MAIN(KyberAlgorithmTest)
#include "test_kyber_algorithm.moc"

