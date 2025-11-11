#include <QtTest>
#include <QObject>
#include <QString>
#include "../include/milos/pqc/algorithm_registry.h"
#include "../include/milos/pqc/algorithm_interface.h"

/**
 * Unit tests for Algorithm Registry
 */
class AlgorithmRegistryTest : public QObject
{
    Q_OBJECT

public:
    AlgorithmRegistryTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testRegisterAlgorithm();
    void testGetAlgorithmMetadata();
    void testGetAllAlgorithms();
    void testGetAlgorithmsByCapability();
    void testGetNISTApprovedAlgorithms();
    void testUpdatePerformanceMetrics();

private:
    AlgorithmRegistry* m_registry;
};

AlgorithmRegistryTest::AlgorithmRegistryTest()
    : m_registry(nullptr)
{
}

void AlgorithmRegistryTest::initTestCase()
{
    m_registry = &AlgorithmRegistry::getInstance();
}

void AlgorithmRegistryTest::cleanupTestCase()
{
}

void AlgorithmRegistryTest::testRegisterAlgorithm()
{
    AlgorithmMetadata metadata;
    metadata.algorithm_id = "test-algorithm";
    metadata.name = "Test Algorithm";
    metadata.version = "1.0.0";
    metadata.description = "Test algorithm";
    metadata.capabilities.push_back(AlgorithmCapability::KEY_ENCAPSULATION);
    metadata.is_nist_approved = true;
    metadata.is_hybrid_supported = true;
    metadata.key_size = 32;
    metadata.ciphertext_overhead = 16;
    metadata.performance_score = 1.0;

    bool success = m_registry->registerAlgorithm(metadata);
    QVERIFY(success);
}

void AlgorithmRegistryTest::testGetAlgorithmMetadata()
{
    auto metadata = m_registry->getAlgorithmMetadata("test-algorithm");
    QVERIFY(metadata != nullptr);
    QCOMPARE(QString::fromStdString(metadata->algorithm_id), QString("test-algorithm"));
    QCOMPARE(QString::fromStdString(metadata->name), QString("Test Algorithm"));
}

void AlgorithmRegistryTest::testGetAllAlgorithms()
{
    std::vector<AlgorithmMetadata> algorithms = m_registry->getAllAlgorithms();
    QVERIFY(!algorithms.empty());
    
    bool found = false;
    for (const auto& alg : algorithms) {
        if (alg.algorithm_id == "test-algorithm") {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

void AlgorithmRegistryTest::testGetAlgorithmsByCapability()
{
    std::vector<AlgorithmMetadata> algorithms = m_registry->getAlgorithmsByCapability(
        AlgorithmCapability::KEY_ENCAPSULATION
    );
    QVERIFY(!algorithms.empty());
    
    bool found = false;
    for (const auto& alg : algorithms) {
        if (alg.algorithm_id == "test-algorithm") {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

void AlgorithmRegistryTest::testGetNISTApprovedAlgorithms()
{
    std::vector<AlgorithmMetadata> algorithms = m_registry->getNISTApprovedAlgorithms();
    QVERIFY(!algorithms.empty());
    
    bool found = false;
    for (const auto& alg : algorithms) {
        if (alg.algorithm_id == "test-algorithm") {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

void AlgorithmRegistryTest::testUpdatePerformanceMetrics()
{
    bool success = m_registry->updatePerformanceMetrics("test-algorithm", 2.5);
    QVERIFY(success);
    
    auto metadata = m_registry->getAlgorithmMetadata("test-algorithm");
    QVERIFY(metadata != nullptr);
    QCOMPARE(metadata->performance_score, 2.5);
}

QTEST_MAIN(AlgorithmRegistryTest)
#include "test_algorithm_registry.moc"

