#include <QtTest>
#include <QObject>
#include <QString>
#include "../include/milos/encryption/key_derivation.h"

/**
 * Unit tests for Key Derivation Library
 */
class KeyDerivationTest : public QObject
{
    Q_OBJECT

public:
    KeyDerivationTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialize();
    void testDeriveFromPasswordPBKDF2();
    void testDeriveHKDF();
    void testGenerateSalt();
    void testKeyLength();

private:
    KeyDerivation* m_keyDerivation;
};

KeyDerivationTest::KeyDerivationTest()
    : m_keyDerivation(nullptr)
{
}

void KeyDerivationTest::initTestCase()
{
    m_keyDerivation = new KeyDerivation();
}

void KeyDerivationTest::cleanupTestCase()
{
    delete m_keyDerivation;
}

void KeyDerivationTest::testInitialize()
{
    bool success = m_keyDerivation->initialize(KeyDerivationAlgorithm::PBKDF2_PQC, "SHA-256");
    QVERIFY(success);
    QVERIFY(m_keyDerivation->isInitialized());
}

void KeyDerivationTest::testDeriveFromPasswordPBKDF2()
{
    m_keyDerivation->initialize(KeyDerivationAlgorithm::PBKDF2_PQC, "SHA-256");
    
    std::string password = "test_password_123";
    std::vector<uint8_t> salt = KeyDerivation::generateSalt(32);
    size_t keyLength = 32;
    uint32_t iterations = 10000;
    
    KeyDerivationResult result = m_keyDerivation->deriveFromPassword(
        password, salt, keyLength, iterations
    );
    
    QVERIFY(result.success);
    QVERIFY(result.derived_key.size() == keyLength);
    QVERIFY(!result.derived_key.empty());
}

void KeyDerivationTest::testDeriveHKDF()
{
    m_keyDerivation->initialize(KeyDerivationAlgorithm::HKDF_PQC, "SHA-256");
    
    std::vector<uint8_t> inputKeyMaterial = {0x01, 0x02, 0x03, 0x04, 0x05};
    std::vector<uint8_t> salt = KeyDerivation::generateSalt(32);
    std::vector<uint8_t> info = {0x61, 0x62, 0x63};  // "abc"
    size_t keyLength = 32;
    
    KeyDerivationResult result = m_keyDerivation->deriveHKDF(
        inputKeyMaterial, salt, info, keyLength
    );
    
    QVERIFY(result.success);
    QVERIFY(result.derived_key.size() == keyLength);
    QVERIFY(!result.derived_key.empty());
}

void KeyDerivationTest::testGenerateSalt()
{
    std::vector<uint8_t> salt1 = KeyDerivation::generateSalt(32);
    std::vector<uint8_t> salt2 = KeyDerivation::generateSalt(32);
    
    QVERIFY(salt1.size() == 32);
    QVERIFY(salt2.size() == 32);
    // Salts should be different (very high probability)
    QVERIFY(salt1 != salt2);
}

void KeyDerivationTest::testKeyLength()
{
    m_keyDerivation->initialize(KeyDerivationAlgorithm::PBKDF2_PQC, "SHA-256");
    
    std::string password = "test_password";
    std::vector<uint8_t> salt = KeyDerivation::generateSalt(32);
    
    // Test different key lengths
    for (size_t keyLen : {16, 32, 64, 128}) {
        KeyDerivationResult result = m_keyDerivation->deriveFromPassword(
            password, salt, keyLen, 10000
        );
        
        QVERIFY(result.success);
        QVERIFY(result.derived_key.size() == keyLen);
    }
}

QTEST_MAIN(KeyDerivationTest)
#include "test_key_derivation.moc"

