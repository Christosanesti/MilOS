#include "stig_compliance_checker.h"
#include "milos/logging/logger.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("milos-stig-checker");
    QCoreApplication::setApplicationVersion("1.0.0");

    // Initialize logger
    Logger::instance()->initialize("stig-checker");

    QCommandLineParser parser;
    parser.setApplicationDescription("STIG Compliance Checker for MilOS");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption allOption(QStringList() << "a" << "all",
                                  "Run all STIG compliance checks");
    parser.addOption(allOption);

    QCommandLineOption categoryOption(QStringList() << "c" << "category",
                                       "Run checks for specific category (network, audit, access, system)",
                                       "category");
    parser.addOption(categoryOption);

    QCommandLineOption checkOption(QStringList() << "i" << "check-id",
                                    "Run specific check by ID",
                                    "check-id");
    parser.addOption(checkOption);

    QCommandLineOption reportOption(QStringList() << "r" << "report",
                                     "Generate compliance report",
                                     "format");
    parser.addOption(reportOption);

    QCommandLineOption outputOption(QStringList() << "o" << "output",
                                     "Output file path for report",
                                     "path");
    parser.addOption(outputOption);

    QCommandLineOption listOption(QStringList() << "l" << "list",
                                   "List all available checks");
    parser.addOption(listOption);

    QCommandLineOption jsonOption(QStringList() << "j" << "json",
                                   "Output results in JSON format");
    parser.addOption(jsonOption);

    parser.process(app);

    STIGComplianceChecker checker;
    if (!checker.initialize()) {
        std::cerr << "Failed to initialize STIG Compliance Checker" << std::endl;
        return 1;
    }

    // List checks
    if (parser.isSet(listOption)) {
        std::cout << "Available STIG Checks:\n" << std::endl;
        for (const auto& check : checker.getAllChecks()) {
            std::cout << "  " << check.checkId.toStdString() << ": " 
                      << check.title.toStdString() << std::endl;
            QString severityStr;
            switch (check.severity) {
                case STIGSeverity::Critical: severityStr = "Critical"; break;
                case STIGSeverity::High: severityStr = "High"; break;
                case STIGSeverity::Medium: severityStr = "Medium"; break;
                case STIGSeverity::Low: severityStr = "Low"; break;
                case STIGSeverity::Informational: severityStr = "Informational"; break;
            }
            QString resultStr;
            switch (check.result) {
                case STIGCheckResult::Pass: resultStr = "Pass"; break;
                case STIGCheckResult::Fail: resultStr = "Fail"; break;
                case STIGCheckResult::NotApplicable: resultStr = "NotApplicable"; break;
                case STIGCheckResult::NotChecked: resultStr = "NotChecked"; break;
                case STIGCheckResult::Error: resultStr = "Error"; break;
            }
            std::cout << "    Severity: " << severityStr.toStdString() << std::endl;
            std::cout << "    Status: " << resultStr.toStdString() << std::endl;
            std::cout << std::endl;
        }
        return 0;
    }

    // Run checks
    bool success = false;
    if (parser.isSet(allOption)) {
        success = checker.runAllChecks();
    } else if (parser.isSet(categoryOption)) {
        QString category = parser.value(categoryOption);
        success = checker.runCategoryChecks(category);
    } else if (parser.isSet(checkOption)) {
        QString checkId = parser.value(checkOption);
        STIGCheckResult result = checker.runCheck(checkId);
        success = (result == STIGCheckResult::Pass);
    } else {
        // Default: run all checks
        success = checker.runAllChecks();
    }

    // Generate report
    if (parser.isSet(reportOption)) {
        QString format = parser.value(reportOption);
        QString outputPath = parser.isSet(outputOption) ? parser.value(outputOption) : "";
        if (!checker.generateReport(format, outputPath)) {
            std::cerr << "Failed to generate report" << std::endl;
            return 1;
        }
        std::cout << "Report generated successfully" << std::endl;
    }

    // Output results
    if (parser.isSet(jsonOption)) {
        QJsonObject status = checker.exportComplianceStatus();
        QJsonDocument doc(status);
        std::cout << doc.toJson().toStdString() << std::endl;
    } else {
        std::cout << "\nSTIG Compliance Check Results\n";
        std::cout << "==============================\n\n";
        std::cout << "Compliance Score: " << checker.complianceScore().toStdString() << std::endl;
        std::cout << "Total Checks: " << checker.totalChecks() << std::endl;
        std::cout << "Passed: " << checker.passedChecks() << std::endl;
        std::cout << "Failed: " << checker.failedChecks() << std::endl;
        std::cout << std::endl;

        QList<STIGCheck> failed = checker.getFailedChecks();
        if (!failed.isEmpty()) {
            std::cout << "Failed Checks:\n";
            for (const auto& check : failed) {
                std::cout << "  - " << check.checkId.toStdString() << ": " 
                          << check.title.toStdString() << std::endl;
                std::cout << "    " << check.message.toStdString() << std::endl;
                if (!check.remediation.isEmpty()) {
                    std::cout << "    Remediation: " << check.remediation.toStdString() << std::endl;
                }
                std::cout << std::endl;
            }
        }

        QList<STIGCheck> critical = checker.getCriticalFailures();
        if (!critical.isEmpty()) {
            std::cout << "Critical Failures:\n";
            for (const auto& check : critical) {
                std::cout << "  - " << check.checkId.toStdString() << ": " 
                          << check.title.toStdString() << std::endl;
                std::cout << "    " << check.message.toStdString() << std::endl;
                std::cout << std::endl;
            }
        }
    }

    return success ? 0 : 1;
}

