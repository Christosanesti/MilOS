#ifndef NETWORK_FORENSICS_H
#define NETWORK_FORENSICS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>
#include <QSqlDatabase>

/**
 * @brief Forensic Evidence
 */
struct ForensicEvidence {
    QString id;
    QString type;  // "packet", "flow", "session", "file"
    QString source;
    QDateTime timestamp;
    QByteArray data;
    QVariantMap metadata;
};

/**
 * @brief Network Forensics
 * 
 * Network forensics and investigation capabilities.
 */
class NetworkForensics : public QObject {
    Q_OBJECT

public:
    explicit NetworkForensics(QObject* parent = nullptr);
    ~NetworkForensics();

    /**
     * @brief Initialize network forensics
     * @param dbPath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Capture forensic evidence
     * @param evidence Forensic evidence
     * @return true if capture successful, false otherwise
     */
    bool captureEvidence(const ForensicEvidence& evidence);

    /**
     * @brief Get forensic evidence
     * @param evidenceId Evidence ID
     * @return Forensic evidence or empty if not found
     */
    ForensicEvidence getEvidence(const QString& evidenceId) const;

    /**
     * @brief Search forensic evidence
     * @param criteria Search criteria
     * @return List of evidence IDs
     */
    QStringList searchEvidence(const QVariantMap& criteria) const;

    /**
     * @brief Generate forensic report
     * @param startTime Start time
     * @param endTime End time
     * @param filters Filters
     * @return Report data
     */
    QVariantMap generateReport(const QDateTime& startTime, const QDateTime& endTime, const QVariantMap& filters = QVariantMap()) const;

    /**
     * @brief Analyze network flow
     * @param flowId Flow ID
     * @return Analysis results
     */
    QVariantMap analyzeFlow(const QString& flowId) const;

private:
    QSqlDatabase m_database;
    bool createTables();
    ForensicEvidence evidenceFromQuery(const QSqlQuery& query) const;
};

Q_DECLARE_METATYPE(ForensicEvidence)

#endif // NETWORK_FORENSICS_H

