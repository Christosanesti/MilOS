#include "export_manager.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QPrinter>
#include <QPainter>

ExportManager::ExportManager(QObject* parent)
    : QObject(parent)
{
}

ExportManager::~ExportManager() {
}

bool ExportManager::initialize() {
    return true;
}

bool ExportManager::exportAttendanceReport(const AttendanceReport& report,
                                          ExportFormat format,
                                          const QString& filePath) {
    QVariantMap data;
    data["report_id"] = report.reportId;
    data["report_type"] = static_cast<int>(report.reportType);
    data["start_date"] = report.startDate.toString(Qt::ISODate);
    data["end_date"] = report.endDate.toString(Qt::ISODate);
    data["attendance_records"] = QVariant::fromValue(report.attendanceRecords);
    data["late_arrivals"] = QVariant::fromValue(report.lateArrivals);
    data["absences"] = QVariant::fromValue(report.absences);
    data["statistics"] = report.statistics;
    
    switch (format) {
        case ExportFormat::CSV:
            return exportToCSV(data, filePath);
        case ExportFormat::PDF:
            return exportToPDF(data, filePath);
        case ExportFormat::JSON:
            return exportToJSON(data, filePath);
        default:
            return false;
    }
}

bool ExportManager::exportAccessControlLog(const QList<QVariantMap>& logData,
                                          ExportFormat format,
                                          const QString& filePath) {
    QVariantMap data;
    data["log_entries"] = QVariant::fromValue(logData);
    
    switch (format) {
        case ExportFormat::CSV:
            return exportToCSV(data, filePath);
        case ExportFormat::PDF:
            return exportToPDF(data, filePath);
        case ExportFormat::JSON:
            return exportToJSON(data, filePath);
        default:
            return false;
    }
}

bool ExportManager::exportComplianceReport(const QVariantMap& complianceData,
                                          ExportFormat format,
                                          const QString& filePath) {
    switch (format) {
        case ExportFormat::CSV:
            return exportToCSV(complianceData, filePath);
        case ExportFormat::PDF:
            return exportToPDF(complianceData, filePath);
        case ExportFormat::JSON:
            return exportToJSON(complianceData, filePath);
        default:
            return false;
    }
}

bool ExportManager::exportToCSV(const QVariantMap& data, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Failed to open file for writing");
        return false;
    }
    
    QTextStream out(&file);
    
    // Write CSV header
    out << "Field,Value\n";
    
    // Write data
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        out << it.key() << "," << it.value().toString() << "\n";
    }
    
    file.close();
    emit exportCompleted(filePath);
    return true;
}

bool ExportManager::exportToPDF(const QVariantMap& data, const QString& filePath) {
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        emit exportFailed("Failed to initialize PDF printer");
        return false;
    }
    
    // Draw PDF content
    painter.setFont(QFont("Arial", 12));
    painter.drawText(100, 100, "Report");
    
    int y = 150;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        painter.drawText(100, y, it.key() + ": " + it.value().toString());
        y += 30;
    }
    
    painter.end();
    emit exportCompleted(filePath);
    return true;
}

bool ExportManager::exportToJSON(const QVariantMap& data, const QString& filePath) {
    QJsonObject jsonObj = QJsonObject::fromVariantMap(data);
    QJsonDocument doc(jsonObj);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Failed to open file for writing");
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    emit exportCompleted(filePath);
    return true;
}

