#include "milos/ui/bug_reporter.h"
#include "milos/logging/logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>

BugReporter* BugReporter::s_instance = nullptr;

BugReporter::BugReporter(QObject* parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_initialized(false)
{
}

BugReporter::~BugReporter() {
    if (m_networkManager) {
        delete m_networkManager;
    }
}

BugReporter* BugReporter::instance() {
    if (!s_instance) {
        s_instance = new BugReporter();
    }
    return s_instance;
}

bool BugReporter::initialize(const QString& bugTrackerUrl) {
    if (m_initialized) {
        return true;
    }

    m_networkManager = new QNetworkAccessManager(this);
    m_bugTrackerUrl = bugTrackerUrl;

    // Determine bug tracker type from URL
    if (bugTrackerUrl.contains("github.com")) {
        m_bugTrackerType = "github";
    } else if (bugTrackerUrl.contains("jira")) {
        m_bugTrackerType = "jira";
    } else if (bugTrackerUrl.startsWith("mailto:")) {
        m_bugTrackerType = "email";
    } else if (bugTrackerUrl.isEmpty()) {
        m_bugTrackerType = "file";  // Default to local file storage
    } else {
        m_bugTrackerType = "unknown";
    }

    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize("BugReporter");
    }

    m_initialized = true;
    LOG_INFO("Bug reporter initialized");
    return true;
}

bool BugReporter::submitBugReport(const BugReport& report) {
    if (!m_initialized) {
        LOG_ERROR("Bug reporter not initialized");
        return false;
    }

    QString sanitized = sanitizeReport(formatBugReport(report));
    LOG_INFO(QString("Submitting bug report: %1").arg(report.reportId));

    bool success = false;
    if (m_bugTrackerType == "github") {
        success = submitToGitHub(report);
    } else if (m_bugTrackerType == "jira") {
        success = submitToJira(report);
    } else if (m_bugTrackerType == "email") {
        success = submitToEmail(report);
    } else {
        success = submitToLocalFile(report);
    }

    emit bugReportSubmitted(report.reportId, success);
    return success;
}

bool BugReporter::submitCrashReport(const QString& crashReportPath, const QString& userMessage) {
    QFile file(crashReportPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to read crash report: %1").arg(crashReportPath));
        return false;
    }

    QTextStream stream(&file);
    QString crashJson = stream.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(crashJson.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        LOG_ERROR("Invalid crash report format");
        return false;
    }

    QJsonObject crashObj = doc.object();

    BugReport report;
    report.reportId = crashObj.value("crash_id").toString();
    report.title = QString("Crash: %1 - %2")
                   .arg(crashObj.value("application_name").toString())
                   .arg(crashObj.value("signal").toString());
    report.description = QString("Application crashed with signal %1.\n\n%2\n\n%3")
                        .arg(crashObj.value("signal").toString())
                        .arg(crashObj.value("error_message").toString())
                        .arg(userMessage.isEmpty() ? "" : "User message: " + userMessage);
    report.severity = "critical";
    report.category = "crash";
    report.stepsToReproduce << "Application crashed unexpectedly";
    report.expectedBehavior = "Application should not crash";
    report.actualBehavior = crashObj.value("error_message").toString();
    report.systemInfo = crashObj.value("system_info").toObject().toVariantMap();
    report.attachments.append(crashReportPath);

    return submitBugReport(report);
}

bool BugReporter::isBugTrackerAvailable() const {
    return m_initialized && !m_bugTrackerUrl.isEmpty();
}

void BugReporter::onSubmissionFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        LOG_INFO("Bug report submitted successfully");
    } else {
        LOG_ERROR(QString("Bug report submission failed: %1").arg(reply->errorString()));
    }

    reply->deleteLater();
}

bool BugReporter::submitToGitHub(const BugReport& report) {
    // GitHub Issues API
    // Format: POST https://api.github.com/repos/{owner}/{repo}/issues
    
    QString githubToken = qgetenv("GITHUB_TOKEN");
    if (githubToken.isEmpty()) {
        LOG_WARNING("GITHUB_TOKEN not set, cannot submit to GitHub");
        return submitToLocalFile(report);  // Fallback to local file
    }

    // Parse repository from URL
    // URL format: https://github.com/owner/repo
    QUrl url(m_bugTrackerUrl);
    QStringList pathParts = url.path().split('/', Qt::SkipEmptyParts);
    if (pathParts.size() < 2) {
        LOG_ERROR("Invalid GitHub URL format");
        return false;
    }

    QString owner = pathParts[0];
    QString repo = pathParts[1];
    QString apiUrl = QString("https://api.github.com/repos/%1/%2/issues").arg(owner, repo);

    QNetworkRequest request(QUrl(apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("token %1").arg(githubToken).toUtf8());
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    QJsonObject issue;
    issue["title"] = report.title;
    
    QString body = formatBugReport(report);
    issue["body"] = body;
    
    QJsonArray labels;
    labels.append("bug");
    labels.append(report.severity);
    labels.append(report.category);
    issue["labels"] = labels;

    QJsonDocument doc(issue);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &BugReporter::onSubmissionFinished);

    return true;
}

