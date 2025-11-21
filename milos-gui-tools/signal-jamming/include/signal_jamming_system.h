#ifndef SIGNAL_JAMMING_SYSTEM_H
#define SIGNAL_JAMMING_SYSTEM_H

#include "jamming_manager.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Signal Jamming System
 * 
 * Main system for signal jamming management.
 */
class SignalJammingSystem : public QObject {
    Q_OBJECT

public:
    explicit SignalJammingSystem(QObject* parent = nullptr);
    ~SignalJammingSystem();

    /**
     * @brief Initialize signal jamming system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get jamming manager
     * @return Jamming manager
     */
    JammingManager* getManager() const { return m_manager; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    SignalJammingDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    JammingManager* m_manager;
    SignalJammingDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // SIGNAL_JAMMING_SYSTEM_H

