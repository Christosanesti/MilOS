#include <QtTest>
#include <QObject>
#include <QString>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include "../src/config_parser.h"

/**
 * Unit tests for Configuration Parser
 * Tests YAML parsing and configuration access
 */
class ConfigParserTest : public QObject
{
    Q_OBJECT

public:
    ConfigParserTest();
    ~ConfigParserTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testLoadValidConfig();
    void testLoadInvalidConfig();
    void testLoadNonExistentConfig();
    void testGetString();
    void testGetStringNested();
    void testGetStringNotFound();
    void testGetBool();
    void testGetBoolDefault();
    void testGetInt();
    void testGetIntDefault();
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

ConfigParserTest::~ConfigParserTest()
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
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "/var/lib/milos/audit.db"
)";
    createConfigFile(configContent);
    QVERIFY(m_configParser->isLoaded());
}

void ConfigParserTest::testLoadInvalidConfig()
{
    QString configPath = m_tempDir->path() + "/invalid_config.yaml";
    QFile file(configPath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&file);
    out << "service:\n  enabled: true\n  database_type: sqlite\ninvalid_yaml: : :"; // Malformed YAML
    file.close();
    
    ConfigParser parser;
    QVERIFY(!parser.load(configPath.toStdString()));
    QVERIFY(!parser.isLoaded());
}

void ConfigParserTest::testLoadNonExistentConfig()
{
    ConfigParser parser;
    QVERIFY(!parser.load("/non/existent/path/config.yaml"));
    QVERIFY(!parser.isLoaded());
}

void ConfigParserTest::testGetString()
{
    QString configContent = R"(
service:
  database_type: sqlite
)";
    createConfigFile(configContent);
    QCOMPARE(QString::fromStdString(m_configParser->getString("service.database_type")), QString("sqlite"));
}

void ConfigParserTest::testGetStringNested()
{
    QString configContent = R"(
retention:
  archive:
    path: "/var/lib/milos/archive"
)";
    createConfigFile(configContent);
    QCOMPARE(QString::fromStdString(m_configParser->getString("retention.archive.path")), QString("/var/lib/milos/archive"));
}

void ConfigParserTest::testGetStringNotFound()
{
    QString configContent = R"(
service:
  enabled: true
)";
    createConfigFile(configContent);
    QCOMPARE(QString::fromStdString(m_configParser->getString("service.database_type")), QString(""));
    QCOMPARE(QString::fromStdString(m_configParser->getString("nonexistent.key")), QString(""));
}

void ConfigParserTest::testGetBool()
{
    QString configContent = R"(
service:
  enabled: true
  debug: false
)";
    createConfigFile(configContent);
    QVERIFY(m_configParser->getBool("service.enabled", false));
    QVERIFY(!m_configParser->getBool("service.debug", true));
}

void ConfigParserTest::testGetBoolDefault()
{
    QString configContent = R"(
service:
  name: AuditService
)";
    createConfigFile(configContent);
    QVERIFY(m_configParser->getBool("service.enabled", true)); // Default true
    QVERIFY(!m_configParser->getBool("service.debug", false)); // Default false
}

void ConfigParserTest::testGetInt()
{
    QString configContent = R"(
retention:
  retention_days: 365
  archive_retention_days: 2555
)";
    createConfigFile(configContent);
    QCOMPARE(m_configParser->getInt("retention.retention_days", 0), 365);
    QCOMPARE(m_configParser->getInt("retention.archive_retention_days", 0), 2555);
}

void ConfigParserTest::testGetIntDefault()
{
    QString configContent = R"(
service:
  name: AuditService
)";
    createConfigFile(configContent);
    QCOMPARE(m_configParser->getInt("retention.retention_days", 365), 365); // Default 365
    QCOMPARE(m_configParser->getInt("retention.archive_retention_days", 2555), 2555); // Default 2555
}

void ConfigParserTest::testIsLoaded()
{
    ConfigParser parser;
    QVERIFY(!parser.isLoaded());
    createConfigFile("service:\n  enabled: true");
    QVERIFY(m_configParser->isLoaded());
}

QTEST_MAIN(ConfigParserTest)
#include "test_config_parser.moc"