bool BugReporter::submitToJira(const BugReport& report) {
    // Jira REST API
    // Format: POST https://{instance}.atlassian.net/rest/api/3/issue
    
    QString jiraToken = qgetenv("JIRA_API_TOKEN");
    QString jiraEmail = qgetenv("JIRA_EMAIL");
    
    if (jiraToken.isEmpty() || jiraEmail.isEmpty()) {
        LOG_WARNING("JIRA credentials not set, cannot submit to Jira");
        return submitToLocalFile(report);
    }

    QNetworkRequest request(QUrl(m_bugTrackerUrl + "/rest/api/3/issue"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QString auth = QString("%1:%2").arg(jiraEmail, jiraToken).toUtf8().toBase64();
    request.setRawHeader("Authorization", QString("Basic %1").arg(QString::fromUtf8(auth)).toUtf8());

    QJsonObject fields;
    fields["summary"] = report.title;
    fields["description"] = QJsonObject{
        {"type", "doc"},
        {"version", 1},
        {"content", QJsonArray{
            QJsonObject{
                {"type", "paragraph"},
                {"content", QJsonArray{
                    QJsonObject{
                        {"type", "text"},
                        {"text", formatBugReport(report)}
                    }
                }}
            }
        }}
    };
    fields["issuetype"] = QJsonObject{{"name", "Bug"}};
    fields["priority"] = QJsonObject{{"name", report.severity}};

    QJsonObject issue;
    issue["fields"] = fields;

    QJsonDocument doc(issue);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &BugReporter::onSubmissionFinished);

    return true;
}

bool BugReporter::submitToEmail(const BugReport& report) {
    // Email submission via mailto: or SMTP
    // For now, save to file and notify user to send manually
    LOG_INFO("Email submission not yet implemented, saving to file");
    return submitToLocalFile(report);
}

bool BugReporter::submitToLocalFile(const BugReport& report) {
    // Save bug report to local file
    QString reportsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/bug-reports";
    QDir().mkpath(reportsDir);

    QString filename = QString("bug-%1-%2.json")
                      .arg(report.reportId)
                      .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"));
    QString filepath = reportsDir + "/" + filename;

    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << formatBugReport(report);
        file.close();
        
        LOG_INFO(QString("Bug report saved locally: %1").arg(filepath));
        return true;
    } else {
        LOG_ERROR(QString("Failed to save bug report: %1").arg(file.errorString()));
        return false;
    }
}

QString BugReporter::formatBugReport(const BugReport& report) const {
    QString formatted;
    formatted += QString("Bug Report ID: %1\n").arg(report.reportId);
    formatted += QString("Title: %1\n").arg(report.title);
    formatted += QString("Severity: %1\n").arg(report.severity);
    formatted += QString("Category: %1\n").arg(report.category);
    formatted += "\n";
    formatted += "Description:\n";
    formatted += report.description + "\n";
    formatted += "\n";
    
    if (!report.stepsToReproduce.isEmpty()) {
        formatted += "Steps to Reproduce:\n";
        for (int i = 0; i < report.stepsToReproduce.size(); ++i) {
            formatted += QString("%1. %2\n").arg(i + 1).arg(report.stepsToReproduce[i]);
        }
        formatted += "\n";
    }
    
    formatted += QString("Expected Behavior: %1\n").arg(report.expectedBehavior);
    formatted += QString("Actual Behavior: %1\n").arg(report.actualBehavior);
    formatted += "\n";
    
    if (!report.systemInfo.isEmpty()) {
        formatted += "System Information:\n";
        for (auto it = report.systemInfo.begin(); it != report.systemInfo.end(); ++it) {
            formatted += QString("  %1: %2\n").arg(it.key(), it.value().toString());
        }
    }
    
    return formatted;
}

QString BugReporter::sanitizeReport(const QString& report) const {
    // Remove sensitive information
    QString sanitized = report;
    
    // Remove potential passwords/tokens
    QRegularExpression passwordRegex(R"((password|token|key|secret)\s*[:=]\s*["']?[^"'\s]+["']?)", QRegularExpression::CaseInsensitiveOption);
    sanitized.replace(passwordRegex, "\\1: [REDACTED]");
    
    // Remove file paths with user information
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    sanitized.replace(homePath, "[HOME]");
    
    return sanitized;
}


