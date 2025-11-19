#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QList>

/**
 * @brief Report Type
 */
enum class ReportType {
    Daily,
    Weekly,
    Monthly,
    Custom
};

/**
 * @brief Attendance Report
 */
struct AttendanceReport {
    QString reportId;
    ReportType reportType;
    QDateTime startDate;
    QDateTime endDate;
    QList<QVariantMap> attendanceRecords;
    QList<QVariantMap> lateArrivals;
    QList<QVariantMap> absences;
    QVariantMap statistics;
};

/**
 * @brief Report Generator
 * 
 * Generates attendance and access control reports.
 */
class ReportGenerator : public QObject {
    Q_OBJECT

public:
    explicit ReportGenerator(QObject* parent = nullptr);
    ~ReportGenerator();

    /**
     * @brief Initialize report generator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Generate attendance report
     * @param reportType Report type
     * @param startDate Start date
     * @param endDate End date
     * @param personnelId Optional personnel ID filter
     * @return Attendance report
     */
    AttendanceReport generateAttendanceReport(ReportType reportType,
                                             const QDateTime& startDate,
                                             const QDateTime& endDate,
                                             const QString& personnelId = QString());

    /**
     * @brief Generate access control audit log
     * @param startDate Start date
     * @param endDate End date
     * @param personnelId Optional personnel ID filter
     * @param location Optional location filter
     * @return List of access control events
     */
    QList<QVariantMap> generateAccessControlLog(const QDateTime& startDate,
                                                const QDateTime& endDate,
                                                const QString& personnelId = QString(),
                                                const QString& location = QString());

    /**
     * @brief Generate compliance report
     * @param startDate Start date
     * @param endDate End date
     * @return Compliance report data
     */
    QVariantMap generateComplianceReport(const QDateTime& startDate, const QDateTime& endDate);

    /**
     * @brief Track late arrivals
     * @param personnelId Personnel ID
     * @param expectedTime Expected arrival time
     * @param actualTime Actual arrival time
     * @return Late arrival record
     */
    QVariantMap trackLateArrival(const QString& personnelId,
                                 const QDateTime& expectedTime,
                                 const QDateTime& actualTime);

    /**
     * @brief Track absence
     * @param personnelId Personnel ID
     * @param date Date of absence
     * @param reason Reason for absence
     * @return Absence record
     */
    QVariantMap trackAbsence(const QString& personnelId,
                            const QDateTime& date,
                            const QString& reason);

Q_SIGNALS:
    /**
     * @brief Emitted when report is generated
     */
    void reportGenerated(const QString& reportId);

private:
    QString generateReportId() const;
    QVariantMap calculateStatistics(const QList<QVariantMap>& records) const;
};

#endif // REPORT_GENERATOR_H

