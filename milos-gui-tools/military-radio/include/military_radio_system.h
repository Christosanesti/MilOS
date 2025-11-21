#ifndef MILITARY_RADIO_SYSTEM_H
#define MILITARY_RADIO_SYSTEM_H

#include "radio_manager.h"
#include "radio_encryption.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Military Radio System
 * 
 * Main system for military radio integration.
 */
class MilitaryRadioSystem : public QObject {
    Q_OBJECT

public:
    explicit MilitaryRadioSystem(QObject* parent = nullptr);
    ~MilitaryRadioSystem();

    /**
     * @brief Initialize military radio system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get radio manager
     * @return Radio manager
     */
    RadioManager* getRadioManager() const { return m_radioManager; }

    /**
     * @brief Get radio encryption
     * @return Radio encryption
     */
    RadioEncryption* getRadioEncryption() const { return m_radioEncryption; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    MilitaryRadioDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    RadioManager* m_radioManager;
    RadioEncryption* m_radioEncryption;
    MilitaryRadioDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // MILITARY_RADIO_SYSTEM_H

