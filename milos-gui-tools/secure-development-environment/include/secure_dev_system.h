#ifndef SECURE_DEV_SYSTEM_H
#define SECURE_DEV_SYSTEM_H

#include "code_analyzer.h"
#include "supply_chain_manager.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Secure Development Environment System
 * 
 * Main system for secure development tools.
 */
class SecureDevSystem : public QObject {
    Q_OBJECT

public:
    explicit SecureDevSystem(QObject* parent = nullptr);
    ~SecureDevSystem();

    /**
     * @brief Initialize secure development system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get code analyzer
     * @return Code analyzer
     */
    CodeAnalyzer* getCodeAnalyzer() const { return m_codeAnalyzer; }

    /**
     * @brief Get supply chain manager
     * @return Supply chain manager
     */
    SupplyChainManager* getSupplyChainManager() const { return m_supplyChainManager; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    SecureDevDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    CodeAnalyzer* m_codeAnalyzer;
    SupplyChainManager* m_supplyChainManager;
    SecureDevDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // SECURE_DEV_SYSTEM_H






