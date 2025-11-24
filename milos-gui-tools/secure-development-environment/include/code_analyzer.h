#ifndef CODE_ANALYZER_H
#define CODE_ANALYZER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QList>

/**
 * @brief Security Issue
 */
struct SecurityIssue {
    QString id;
    QString severity;  // "low", "medium", "high", "critical"
    QString type;  // "vulnerability", "weakness", "misconfiguration"
    QString file;
    int line;
    QString description;
    QString recommendation;
};

/**
 * @brief Code Analyzer
 * 
 * Analyzes code for security issues.
 */
class CodeAnalyzer : public QObject {
    Q_OBJECT

public:
    explicit CodeAnalyzer(QObject* parent = nullptr);
    ~CodeAnalyzer();

    /**
     * @brief Initialize code analyzer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Analyze code file
     * @param filePath File path
     * @return List of security issues
     */
    QList<SecurityIssue> analyzeFile(const QString& filePath);

    /**
     * @brief Analyze code directory
     * @param directoryPath Directory path
     * @return List of security issues
     */
    QList<SecurityIssue> analyzeDirectory(const QString& directoryPath);

    /**
     * @brief Get security statistics
     * @return Security statistics
     */
    QVariantMap getSecurityStatistics() const;

Q_SIGNALS:
    /**
     * @brief Emitted when security issue is found
     */
    void securityIssueFound(const SecurityIssue& issue);

private:
    QList<SecurityIssue> m_issues;
    
    SecurityIssue checkForVulnerability(const QString& code, const QString& filePath, int line) const;
    QString generateIssueId() const;
};

Q_DECLARE_METATYPE(SecurityIssue)

#endif // CODE_ANALYZER_H