#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>

BugReporter* BugReporter::s_instance = nullptr;

BugReporter::BugReporter(QObject* parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_initialized(false)
{
}

BugReporter::~BugReporter() {
    if (m_networkManager) {
        delete m_networkManager;
    }
}

BugReporter* BugReporter::instance() {
    if (!s_instance) {
        s_instance = new BugReporter();
    }
    return s_instance;
}

bool BugReporter::initialize(const QString& bugTrackerUrl) {
    if (m_initialized) {
        return true;
    }

    m_networkManager = new QNetworkAccessManager(this);
    m_bugTrackerUrl = bugTrackerUrl;

    // Determine bug tracker type from URL
    if (bugTrackerUrl.contains("github.com")) {
        m_bugTrackerType = "github";
    } else if (bugTrackerUrl.contains("jira")) {
        m_bugTrackerType = "jira";
    } else if (bugTrackerUrl.startsWith("mailto:")) {
        m_bugTrackerType = "email";
    } else if (bugTrackerUrl.isEmpty()) {
        m_bugTrackerType = "file";  // Default to local file storage
    } else {
        m_bugTrackerType = "unknown";
    }

    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize("BugReporter");
    }

    m_initialized = true;
    LOG_INFO("Bug reporter initialized");
    return true;
}

bool BugReporter::submitBugReport(const BugReport& report) {
    if (!m_initialized) {
        LOG_ERROR("Bug reporter not initialized");
        return false;
    }

    QString sanitized = sanitizeReport(formatBugReport(report));
    LOG_INFO(QString("Submitting bug report: %1").arg(report.reportId));

    bool success = false;
    if (m_bugTrackerType == "github") {
        success = submitToGitHub(report);
    } else if (m_bugTrackerType == "jira") {
        success = submitToJira(report);
    } else if (m_bugTrackerType == "email") {
        success = submitToEmail(report);
    } else {
        success = submitToLocalFile(report);
    }

    emit bugReportSubmitted(report.reportId, success);
    return success;
}

bool BugReporter::submitCrashReport(const QString& crashReportPath, const QString& userMessage) {
    QFile file(crashReportPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to read crash report: %1").arg(crashReportPath));
        return false;
    }

    QTextStream stream(&file);
    QString crashJson = stream.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(crashJson.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        LOG_ERROR("Invalid crash report format");
        return false;
    }

    QJsonObject crashObj = doc.object();

    BugReport report;
    report.reportId = crashObj.value("crash_id").toString();
    report.title = QString("Crash: %1 - %2")
                   .arg(crashObj.value("application_name").toString())
                   .arg(crashObj.value("signal").toString());
    report.description = QString("Application crashed with signal %1.\n\n%2\n\n%3")
                        .arg(crashObj.value("signal").toString())
                        .arg(crashObj.value("error_message").toString())
                        .arg(userMessage.isEmpty() ? "" : "User message: " + userMessage);
    report.severity = "critical";
    report.category = "crash";
    report.stepsToReproduce << "Application crashed unexpectedly";
    report.expectedBehavior = "Application should not crash";
    report.actualBehavior = crashObj.value("error_message").toString();
    report.systemInfo = crashObj.value("system_info").toObject().toVariantMap();
    report.attachments.append(crashReportPath);

    return submitBugReport(report);
}

bool BugReporter::isBugTrackerAvailable() const {
    return m_initialized && !m_bugTrackerUrl.isEmpty();
}

void BugReporter::onSubmissionFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        LOG_INFO("Bug report submitted successfully");
    } else {
        LOG_ERROR(QString("Bug report submission failed: %1").arg(reply->errorString()));
    }

    reply->deleteLater();
}

bool BugReporter::submitToGitHub(const BugReport& report) {
    // GitHub Issues API
    // Format: POST https://api.github.com/repos/{owner}/{repo}/issues
    
    QString githubToken = qgetenv("GITHUB_TOKEN");
    if (githubToken.isEmpty()) {
        LOG_WARNING("GITHUB_TOKEN not set, cannot submit to GitHub");
        return submitToLocalFile(report);  // Fallback to local file
    }

    // Parse repository from URL
    // URL format: https://github.com/owner/repo
    QUrl url(m_bugTrackerUrl);
    QStringList pathParts = url.path().split('/', Qt::SkipEmptyParts);
    if (pathParts.size() < 2) {
        LOG_ERROR("Invalid GitHub URL format");
        return false;
    }

    QString owner = pathParts[0];
    QString repo = pathParts[1];
    QString apiUrl = QString("https://api.github.com/repos/%1/%2/issues").arg(owner, repo);

    QNetworkRequest request(QUrl(apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("token %1").arg(githubToken).toUtf8());
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    QJsonObject issue;
    issue["title"] = report.title;
    
    QString body = formatBugReport(report);
    issue["body"] = body;
    
    QJsonArray labels;
    labels.append("bug");
    labels.append(report.severity);
    labels.append(report.category);
    issue["labels"] = labels;

    QJsonDocument doc(issue);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &BugReporter::onSubmissionFinished);

    return true;
}

