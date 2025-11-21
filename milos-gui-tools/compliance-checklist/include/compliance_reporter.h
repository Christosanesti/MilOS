#ifndef COMPLIANCE_REPORTER_H
#define COMPLIANCE_REPORTER_H

#include "compliance_checklist.h"
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief Compliance Reporter
 * 
 * Generates compliance reports and documentation.
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
     * @param checklistId Checklist ID
     * @param format Report format ("json", "html", "pdf")
     * @return Report file path or empty if failed
     */
    QString generateReport(const QString& checklistId, const QString& format = "json");

    /**
     * @brief Generate gap analysis
     * @param checklistId Checklist ID
     * @return Gap analysis data
     */
    QVariantMap generateGapAnalysis(const QString& checklistId) const;

    /**
     * @brief Export checklist for audit
     * @param checklistId Checklist ID
     * @param outputPath Output file path
     * @return true if export successful, false otherwise
     */
    bool exportForAudit(const QString& checklistId, const QString& outputPath) const;

private:
    QString generateJSONReport(const ComplianceChecklist& checklist) const;
    QString generateHTMLReport(const ComplianceChecklist& checklist) const;
    QVariantMap calculateGaps(const ComplianceChecklist& checklist) const;
};

#endif // COMPLIANCE_REPORTER_H

