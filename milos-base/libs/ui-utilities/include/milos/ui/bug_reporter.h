#ifndef MILOS_UI_BUG_REPORTER_H
#define MILOS_UI_BUG_REPORTER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief Bug Reporter
 * 
 * Submits bug reports and crash reports to bug tracking systems.
 */
class BugReporter : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Bug report information
     */
    struct BugReport {
        QString reportId;
        QString title;
        QString description;
        QString severity;  // "critical", "high", "medium", "low"
        QString category;  // "crash", "bug", "feature-request", "security"
        QStringList stepsToReproduce;
        QString expectedBehavior;
        QString actualBehavior;
        QVariantMap systemInfo;
        QStringList attachments;  // File paths
        QString reporterEmail;  // Optional
    };

    explicit BugReporter(QObject* parent = nullptr);
    ~BugReporter();

    /**
     * @brief Get singleton instance
     */
    static BugReporter* instance();

    /**
     * @brief Initialize bug reporter
     * @param bugTrackerUrl Bug tracking system URL (optional)
     * @return true if successful
     */
    bool initialize(const QString& bugTrackerUrl = "");

    /**
     * @brief Submit bug report
     * @param report Bug report information
     * @return true if submission initiated
     */
    Q_INVOKABLE bool submitBugReport(const BugReport& report);

    /**
     * @brief Submit bug report from crash info
     * @param crashReportPath Path to crash report file
     * @param userMessage Additional user message
     * @return true if submission initiated
     */
    Q_INVOKABLE bool submitCrashReport(const QString& crashReportPath, const QString& userMessage = "");

    /**
     * @brief Check if bug tracker is available
     */
    Q_INVOKABLE bool isBugTrackerAvailable() const;

Q_SIGNALS:
    /**
     * @brief Emitted when bug report is submitted
     */
    void bugReportSubmitted(const QString& reportId, bool success, const QString& errorMessage = "");

private slots:
    void onSubmissionFinished();

private:
    bool submitToGitHub(const BugReport& report);
    bool submitToJira(const BugReport& report);
    bool submitToEmail(const BugReport& report);
    bool submitToLocalFile(const BugReport& report);
    QString formatBugReport(const BugReport& report) const;
    QString sanitizeReport(const QString& report) const;

    static BugReporter* s_instance;
    QNetworkAccessManager* m_networkManager;
    QString m_bugTrackerUrl;
    QString m_bugTrackerType;  // "github", "jira", "email", "file"
    bool m_initialized;
};

Q_DECLARE_METATYPE(BugReporter::BugReport)

#endif // MILOS_UI_BUG_REPORTER_H



#define MILOS_UI_BUG_REPORTER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief Bug Reporter
 * 
 * Submits bug reports and crash reports to bug tracking systems.
 */
class BugReporter : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Bug report information
     */
    struct BugReport {
        QString reportId;
        QString title;
        QString description;
        QString severity;  // "critical", "high", "medium", "low"
        QString category;  // "crash", "bug", "feature-request", "security"
        QStringList stepsToReproduce;
        QString expectedBehavior;
        QString actualBehavior;
        QVariantMap systemInfo;
        QStringList attachments;  // File paths
        QString reporterEmail;  // Optional
    };

    explicit BugReporter(QObject* parent = nullptr);
    ~BugReporter();

    /**
     * @brief Get singleton instance
     */
    static BugReporter* instance();

    /**
     * @brief Initialize bug reporter
     * @param bugTrackerUrl Bug tracking system URL (optional)
     * @return true if successful
     */
    bool initialize(const QString& bugTrackerUrl = "");

    /**
     * @brief Submit bug report
     * @param report Bug report information
     * @return true if submission initiated
     */
    Q_INVOKABLE bool submitBugReport(const BugReport& report);

    /**
     * @brief Submit bug report from crash info
     * @param crashReportPath Path to crash report file
     * @param userMessage Additional user message
     * @return true if submission initiated
     */
    Q_INVOKABLE bool submitCrashReport(const QString& crashReportPath, const QString& userMessage = "");

    /**
     * @brief Check if bug tracker is available
     */
    Q_INVOKABLE bool isBugTrackerAvailable() const;

Q_SIGNALS:
    /**
     * @brief Emitted when bug report is submitted
     */
    void bugReportSubmitted(const QString& reportId, bool success, const QString& errorMessage = "");

private slots:
    void onSubmissionFinished();

private:
    bool submitToGitHub(const BugReport& report);
    bool submitToJira(const BugReport& report);
    bool submitToEmail(const BugReport& report);
    bool submitToLocalFile(const BugReport& report);
    QString formatBugReport(const BugReport& report) const;
    QString sanitizeReport(const QString& report) const;

    static BugReporter* s_instance;
    QNetworkAccessManager* m_networkManager;
    QString m_bugTrackerUrl;
    QString m_bugTrackerType;  // "github", "jira", "email", "file"
    bool m_initialized;
};

Q_DECLARE_METATYPE(BugReporter::BugReport)

#endif // MILOS_UI_BUG_REPORTER_H


