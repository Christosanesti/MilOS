#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class FIMService;
class BaselineManager;
class ChangeDetector;
class IntegrityVerifier;

/**
 * @brief D-Bus Interface for File Integrity Monitoring Service
 * 
 * Implements org.milos.FileIntegrityMonitoring D-Bus interface.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.FileIntegrityMonitoring")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Set FIM service instance
     */
    void setFIMService(FIMService* service);

    /**
     * @brief Set baseline manager instance
     */
    void setBaselineManager(BaselineManager* manager);

    /**
     * @brief Set change detector instance
     */
    void setChangeDetector(ChangeDetector* detector);

    /**
     * @brief Set integrity verifier instance
     */
    void setIntegrityVerifier(IntegrityVerifier* verifier);

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Create baseline for file(s)
     * @param filePaths List of file paths
     * @return List of baseline IDs
     */
    QStringList CreateBaseline(const QStringList& filePaths);

    /**
     * @brief Verify file integrity
     * @param filePath File path to verify
     * @return Verification result (JSON string)
     */
    QString VerifyIntegrity(const QString& filePath);

    /**
     * @brief Get detected changes
     * @param filters Optional filters (JSON string)
     * @return List of changes (JSON string)
     */
    QString GetChanges(const QString& filters = "");

    /**
     * @brief Get baseline status
     * @param baselineId Baseline ID
     * @return Status string
     */
    QString GetBaselineStatus(const QString& baselineId);

    /**
     * @brief Update baseline
     * @param baselineId Baseline ID
     * @return true if update successful, false otherwise
     */
    bool UpdateBaseline(const QString& baselineId);

Q_SIGNALS:
    /**
     * @brief Emitted when file changes detected
     * @param filePath File path
     * @param changeType Change type
     */
    void FileChanged(const QString& filePath, const QString& changeType);

    /**
     * @brief Emitted on integrity violation
     * @param filePath File path
     * @param details Violation details (JSON string)
     */
    void IntegrityViolation(const QString& filePath, const QString& details);

private:
    bool m_initialized;
    FIMService* m_fimService;
    BaselineManager* m_baselineManager;
    ChangeDetector* m_changeDetector;
    IntegrityVerifier* m_integrityVerifier;
};

#endif // DBUS_INTERFACE_H

