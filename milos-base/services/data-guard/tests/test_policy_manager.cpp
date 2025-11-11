#include <QtTest>
#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include <QTextStream>
#include "../src/policy_manager.h"
#include "../src/config_parser.h"

/**
 * Unit tests for Policy Manager
 * Tests policy loading, validation, and application
 */
class PolicyManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Policy loading tests
    void testPolicyManagerInitialization();
    void testLoadPolicies();
    void testReloadPolicies();
    
    // Policy validation tests
    void testValidatePolicyValid();
    void testValidatePolicyInvalidId();
    void testValidatePolicyInvalidName();
    void testValidatePolicyInvalidType();
    void testValidatePolicyEmptyRules();
    void testValidatePolicyInvalidRuleAction();
    
    // Policy retrieval tests
    void testGetPolicies();
    void testGetPolicyById();
    void testGetPolicyByIdNotFound();
    
    // Policy application tests
    void testApplyPolicy();
    void testPolicyPrioritySorting();
    
private:
    ConfigParser* createTestConfigParser();
    QTemporaryDir* m_tempDir;
};

void PolicyManagerTest::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void PolicyManagerTest::cleanupTestCase()
{
    delete m_tempDir;
}

ConfigParser* PolicyManagerTest::createTestConfigParser()
{
    // Create a test configuration file
    QString configPath = m_tempDir->filePath("config.yaml");
    QFile configFile(configPath);
    
    if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&configFile);
        out << "service:\n";
        out << "  enabled: true\n";
        out << "  log_level: info\n";
        configFile.close();
    }
    
    ConfigParser* parser = new ConfigParser();
    if (parser->load(configPath.toStdString())) {
        return parser;
    }
    
    delete parser;
    return nullptr;
}

void PolicyManagerTest::testPolicyManagerInitialization()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    delete parser;
}

void PolicyManagerTest::testLoadPolicies()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    std::vector<NetworkPolicy> policies = manager.getPolicies();
    QVERIFY(policies.size() > 0);  // Should have default policies
    
    delete parser;
}

void PolicyManagerTest::testReloadPolicies()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    size_t initialCount = manager.getPolicies().size();
    
    // Reload policies
    QVERIFY(manager.reloadPolicies(parser));
    
    size_t reloadedCount = manager.getPolicies().size();
    QCOMPARE(initialCount, reloadedCount);
    
    delete parser;
}

void PolicyManagerTest::testValidatePolicyValid()
{
    PolicyManager manager;
    
    NetworkPolicy policy;
    policy.policy_id = "test-policy-1";
    policy.policy_name = "Test Policy";
    policy.policy_type = "ENCRYPTION_REQUIRED";
    policy.enabled = true;
    policy.priority = 100;
    
    PolicyRule rule;
    rule.rule_id = "rule-1";
    rule.encryption_required = true;
    rule.action = "BLOCK";
    policy.rules.push_back(rule);
    
    QVERIFY(manager.validatePolicy(policy));
}

void PolicyManagerTest::testValidatePolicyInvalidId()
{
    PolicyManager manager;
    
    NetworkPolicy policy;
    policy.policy_id = "";  // Empty ID
    policy.policy_name = "Test Policy";
    policy.policy_type = "ENCRYPTION_REQUIRED";
    policy.enabled = true;
    policy.priority = 100;
    
    PolicyRule rule;
    rule.rule_id = "rule-1";
    rule.encryption_required = true;
    rule.action = "BLOCK";
    policy.rules.push_back(rule);
    
    QVERIFY(!manager.validatePolicy(policy));
}

void PolicyManagerTest::testValidatePolicyInvalidName()
{
    PolicyManager manager;
    
    NetworkPolicy policy;
    policy.policy_id = "test-policy-1";
    policy.policy_name = "";  // Empty name
    policy.policy_type = "ENCRYPTION_REQUIRED";
    policy.enabled = true;
    policy.priority = 100;
    
    PolicyRule rule;
    rule.rule_id = "rule-1";
    rule.encryption_required = true;
    rule.action = "BLOCK";
    policy.rules.push_back(rule);
    
    QVERIFY(!manager.validatePolicy(policy));
}

void PolicyManagerTest::testValidatePolicyInvalidType()
{
    PolicyManager manager;
    
    NetworkPolicy policy;
    policy.policy_id = "test-policy-1";
    policy.policy_name = "Test Policy";
    policy.policy_type = "INVALID_TYPE";  // Invalid type
    policy.enabled = true;
    policy.priority = 100;
    
    PolicyRule rule;
    rule.rule_id = "rule-1";
    rule.encryption_required = true;
    rule.action = "BLOCK";
    policy.rules.push_back(rule);
    
    QVERIFY(!manager.validatePolicy(policy));
}

void PolicyManagerTest::testValidatePolicyEmptyRules()
{
    PolicyManager manager;
    
    NetworkPolicy policy;
    policy.policy_id = "test-policy-1";
    policy.policy_name = "Test Policy";
    policy.policy_type = "ENCRYPTION_REQUIRED";
    policy.enabled = true;
    policy.priority = 100;
    // No rules
    
    QVERIFY(!manager.validatePolicy(policy));
}

void PolicyManagerTest::testValidatePolicyInvalidRuleAction()
{
    PolicyManager manager;
    
    NetworkPolicy policy;
    policy.policy_id = "test-policy-1";
    policy.policy_name = "Test Policy";
    policy.policy_type = "ENCRYPTION_REQUIRED";
    policy.enabled = true;
    policy.priority = 100;
    
    PolicyRule rule;
    rule.rule_id = "rule-1";
    rule.encryption_required = true;
    rule.action = "INVALID_ACTION";  // Invalid action
    policy.rules.push_back(rule);
    
    QVERIFY(!manager.validatePolicy(policy));
}

void PolicyManagerTest::testGetPolicies()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    std::vector<NetworkPolicy> policies = manager.getPolicies();
    QVERIFY(policies.size() > 0);
    
    delete parser;
}

void PolicyManagerTest::testGetPolicyById()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    std::vector<NetworkPolicy> policies = manager.getPolicies();
    if (policies.size() > 0) {
        const NetworkPolicy* policy = manager.getPolicy(policies[0].policy_id);
        QVERIFY(policy != nullptr);
        QCOMPARE(policy->policy_id, policies[0].policy_id);
    }
    
    delete parser;
}

void PolicyManagerTest::testGetPolicyByIdNotFound()
{
    PolicyManager manager;
    
    const NetworkPolicy* policy = manager.getPolicy("non-existent-policy");
    QVERIFY(policy == nullptr);
}

void PolicyManagerTest::testApplyPolicy()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    std::vector<NetworkPolicy> policies = manager.getPolicies();
    if (policies.size() > 0) {
        // Apply first policy
        QVERIFY(manager.applyPolicy(policies[0].policy_id));
    }
    
    delete parser;
}

void PolicyManagerTest::testPolicyPrioritySorting()
{
    ConfigParser* parser = createTestConfigParser();
    QVERIFY(parser != nullptr);
    
    PolicyManager manager;
    QVERIFY(manager.initialize(parser));
    
    std::vector<NetworkPolicy> policies = manager.getPolicies();
    
    // Verify policies are sorted by priority (higher priority first)
    for (size_t i = 1; i < policies.size(); ++i) {
        QVERIFY(policies[i-1].priority >= policies[i].priority);
    }
    
    delete parser;
}

QTEST_MAIN(PolicyManagerTest)
#include "test_policy_manager.moc"

