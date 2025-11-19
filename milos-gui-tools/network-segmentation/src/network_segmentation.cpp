#include "network_segmentation.h"
#include "segment_manager.h"
#include "topology_display.h"
#include "firewall_manager.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <iostream>

NetworkSegmentation::NetworkSegmentation(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_app(nullptr)
    , m_engine(nullptr)
    , m_segmentManager(nullptr)
    , m_topologyDisplay(nullptr)
{
}

NetworkSegmentation::~NetworkSegmentation() {
}

bool NetworkSegmentation::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create QApplication
    int argc = 0;
    m_app = new QApplication(argc, nullptr);
    m_app->setApplicationName("MilOS Network Segmentation");
    m_app->setOrganizationName("MilOS");

    // Create QML engine
    m_engine = new QQmlApplicationEngine(this);

    // Initialize segment manager
    m_segmentManager = new SegmentManager(this);
    if (!m_segmentManager->initialize()) {
        std::cerr << "Failed to initialize segment manager" << std::endl;
        return false;
    }

    // Initialize topology display
    m_topologyDisplay = new TopologyDisplay(this);
    if (!m_topologyDisplay->initialize()) {
        std::cerr << "Failed to initialize topology display" << std::endl;
        return false;
    }

    // Initialize firewall manager
    FirewallManager* firewallManager = new FirewallManager(this);
    if (!firewallManager->initialize()) {
        std::cerr << "Failed to initialize firewall manager" << std::endl;
        return false;
    }

    // Generate firewall rules when segments change
    connect(m_segmentManager, &SegmentManager::segmentsChanged, [firewallManager, this]() {
        firewallManager->generateRulesFromSegments(m_segmentManager->segments());
    });

    // Initialize D-Bus interface
    DBusInterface* dbusInterface = new DBusInterface(this);
    dbusInterface->setSegmentManager(m_segmentManager);
    dbusInterface->setFirewallManager(firewallManager);
    if (!dbusInterface->initialize()) {
        std::cerr << "Warning: Failed to initialize D-Bus interface (continuing without D-Bus)" << std::endl;
    }

    // Initialize audit logger
    AuditLogger* auditLogger = new AuditLogger(this);
    if (!auditLogger->initialize()) {
        std::cerr << "Warning: Failed to initialize audit logger (continuing without audit logging)" << std::endl;
    }

    // Update topology when segments change
    connect(m_segmentManager, &SegmentManager::segmentsChanged, [this]() {
        m_topologyDisplay->updateTopology(m_segmentManager->segments());
    });

    // Log segment operations
    connect(m_segmentManager, &SegmentManager::segmentCreated, [auditLogger](const QString& segmentId) {
        auditLogger->logSegmentationOperation("create", segmentId);
    });
    connect(m_segmentManager, &SegmentManager::segmentUpdated, [auditLogger](const QString& segmentId) {
        auditLogger->logSegmentationOperation("update", segmentId);
    });
    connect(m_segmentManager, &SegmentManager::segmentDeleted, [auditLogger](const QString& segmentId) {
        auditLogger->logSegmentationOperation("delete", segmentId);
    });

    // Register with QML
    m_engine->rootContext()->setContextProperty("segmentManager", m_segmentManager);
    m_engine->rootContext()->setContextProperty("topologyDisplay", m_topologyDisplay);
    m_engine->rootContext()->setContextProperty("firewallManager", firewallManager);

    // Load QML
    const QUrl qmlUrl(QStringLiteral("qrc:/ui/main.qml"));
    m_engine->load(qmlUrl);

    if (m_engine->rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

int NetworkSegmentation::run() {
    if (!m_initialized) {
        if (!initialize()) {
            return 1;
        }
    }

    return m_app->exec();
}

