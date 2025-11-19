#ifndef COMPLIANCE_REPORTER_H
#define COMPLIANCE_REPORTER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QList>

/**
 * @brief Compliance Standard
 */
enum class ComplianceStandard {
    ISO27001,
    NIST,
    HIPAA,
    Custom
};

/**
 * @brief Compliance Reporter
 * 
 * Generates compliance reports for various standards.
 */
class ComplianceReporter : public QObject {
    Q_OBJECT

public:
    explicit ComplianceReporter(QObject* parent = nullptr);
    ~ComplianceReporter();

    /**
     * @brief Initialize compliance reporter
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Generate compliance report
     * @param standard Compliance standard
     * @param startDate Start date
     * @param endDate End date
     * @return Compliance report data
     */
    QVariantMap generateComplianceReport(ComplianceStandard standard,
                                       const QDateTime& startDate,
                                       const QDateTime& endDate);

    /**
     * @brief Check compliance status
     * @param standard Compliance standard
     * @return Compliance status map
     */
    QVariantMap checkComplianceStatus(ComplianceStandard standard) const;

Q_SIGNALS:
    /**
     * @brief Emitted when compliance report is generated
     */
    void complianceReportGenerated(const QString& reportId);

private:
    QVariantMap collectComplianceData(ComplianceStandard standard,
                                     const QDateTime& startDate,
                                     const QDateTime& endDate) const;
};

#endif // COMPLIANCE_REPORTER_H

