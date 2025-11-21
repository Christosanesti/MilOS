#ifndef DOCUMENT_CLASSIFICATION_AUDIT_LOGGER_H
#define DOCUMENT_CLASSIFICATION_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logClassification(const QString& filePath, const QString& level);
    void logOperationBlocked(const QString& filePath, const QString& operation, const QString& reason);

private:
    QString generateEventId() const;
};

#endif // DOCUMENT_CLASSIFICATION_AUDIT_LOGGER_H

