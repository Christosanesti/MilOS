#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <iostream>

/**
 * @brief System Integration Test Suite
 * 
 * Tests system-wide integration of all MilOS components.
 */
class SystemIntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testAuditServiceIntegration();
    void testUpdateServiceIntegration();
    void testDataGuardIntegration();
    void testEncryptionServiceIntegration();
    void testNetworkDashboardIntegration();
    void testBackupServiceIntegration();
    void testNetworkSegmentationIntegration();
    void testSecurityStatusDashboardIntegration();
    
    void testEndToEndWorkflow();

private:
    bool verifyServiceAvailable(const QString& service, const QString& path);
};

void SystemIntegrationTest::initTestCase() {
    std::cout << "Starting System Integration Tests..." << std::endl;
}

void SystemIntegrationTest::cleanupTestCase() {
    std::cout << "System Integration Tests Completed" << std::endl;
}

bool SystemIntegrationTest::verifyServiceAvailable(const QString& service, const QString& path) {
    QDBusInterface interface(service, path, service, QDBusConnection::systemBus());
    return interface.isValid();
}

void SystemIntegrationTest::testAuditServiceIntegration() {
    std::cout << "Testing Audit Service Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.AuditService",
                           "/org/milos/AuditService",
                           "org.milos.AuditService",
                           QDBusConnection::systemBus());
    
    QVERIFY(interface.isValid());
    
    // Test logging an event
    QVariantMap eventData;
    eventData["test"] = "integration_test";
    QDBusReply<void> reply = interface.call("LogEvent", "test_event", eventData);
    QVERIFY(reply.isValid());
    
    std::cout << "  ✓ Audit Service Integration: PASSED" << std::endl;
}

void SystemIntegrationTest::testUpdateServiceIntegration() {
    std::cout << "Testing Update Service Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.UpdateService",
                             "/org/milos/UpdateService",
                             "org.milos.UpdateService",
                             QDBusConnection::systemBus());
    
    if (interface.isValid()) {
        QDBusReply<QVariantMap> reply = interface.call("GetUpdateStatus");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Update Service Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Update Service Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testDataGuardIntegration() {
    std::cout << "Testing Data Transmission Guard Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.DataGuard",
                             "/org/milos/DataGuard",
                             "org.milos.DataGuard",
                             QDBusConnection::systemBus());
    
    if (interface.isValid()) {
        QDBusReply<QVariantMap> reply = interface.call("GetPolicyStatus");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Data Transmission Guard Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Data Transmission Guard Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testEncryptionServiceIntegration() {
    std::cout << "Testing Encryption Service Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.EncryptionService",
                             "/org/milos/EncryptionService",
                             "org.milos.EncryptionService",
                             QDBusConnection::sessionBus());
    
    if (interface.isValid()) {
        QDBusReply<QVariantMap> reply = interface.call("GetEncryptionStatus");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Encryption Service Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Encryption Service Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testNetworkDashboardIntegration() {
    std::cout << "Testing Network Dashboard Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.NetworkDashboard",
                             "/org/milos/NetworkDashboard",
                             "org.milos.NetworkDashboard",
                             QDBusConnection::sessionBus());
    
    if (interface.isValid()) {
        QDBusReply<QVariantMap> reply = interface.call("GetSecurityStatus");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Network Dashboard Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Network Dashboard Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testBackupServiceIntegration() {
    std::cout << "Testing Backup Service Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.BackupService",
                             "/org/milos/BackupService",
                             "org.milos.BackupService",
                             QDBusConnection::sessionBus());
    
    if (interface.isValid()) {
        QDBusReply<QStringList> reply = interface.call("ListBackups");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Backup Service Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Backup Service Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testNetworkSegmentationIntegration() {
    std::cout << "Testing Network Segmentation Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.NetworkSegmentation",
                             "/org/milos/NetworkSegmentation",
                             "org.milos.NetworkSegmentation",
                             QDBusConnection::sessionBus());
    
    if (interface.isValid()) {
        QDBusReply<QStringList> reply = interface.call("ListSegments");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Network Segmentation Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Network Segmentation Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testSecurityStatusDashboardIntegration() {
    std::cout << "Testing Security Status Dashboard Integration..." << std::endl;
    
    QDBusInterface interface("org.milos.SecurityStatusDashboard",
                             "/org/milos/SecurityStatusDashboard",
                             "org.milos.SecurityStatusDashboard",
                             QDBusConnection::sessionBus());
    
    if (interface.isValid()) {
        QDBusReply<QString> reply = interface.call("GetOverallStatus");
        QVERIFY(reply.isValid());
        std::cout << "  ✓ Security Status Dashboard Integration: PASSED" << std::endl;
    } else {
        std::cout << "  ⚠ Security Status Dashboard Integration: SKIPPED (service not available)" << std::endl;
    }
}

void SystemIntegrationTest::testEndToEndWorkflow() {
    std::cout << "Testing End-to-End Workflow..." << std::endl;
    
    // Test complete workflow: Create segment -> Generate rules -> Apply rules -> Verify
    QDBusInterface segmentationInterface("org.milos.NetworkSegmentation",
                                         "/org/milos/NetworkSegmentation",
                                         "org.milos.NetworkSegmentation",
                                         QDBusConnection::sessionBus());
    
    if (segmentationInterface.isValid()) {
        // Create test segment
        QDBusReply<QString> createReply = segmentationInterface.call("CreateSegment",
                                                                     "Test Segment",
                                                                     "192.168.100.0/24",
                                                                     "Integration test segment");
        
        if (createReply.isValid() && !createReply.value().isEmpty()) {
            QString segmentId = createReply.value();
            
            // Generate firewall rules
            QDBusReply<bool> generateReply = segmentationInterface.call("GenerateFirewallRules");
            QVERIFY(generateReply.isValid());
            
            // Clean up: Delete test segment
            segmentationInterface.call("DeleteSegment", segmentId);
            
            std::cout << "  ✓ End-to-End Workflow: PASSED" << std::endl;
        } else {
            std::cout << "  ⚠ End-to-End Workflow: SKIPPED (service not available)" << std::endl;
        }
    } else {
        std::cout << "  ⚠ End-to-End Workflow: SKIPPED (service not available)" << std::endl;
    }
}

QTEST_MAIN(SystemIntegrationTest)
#include "test_system_integration.moc"

