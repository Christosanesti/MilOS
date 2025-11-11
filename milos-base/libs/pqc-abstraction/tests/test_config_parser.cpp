#include <QtTest>
#include <QObject>
#include <QString>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include "../include/milos/pqc/config_parser.h"

/**
 * Unit tests for Configuration Parser
 */
class ConfigParserTest : public QObject
{
    Q_OBJECT

public:
    ConfigParserTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testLoadValidConfig();
    void testGetString();
    void testGetBool();
    void testGetInt();
    void testIsLoaded();

private:
    ConfigParser* m_configParser;
    QTemporaryDir* m_tempDir;

    void createConfigFile(const QString& content);
};

ConfigParserTest::ConfigParserTest()
    : m_configParser(nullptr)
    , m_tempDir(nullptr)
{
}

void ConfigParserTest::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void ConfigParserTest::cleanupTestCase()
{
    delete m_tempDir;
}

void ConfigParserTest::init()
{
    m_configParser = new ConfigParser();
}

void ConfigParserTest::cleanup()
{
    delete m_configParser;
}

void ConfigParserTest::createConfigFile(const QString& content)
{
    QString configPath = m_tempDir->path() + "/test_config.yaml";
    QFile file(configPath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&file);
    out << content;
    file.close();
    QVERIFY(m_configParser->load(configPath.toStdString()));
}

void ConfigParserTest::testLoadValidConfig()
{
    QString configContent = R"(
algorithm_selection:
  default_kem: "Kyber512"
  selection_mode: "auto"
)";
    createConfigFile(configContent);
    QVERIFY(m_configParser->isLoaded());
}

void ConfigParserTest::testGetString()
{
    QString configContent = R"(
algorithm_selection:
  default_kem: "Kyber512"
)";
    createConfigFile(configContent);
    QCOMPARE(QString::fromStdString(m_configParser->getString("algorithm_selection.default_kem")), 
             QString("Kyber512"));
}

void ConfigParserTest::testGetBool()
{
    QString configContent = R"(
hybrid_encryption:
  enabled: true
fallback:
  fallback_to_classical: false
)";
    createConfigFile(configContent);
    QVERIFY(m_configParser->getBool("hybrid_encryption.enabled", false));
    QVERIFY(!m_configParser->getBool("fallback.fallback_to_classical", true));
}

void ConfigParserTest::testGetInt()
{
    QString configContent = R"(
performance:
  cache_size_mb: 100
)";
    createConfigFile(configContent);
    QCOMPARE(m_configParser->getInt("performance.cache_size_mb", 0), 100);
}

void ConfigParserTest::testIsLoaded()
{
    ConfigParser parser;
    QVERIFY(!parser.isLoaded());
    createConfigFile("algorithm_selection:\n  default_kem: Kyber512");
    QVERIFY(m_configParser->isLoaded());
}

QTEST_MAIN(ConfigParserTest)
#include "test_config_parser.moc"

