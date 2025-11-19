#ifndef EXPORT_MANAGER_H
#define EXPORT_MANAGER_H

#include "report_generator.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Export Format
 */
enum class ExportFormat {
    CSV,
    PDF,
    JSON
};

/**
 * @brief Export Manager
 * 
 * Manages export of reports and data to various formats.
 */
class ExportManager : public QObject {
    Q_OBJECT

public:
    explicit ExportManager(QObject* parent = nullptr);
    ~ExportManager();

    /**
     * @brief Initialize export manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Export attendance report
     * @param report Attendance report
     * @param format Export format
     * @param filePath Output file path
     * @return true if export successful, false otherwise
     */
    bool exportAttendanceReport(const AttendanceReport& report,
                               ExportFormat format,
                               const QString& filePath);

    /**
     * @brief Export access control log
     * @param logData List of access control events
     * @param format Export format
     * @param filePath Output file path
     * @return true if export successful, false otherwise
     */
    bool exportAccessControlLog(const QList<QVariantMap>& logData,
                                ExportFormat format,
                                const QString& filePath);

    /**
     * @brief Export compliance report
     * @param complianceData Compliance report data
     * @param format Export format
     * @param filePath Output file path
     * @return true if export successful, false otherwise
     */
    bool exportComplianceReport(const QVariantMap& complianceData,
                               ExportFormat format,
                               const QString& filePath);

Q_SIGNALS:
    /**
     * @brief Emitted when export is completed
     */
    void exportCompleted(const QString& filePath);

    /**
     * @brief Emitted when export fails
     */
    void exportFailed(const QString& error);

private:
    bool exportToCSV(const QVariantMap& data, const QString& filePath);
    bool exportToPDF(const QVariantMap& data, const QString& filePath);
    bool exportToJSON(const QVariantMap& data, const QString& filePath);
};

#endif // EXPORT_MANAGER_H

