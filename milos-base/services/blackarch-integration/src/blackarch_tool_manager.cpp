#include "blackarch_tool_manager.h"
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDateTime>
#include <QVariantMap>
#include <QDebug>
#include <iostream>

// Forward declaration for Update Service interface
class UpdateServiceInterface {
public:
    static void registerPackage(const QString& packageName) {
        // Integration with Update Service (Epic 16)
        // This would call the Update Service D-Bus interface
        qDebug() << "Registering package with Update Service:" << packageName;
    }
};

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

    // Initialize repository
    m_repository = new BlackArchRepository(this);
    if (!m_repository->initialize()) {
        std::cerr << "Failed to initialize BlackArch repository" << std::endl;
        return false;
    }

    // Configure repository if not already configured
    if (!m_repository->isRepositoryConfigured()) {
        if (!m_repository->configureRepository()) {
            std::cerr << "Failed to configure BlackArch repository" << std::endl;
            return false;
        }
    }

    // Initialize offline installer
    m_offlineInstaller = new BlackArchOfflineInstaller(this);
    if (!m_offlineInstaller->initialize()) {
        std::cerr << "Failed to initialize offline installer" << std::endl;
        return false;
    }

    // Initialize audit logger (inline implementation)
    m_auditLogger = new AuditLogger(this);
    if (!m_auditLogger->initialize()) {
        std::cerr << "Warning: Failed to initialize audit logger (continuing without audit logging)" << std::endl;
        // Continue without audit logging (graceful degradation)
    }

    // Connect signals
    connect(m_repository, &BlackArchRepository::toolInstallationStarted,
            this, &BlackArchToolManager::onToolInstallationStarted);
    connect(m_repository, &BlackArchRepository::toolInstallationCompleted,
            this, &BlackArchToolManager::onToolInstallationCompleted);

    m_initialized = true;
    return true;
}

bool BlackArchToolManager::installSelectedTools(const QStringList& toolNames) {
    if (!m_initialized || !m_repository) {
        return false;
    }

    return m_repository->installTools(toolNames);
}

QStringList BlackArchToolManager::getInstalledTools() const {
    if (!m_repository) {
        return QStringList();
    }

    QStringList allTools = m_repository->getAvailableTools();
    QStringList installedTools;

    for (const QString& tool : allTools) {
        if (m_repository->isToolInstalled(tool)) {
            installedTools.append(tool);
        }
    }

    return installedTools;
}

QString BlackArchToolManager::getToolStatus(const QString& toolName) const {
    if (!m_repository) {
        return "error";
    }

    if (m_repository->isToolInstalled(toolName)) {
        return "installed";
    } else {
        return "not_installed";
    }
}

bool BlackArchToolManager::updateTools(const QStringList& toolNames) {
    if (!m_initialized || !m_repository) {
        return false;
    }

    if (toolNames.isEmpty()) {
        return m_repository->updateAllTools();
    } else {
        bool allSuccess = true;
        for (const QString& toolName : toolNames) {
            emit toolUpdateStarted(toolName);
            bool success = m_repository->updateTool(toolName);
            emit toolUpdateCompleted(toolName, success);
            if (!success) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
}

bool BlackArchToolManager::createOfflineMirror(const QStringList& toolNames, const QString& mirrorPath) {
    if (!m_initialized || !m_offlineInstaller) {
        return false;
    }

    return m_offlineInstaller->createMirror(toolNames, mirrorPath);
}

void BlackArchToolManager::onToolInstallationStarted(const QString& toolName) {
    emit toolInstallationStarted(toolName);
    m_toolStatus[toolName] = "installing";
}

void BlackArchToolManager::onToolInstallationCompleted(const QString& toolName, bool success) {
    emit toolInstallationCompleted(toolName, success);
    
    if (success) {
        m_toolStatus[toolName] = "installed";
        logToolInstallation(toolName, true);
        registerToolWithUpdateService(toolName);
    } else {
        m_toolStatus[toolName] = "error";
        logToolInstallation(toolName, false);
    }
}

void BlackArchToolManager::logToolInstallation(const QString& toolName, bool success) {
    if (!m_auditLogger) {
        return;
    }

    QString event = success ? "blackarch_tool_installed" : "blackarch_tool_installation_failed";
    QString message = QString("BlackArch tool %1: %2").arg(toolName).arg(success ? "installed" : "installation failed");
    
    m_auditLogger->logEvent(event, message);
}

void BlackArchToolManager::registerToolWithUpdateService(const QString& toolName) {
    UpdateServiceInterface::registerPackage(toolName);
}

