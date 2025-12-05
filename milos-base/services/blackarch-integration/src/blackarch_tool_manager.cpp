#include "blackarch_tool_manager.h"
#include <milos/logging/logger.h>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDateTime>
#include <QVariantMap>

// Note: Update Service integration removed - this is now data-only

// Audit Logger for BlackArch integration
class AuditLogger : public QObject {
    Q_OBJECT
public:
    AuditLogger(QObject* parent = nullptr) 
        : QObject(parent)
        , m_initialized(false)
        , m_interface(nullptr) {}
    
    ~AuditLogger() {
        if (m_interface) {
            delete m_interface;
        }
    }
    
    bool initialize() {
        if (m_initialized) {
            return true;
        }
        
        m_interface = new QDBusInterface("org.milos.AuditService",
                                         "/org/milos/AuditService",
                                         "org.milos.AuditService",
                                         QDBusConnection::systemBus(),
                                         this);
        m_initialized = m_interface->isValid();
        return m_initialized;
    }
    
    void logEvent(const QString& event, const QString& message) {
        if (!m_initialized || !m_interface) {
            return;
        }
        
        QVariantMap eventData;
        eventData["event"] = event;
        eventData["message"] = message;
        eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        
        m_interface->call("LogEvent", event, eventData);
    }
    
private:
    bool m_initialized;
    QDBusInterface* m_interface;
};

BlackArchToolManager::BlackArchToolManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_repository(nullptr)
    , m_offlineInstaller(nullptr)
    , m_auditLogger(nullptr)
    , m_updateService(nullptr)
{
}

BlackArchToolManager::~BlackArchToolManager() {
}

bool BlackArchToolManager::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize repository (data scraper)
    m_repository = new BlackArchRepository(this);
    if (!m_repository->initialize()) {
        LOG_ERROR("Failed to initialize BlackArch repository");
        return false;
    }

    // Load or scrape data if not available
    if (!m_repository->isRepositoryConfigured()) {
        LOG_INFO("BlackArch data not found, starting data scraping...");
        if (!m_repository->configureRepository()) {
            LOG_WARNING("Failed to scrape BlackArch data (will use cached data if available)");
            // Continue anyway - may have cached data
        }
    }

    // Initialize audit logger (inline implementation)
    m_auditLogger = new AuditLogger(this);
    if (!m_auditLogger->initialize()) {
        LOG_WARNING("Failed to initialize audit logger (continuing without audit logging)");
        // Continue without audit logging (graceful degradation)
    }

    // Connect signals
    connect(m_repository, &BlackArchRepository::dataScrapingStarted,
            this, [this]() {
                LOG_INFO("BlackArch data scraping started");
            });
    connect(m_repository, &BlackArchRepository::dataScrapingCompleted,
            this, [this](bool success) {
                if (success) {
                    LOG_INFO("BlackArch data scraping completed successfully");
                } else {
                    LOG_ERROR("BlackArch data scraping failed");
                }
            });

    m_initialized = true;
    return true;
}

bool BlackArchToolManager::installSelectedTools(const QStringList& toolNames) {
    // Note: This is now a data-only system - tools are not installed
    // This method could be used to mark tools as "selected" for future reference
    if (!m_initialized || !m_repository) {
        return false;
    }

    // Verify tools exist in scraped data
    QStringList availableTools = m_repository->getAvailableTools();
    for (const QString& toolName : toolNames) {
        if (!availableTools.contains(toolName)) {
            LOG_WARNING(QString("Tool not found in scraped data: %1").arg(toolName));
        } else {
            LOG_INFO(QString("Tool available in scraped data: %1").arg(toolName));
        }
    }

    return true;
}

QStringList BlackArchToolManager::getInstalledTools() const {
    // Note: This system doesn't install tools, so return empty list
    // Or could check if tools are actually installed on the system
    return QStringList();
}

QString BlackArchToolManager::getToolStatus(const QString& toolName) const {
    if (!m_repository) {
        return "error";
    }

    // Check if tool exists in scraped data
    QString toolInfo = m_repository->getToolInfo(toolName);
    if (toolInfo.isEmpty()) {
        return "not_found";
    }

    // Could also check if tool is actually installed on system
    // For now, just return "available" if found in scraped data
    return "available";
}

bool BlackArchToolManager::updateTools(const QStringList& toolNames) {
    // Refresh scraped data instead of updating tools
    if (!m_initialized || !m_repository) {
        return false;
    }

    LOG_INFO("Refreshing BlackArch tool data...");
    return m_repository->refreshData();
}

bool BlackArchToolManager::createOfflineMirror(const QStringList& toolNames, const QString& mirrorPath) {
    // Note: Offline mirror creation is not needed for data scraping
    // This could be repurposed to export scraped data
    LOG_INFO(QString("Exporting tool data for %1 tools to %2").arg(toolNames.size()).arg(mirrorPath));
    return true;
}

void BlackArchToolManager::onToolInstallationStarted(const QString& toolName) {
    // Not used in data-only mode - kept for compatibility
    Q_UNUSED(toolName);
}

void BlackArchToolManager::onToolInstallationCompleted(const QString& toolName, bool success) {
    // Not used in data-only mode - kept for compatibility
    Q_UNUSED(toolName);
    Q_UNUSED(success);
}

