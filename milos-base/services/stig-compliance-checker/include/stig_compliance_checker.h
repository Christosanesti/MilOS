#ifndef STIG_COMPLIANCE_CHECKER_H
#define STIG_COMPLIANCE_CHECKER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief STIG Compliance Check Result
 */
enum class STIGCheckResult {
    Pass,           // Check passed
    Fail,           // Check failed
    NotApplicable,  // Check not applicable to this system
    NotChecked,     // Check not yet performed
    Error           // Error during check
};

/**
 * @brief STIG Check Severity
 */
enum class STIGSeverity {
    Critical,      // Critical finding
    High,           // High severity
    Medium,         // Medium severity
    Low,            // Low severity
    Informational   // Informational only
};

/**
 * @brief STIG Compliance Check
 */
struct STIGCheck {
    QString checkId;              // STIG check ID (e.g., "NET-TUN-0010")
    QString title;                // Check title
    QString description;          // Check description
    STIGSeverity severity;        // Check severity
    STIGCheckResult result;       // Check result
    QString message;              // Result message
    QString remediation;           // Remediation steps
    QDateTime lastChecked;         // Last check timestamp
    QVariantMap details;          // Additional check details
    QStringList references;       // STIG/NIST references
};

/**
 * @brief STIG Compliance Checker
 * 
 * Automated STIG compliance checking for MilOS services and system configuration.
 * Implements checks for:
 * - Network security (NET-TUN series)
 * - Audit and accountability (AU series)
 * - Access control (AC series)
 * - System configuration (general STIG requirements)
 * 
 * DoD Alignment:
 * - NIST 800-53 controls
 * - DISA STIG requirements
 * - DoD 8500 series compliance
 */
class STIGComplianceChecker : public QObject {
    Q_OBJECT
    Q_PROPERTY(int totalChecks READ totalChecks NOTIFY checksUpdated)
    Q_PROPERTY(int passedChecks READ passedChecks NOTIFY checksUpdated)
    Q_PROPERTY(int failedChecks READ failedChecks NOTIFY checksUpdated)
    Q_PROPERTY(QString complianceScore READ complianceScore NOTIFY checksUpdated)

public:
    explicit STIGComplianceChecker(QObject* parent = nullptr);
    ~STIGComplianceChecker();

    /**
     * @brief Initialize checker
     * @param configPath Path to STIG configuration file
     * @return true if initialization successful
     */
    bool initialize(const QString& configPath = "");

    /**
     * @brief Get total number of checks
     */
    int totalChecks() const { return m_checks.size(); }

    /**
     * @brief Get number of passed checks
     */
    int passedChecks() const;

    /**
     * @brief Get number of failed checks
     */
    int failedChecks() const;

    /**
     * @brief Get compliance score (percentage)
     */
    QString complianceScore() const;

    /**
     * @brief Run all STIG checks
     * @return true if all critical checks passed
     */
    bool runAllChecks();

    /**
     * @brief Run checks for specific category
     * @param category Check category (e.g., "network", "audit", "access")
     * @return true if all critical checks in category passed
     */
    bool runCategoryChecks(const QString& category);

    /**
     * @brief Run specific check by ID
     * @param checkId STIG check ID
     * @return Check result
     */
    STIGCheckResult runCheck(const QString& checkId);

    /**
     * @brief Get all checks
     */
    QList<STIGCheck> getAllChecks() const { return m_checks.values(); }

    /**
     * @brief Get check by ID
     */
    STIGCheck getCheck(const QString& checkId) const;

    /**
     * @brief Get failed checks
     */
    QList<STIGCheck> getFailedChecks() const;

    /**
     * @brief Get critical failed checks
     */
    QList<STIGCheck> getCriticalFailures() const;

    /**
     * @brief Generate compliance report
     * @param format Report format ("json", "html", "text")
     * @param outputPath Output file path
     * @return true if report generated successfully
     */
    bool generateReport(const QString& format = "json", const QString& outputPath = "");

    /**
     * @brief Export compliance status
     * @return JSON object with compliance status
     */
    QJsonObject exportComplianceStatus() const;

signals:
    void checksUpdated();
    void checkCompleted(const QString& checkId, STIGCheckResult result);
    void complianceScoreChanged(const QString& score);

private slots:
    void onCheckCompleted(const QString& checkId, STIGCheckResult result);

private:
    /**
     * @brief Load STIG check definitions
     */
    void loadSTIGChecks();

    /**
     * @brief Initialize default STIG checks
     */
    void initializeDefaultChecks();

    /**
     * @brief Network security checks
     */
    STIGCheckResult checkNetworkEncryption();
    STIGCheckResult checkNetworkPolicyEnforcement();
    STIGCheckResult checkNetworkMonitoring();

    /**
     * @brief Audit and accountability checks
     */
    STIGCheckResult checkAuditLogging();
    STIGCheckResult checkAuditLogIntegrity();
    STIGCheckResult checkAuditLogRetention();
    STIGCheckResult checkAuditServiceAvailability();

    /**
     * @brief Access control checks
     */
    STIGCheckResult checkAccessControl();
    STIGCheckResult checkAuthentication();
    STIGCheckResult checkAuthorization();

    /**
     * @brief System configuration checks
     */
    STIGCheckResult checkDebugCodeRemoval();
    STIGCheckResult checkSBOMGeneration();
    STIGCheckResult checkVulnerabilityScanning();
    STIGCheckResult checkServiceHardening();
    STIGCheckResult checkSystemdSecurity();

    /**
     * @brief Helper methods
     */
    QString severityToString(STIGSeverity severity) const;
    QString resultToString(STIGCheckResult result) const;
    STIGSeverity stringToSeverity(const QString& str) const;
    STIGCheckResult stringToResult(const QString& str) const;

    QMap<QString, STIGCheck> m_checks;
    QString m_configPath;
    bool m_initialized;
};

#endif // STIG_COMPLIANCE_CHECKER_H

