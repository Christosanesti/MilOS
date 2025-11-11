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

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Configuration loading tests
    void testLoadConfiguration();
    void testLoadConfigurationInvalidFile();
    void testLoadConfigurationNonExistentFile();
    
    // Configuration access tests
    void testGetString();
    void testGetStringNested();
    void testGetStringNotFound();
    void testGetBool();
    void testGetBoolDefault();
    void testGetInt();
    void testGetIntDefault();
    
    // Configuration state tests
    void testIsLoaded();
    
private:
    QString createTestConfigFile();
    QTemporaryDir* m_tempDir;
};

void ConfigParserTest::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void ConfigParserTest::cleanupTestCase()
{
    delete m_tempDir;
}

QString ConfigParserTest::createTestConfigFile()
{
    QString configPath = m_tempDir->filePath("config.yaml");
    QFile configFile(configPath);
    
    if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&configFile);
        out << "service:\n";
        out << "  enabled: true\n";
        out << "  log_level: info\n";
        out << "  audit_service_bus: org.milos.AuditService\n";
        out << "monitoring:\n";
        out << "  real_time_monitoring: true\n";
        out << "  inspection_depth: 1024\n";
        configFile.close();
    }
    
    return configPath;
}

void ConfigParserTest::testLoadConfiguration()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    QVERIFY(parser.isLoaded());
}

void ConfigParserTest::testLoadConfigurationInvalidFile()
{
    QString invalidPath = m_tempDir->filePath("invalid.yaml");
    QFile invalidFile(invalidPath);
    
    if (invalidFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&invalidFile);
        out << "invalid: yaml: content: [\n";  // Invalid YAML
        invalidFile.close();
    }
    
    ConfigParser parser;
    QVERIFY(!parser.load(invalidPath.toStdString()));
}

void ConfigParserTest::testLoadConfigurationNonExistentFile()
{
    ConfigParser parser;
    QVERIFY(!parser.load("/nonexistent/path/config.yaml"));
}

void ConfigParserTest::testGetString()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    std::string value = parser.getString("service.log_level");
    QCOMPARE(QString::fromStdString(value), QString("info"));
}

void ConfigParserTest::testGetStringNested()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    std::string value = parser.getString("service.audit_service_bus");
    QCOMPARE(QString::fromStdString(value), QString("org.milos.AuditService"));
}

void ConfigParserTest::testGetStringNotFound()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    std::string value = parser.getString("nonexistent.key");
    QVERIFY(value.empty());
}

void ConfigParserTest::testGetBool()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    bool value = parser.getBool("service.enabled");
    QVERIFY(value);
}

void ConfigParserTest::testGetBoolDefault()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    bool value = parser.getBool("nonexistent.key", true);
    QVERIFY(value);
}

void ConfigParserTest::testGetInt()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    int value = parser.getInt("monitoring.inspection_depth");
    QCOMPARE(value, 1024);
}

void ConfigParserTest::testGetIntDefault()
{
    QString configPath = createTestConfigFile();
    
    ConfigParser parser;
    QVERIFY(parser.load(configPath.toStdString()));
    
    int value = parser.getInt("nonexistent.key", 42);
    QCOMPARE(value, 42);
}

void ConfigParserTest::testIsLoaded()
{
    ConfigParser parser;
    QVERIFY(!parser.isLoaded());
    
    QString configPath = createTestConfigFile();
    QVERIFY(parser.load(configPath.toStdString()));
    QVERIFY(parser.isLoaded());
}

QTEST_MAIN(ConfigParserTest)
#include "test_config_parser.moc"

