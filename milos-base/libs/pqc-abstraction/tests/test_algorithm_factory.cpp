#include <QtTest>
#include <QObject>
#include <QString>
#include "../include/milos/pqc/algorithm_factory.h"
#include "../include/milos/pqc/algorithm_interface.h"

/**
 * Mock algorithm for testing
 */
class MockAlgorithm : public IAlgorithm {
public:
    MockAlgorithm(const std::string& id) : m_id(id) {}

    AlgorithmMetadata getMetadata() const override {
        AlgorithmMetadata metadata;
        metadata.algorithm_id = m_id;
        metadata.name = "Mock Algorithm";
        metadata.version = "1.0.0";
        metadata.description = "Mock algorithm for testing";
        metadata.capabilities.push_back(AlgorithmCapability::KEY_ENCAPSULATION);
        metadata.is_nist_approved = false;
        metadata.is_hybrid_supported = true;
        metadata.key_size = 32;
        metadata.ciphertext_overhead = 16;
        metadata.performance_score = 1.0;
        return metadata;
    }

    bool supportsCapability(AlgorithmCapability capability) const override {
        return capability == AlgorithmCapability::KEY_ENCAPSULATION;
    }

    KeyGenerationResult generateKeyPair() override {
        KeyGenerationResult result;
        result.success = true;
        result.public_key = {1, 2, 3};
        result.private_key = {4, 5, 6};
        result.algorithm_used = m_id;
        result.algorithm_version = "1.0.0";
        return result;
    }

    EncryptionResult encapsulateKey(const std::vector<uint8_t>& publicKey) override {
        EncryptionResult result;
        result.success = true;
        result.ciphertext = {7, 8, 9};
        result.key = {10, 11, 12};
        result.algorithm_used = m_id;
        result.algorithm_version = "1.0.0";
        return result;
    }

    DecryptionResult decapsulateKey(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& privateKey
    ) override {
        DecryptionResult result;
        result.success = true;
        result.plaintext = {};
        result.key = {10, 11, 12};
        result.algorithm_used = m_id;
        result.algorithm_version = "1.0.0";
        return result;
    }

    EncryptionResult encrypt(
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& publicKey
    ) override {
        EncryptionResult result;
        result.success = true;
        result.ciphertext = plaintext;  // Mock: just copy
        result.algorithm_used = m_id;
        result.algorithm_version = "1.0.0";
        return result;
    }

    DecryptionResult decrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& privateKey
    ) override {
        DecryptionResult result;
        result.success = true;
        result.plaintext = ciphertext;  // Mock: just copy
        result.algorithm_used = m_id;
        result.algorithm_version = "1.0.0";
        return result;
    }

    SignatureResult sign(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& privateKey
    ) override {
        SignatureResult result;
        result.success = false;
        result.error_message = "Not supported";
        return result;
    }

    VerificationResult verify(
        const std::vector<uint8_t>& data,
        const std::vector<uint8_t>& signature,
        const std::vector<uint8_t>& publicKey
    ) override {
        VerificationResult result;
        result.success = false;
        result.verified = false;
        result.error_message = "Not supported";
        return result;
    }

    bool isHealthy() const override {
        return true;
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

private:
    std::string m_id;
};

/**
 * Unit tests for Algorithm Factory
 */
class AlgorithmFactoryTest : public QObject
{
    Q_OBJECT

public:
    AlgorithmFactoryTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testRegisterAlgorithm();
    void testCreateAlgorithm();
    void testGetRegisteredAlgorithms();
    void testIsAlgorithmRegistered();
    void testUnregisterAlgorithm();

private:
    AlgorithmFactory* m_factory;
};

AlgorithmFactoryTest::AlgorithmFactoryTest()
    : m_factory(nullptr)
{
}

void AlgorithmFactoryTest::initTestCase()
{
    m_factory = &AlgorithmFactory::getInstance();
}

void AlgorithmFactoryTest::cleanupTestCase()
{
}

void AlgorithmFactoryTest::testRegisterAlgorithm()
{
    // Register mock algorithm
    bool success = m_factory->registerAlgorithm("mock-algorithm", []() {
        return std::make_unique<MockAlgorithm>("mock-algorithm");
    });
    QVERIFY(success);

    // Try to register again (should fail)
    bool success2 = m_factory->registerAlgorithm("mock-algorithm", []() {
        return std::make_unique<MockAlgorithm>("mock-algorithm");
    });
    QVERIFY(!success2);
}

void AlgorithmFactoryTest::testCreateAlgorithm()
{
    // Create algorithm instance
    auto algorithm = m_factory->createAlgorithm("mock-algorithm");
    QVERIFY(algorithm != nullptr);

    // Verify algorithm works
    auto metadata = algorithm->getMetadata();
    QCOMPARE(QString::fromStdString(metadata.algorithm_id), QString("mock-algorithm"));

    // Try to create non-existent algorithm
    auto algorithm2 = m_factory->createAlgorithm("non-existent");
    QVERIFY(algorithm2 == nullptr);
}

void AlgorithmFactoryTest::testGetRegisteredAlgorithms()
{
    std::vector<std::string> algorithms = m_factory->getRegisteredAlgorithms();
    QVERIFY(!algorithms.empty());
    QVERIFY(std::find(algorithms.begin(), algorithms.end(), "mock-algorithm") != algorithms.end());
}

void AlgorithmFactoryTest::testIsAlgorithmRegistered()
{
    QVERIFY(m_factory->isAlgorithmRegistered("mock-algorithm"));
    QVERIFY(!m_factory->isAlgorithmRegistered("non-existent"));
}

void AlgorithmFactoryTest::testUnregisterAlgorithm()
{
    bool success = m_factory->unregisterAlgorithm("mock-algorithm");
    QVERIFY(success);
    QVERIFY(!m_factory->isAlgorithmRegistered("mock-algorithm"));
}

QTEST_MAIN(AlgorithmFactoryTest)
#include "test_algorithm_factory.moc"

