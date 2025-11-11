#include <QtTest>
#include <QObject>
#include <QString>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include "../src/query_engine.h"
#include "../src/config_parser.h"
#include "../src/log_storage.h"

/**
 * Unit tests for Query Engine
 * Tests query parsing, filtering, statistics, and export
 */
class QueryEngineTest : public QObject
{
    Q_OBJECT

public:
    QueryEngineTest();
    ~QueryEngineTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInitialize();
    void testQueryLogs();
    void testQueryLogsWithFilters();
    void testGetStatistics();
    void testExportToJSON();
    void testExportToCSV();
    void testExportToXML();

private:
    QueryEngine* m_queryEngine;
    ConfigParser* m_configParser;
    LogStorage* m_logStorage;
    QTemporaryDir* m_tempDir;

    void createConfigFile(const QString& content);
    void populateTestData();
};

QueryEngineTest::QueryEngineTest()
    : m_queryEngine(nullptr)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
    , m_tempDir(nullptr)
{
}

QueryEngineTest::~QueryEngineTest()
{
}

void QueryEngineTest::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void QueryEngineTest::cleanupTestCase()
{
    delete m_tempDir;
}

void QueryEngineTest::init()
{
    m_configParser = new ConfigParser();
    m_logStorage = new LogStorage();
    m_queryEngine = new QueryEngine();
}

void QueryEngineTest::cleanup()
{
    delete m_queryEngine;
    delete m_logStorage;
    delete m_configParser;
}

void QueryEngineTest::createConfigFile(const QString& content)
{
    QString configPath = m_tempDir->path() + "/config.yaml";
    QString dbPath = m_tempDir->path() + "/audit.db";
    
    QString fullContent = content;
    fullContent.replace("{{DB_PATH}}", dbPath);
    
    QFile file(configPath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&file);
    out << fullContent;
    file.close();
    QVERIFY(m_configParser->load(configPath.toStdString()));
    QVERIFY(m_logStorage->initialize(m_configParser));
}

void QueryEngineTest::populateTestData()
{
    AuditLogEntry entry1;
    entry1.log_id = "log-1";
    entry1.event_type = "ENCRYPTION";
    entry1.event_severity = "INFO";
    entry1.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
    entry1.application_name = "TestApp";
    entry1.event_data = "{\"test\":1}";
    entry1.log_hash = "hash1";
    m_logStorage->storeLogEntry(entry1);

    AuditLogEntry entry2;
    entry2.log_id = "log-2";
    entry2.event_type = "NETWORK_TRANSMISSION";
    entry2.event_severity = "WARNING";
    entry2.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
    entry2.application_name = "TestApp";
    entry2.event_data = "{\"test\":2}";
    entry2.log_hash = "hash2";
    m_logStorage->storeLogEntry(entry2);
}

void QueryEngineTest::testInitialize()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
}

void QueryEngineTest::testQueryLogs()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
    populateTestData();

    QJsonObject queryParams;
    queryParams["limit"] = 10;
    queryParams["offset"] = 0;
    QJsonDocument doc(queryParams);
    QString queryJson = QString::fromUtf8(doc.toJson());

    std::string result = m_queryEngine->queryLogs(queryJson.toStdString());
    QVERIFY(!result.empty());

    // Parse result
    QJsonParseError error;
    QJsonDocument resultDoc = QJsonDocument::fromJson(QString::fromStdString(result).toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(resultDoc.isObject());
    QJsonObject resultObj = resultDoc.object();
    QVERIFY(resultObj.contains("entries"));
    QVERIFY(resultObj.contains("count"));
}

void QueryEngineTest::testQueryLogsWithFilters()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
    populateTestData();

    QJsonObject queryParams;
    queryParams["event_type"] = "ENCRYPTION";
    queryParams["limit"] = 10;
    queryParams["offset"] = 0;
    QJsonDocument doc(queryParams);
    QString queryJson = QString::fromUtf8(doc.toJson());

    std::string result = m_queryEngine->queryLogs(queryJson.toStdString());
    QVERIFY(!result.empty());

    // Parse result and verify filter
    QJsonParseError error;
    QJsonDocument resultDoc = QJsonDocument::fromJson(QString::fromStdString(result).toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QJsonObject resultObj = resultDoc.object();
    QJsonArray entries = resultObj["entries"].toArray();
    QVERIFY(entries.size() > 0);
    QJsonObject firstEntry = entries[0].toObject();
    QCOMPARE(firstEntry["event_type"].toString(), QString("ENCRYPTION"));
}

void QueryEngineTest::testGetStatistics()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
    populateTestData();

    QJsonObject queryParams;
    QJsonDocument doc(queryParams);
    QString queryJson = QString::fromUtf8(doc.toJson());

    std::string result = m_queryEngine->getStatistics(queryJson.toStdString());
    QVERIFY(!result.empty());

    // Parse result
    QJsonParseError error;
    QJsonDocument resultDoc = QJsonDocument::fromJson(QString::fromStdString(result).toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(resultDoc.isObject());
    QJsonObject stats = resultDoc.object();
    QVERIFY(stats.contains("ENCRYPTION") || stats.contains("NETWORK_TRANSMISSION"));
}

void QueryEngineTest::testExportToJSON()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
    populateTestData();

    QJsonObject timeRange;
    QJsonDocument doc(timeRange);
    QString timeRangeJson = QString::fromUtf8(doc.toJson());

    std::string result = m_queryEngine->exportLogs(timeRangeJson.toStdString(), "json");
    QVERIFY(!result.empty());

    // Verify JSON format
    QJsonParseError error;
    QJsonDocument resultDoc = QJsonDocument::fromJson(QString::fromStdString(result).toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
}

void QueryEngineTest::testExportToCSV()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
    populateTestData();

    QJsonObject timeRange;
    QJsonDocument doc(timeRange);
    QString timeRangeJson = QString::fromUtf8(doc.toJson());

    std::string result = m_queryEngine->exportLogs(timeRangeJson.toStdString(), "csv");
    QVERIFY(!result.empty());
    QVERIFY(result.find("log_id,event_type") != std::string::npos);  // CSV header
}

void QueryEngineTest::testExportToXML()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_queryEngine->initialize(m_configParser, m_logStorage));
    populateTestData();

    QJsonObject timeRange;
    QJsonDocument doc(timeRange);
    QString timeRangeJson = QString::fromUtf8(doc.toJson());

    std::string result = m_queryEngine->exportLogs(timeRangeJson.toStdString(), "xml");
    QVERIFY(!result.empty());
    QVERIFY(result.find("<?xml") != std::string::npos);  // XML declaration
    QVERIFY(result.find("<audit_logs>") != std::string::npos);  // Root element
}

QTEST_MAIN(QueryEngineTest)
#include "test_query_engine.moc"