bool BugReporter::submitToJira(const BugReport& report) {
    // Jira REST API
    // Format: POST https://{instance}.atlassian.net/rest/api/3/issue
    
    QString jiraToken = qgetenv("JIRA_API_TOKEN");
    QString jiraEmail = qgetenv("JIRA_EMAIL");
    
    if (jiraToken.isEmpty() || jiraEmail.isEmpty()) {
        LOG_WARNING("JIRA credentials not set, cannot submit to Jira");
        return submitToLocalFile(report);
    }

    QNetworkRequest request(QUrl(m_bugTrackerUrl + "/rest/api/3/issue"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QString auth = QString("%1:%2").arg(jiraEmail, jiraToken).toUtf8().toBase64();
    request.setRawHeader("Authorization", QString("Basic %1").arg(QString::fromUtf8(auth)).toUtf8());

    QJsonObject fields;
    fields["summary"] = report.title;
    fields["description"] = QJsonObject{
        {"type", "doc"},
        {"version", 1},
        {"content", QJsonArray{
            QJsonObject{
                {"type", "paragraph"},
                {"content", QJsonArray{
                    QJsonObject{
                        {"type", "text"},
                        {"text", formatBugReport(report)}
                    }
                }}
            }
        }}
    };
    fields["issuetype"] = QJsonObject{{"name", "Bug"}};
    fields["priority"] = QJsonObject{{"name", report.severity}};

    QJsonObject issue;
    issue["fields"] = fields;

    QJsonDocument doc(issue);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &BugReporter::onSubmissionFinished);

    return true;
}

bool BugReporter::submitToEmail(const BugReport& report) {
    // Email submission via mailto: or SMTP
    // For now, save to file and notify user to send manually
    LOG_INFO("Email submission not yet implemented, saving to file");
    return submitToLocalFile(report);
}

bool BugReporter::submitToLocalFile(const BugReport& report) {
    // Save bug report to local file
    QString reportsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/bug-reports";
    QDir().mkpath(reportsDir);

    QString filename = QString("bug-%1-%2.json")
                      .arg(report.reportId)
                      .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"));
    QString filepath = reportsDir + "/" + filename;

    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << formatBugReport(report);
        file.close();
        
        LOG_INFO(QString("Bug report saved locally: %1").arg(filepath));
        return true;
    } else {
        LOG_ERROR(QString("Failed to save bug report: %1").arg(file.errorString()));
        return false;
    }
}

QString BugReporter::formatBugReport(const BugReport& report) const {
    QString formatted;
    formatted += QString("Bug Report ID: %1\n").arg(report.reportId);
    formatted += QString("Title: %1\n").arg(report.title);
    formatted += QString("Severity: %1\n").arg(report.severity);
    formatted += QString("Category: %1\n").arg(report.category);
    formatted += "\n";
    formatted += "Description:\n";
    formatted += report.description + "\n";
    formatted += "\n";
    
    if (!report.stepsToReproduce.isEmpty()) {
        formatted += "Steps to Reproduce:\n";
        for (int i = 0; i < report.stepsToReproduce.size(); ++i) {
            formatted += QString("%1. %2\n").arg(i + 1).arg(report.stepsToReproduce[i]);
        }
        formatted += "\n";
    }
    
    formatted += QString("Expected Behavior: %1\n").arg(report.expectedBehavior);
    formatted += QString("Actual Behavior: %1\n").arg(report.actualBehavior);
    formatted += "\n";
    
    if (!report.systemInfo.isEmpty()) {
        formatted += "System Information:\n";
        for (auto it = report.systemInfo.begin(); it != report.systemInfo.end(); ++it) {
            formatted += QString("  %1: %2\n").arg(it.key(), it.value().toString());
        }
    }
    
    return formatted;
}

QString BugReporter::sanitizeReport(const QString& report) const {
    // Remove sensitive information
    QString sanitized = report;
    
    // Remove potential passwords/tokens
    QRegularExpression passwordRegex(R"((password|token|key|secret)\s*[:=]\s*["']?[^"'\s]+["']?)", QRegularExpression::CaseInsensitiveOption);
    sanitized.replace(passwordRegex, "\\1: [REDACTED]");
    
    // Remove file paths with user information
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    sanitized.replace(homePath, "[HOME]");
    
    return sanitized;
}

