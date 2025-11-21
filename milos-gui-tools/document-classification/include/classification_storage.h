#ifndef CLASSIFICATION_STORAGE_H
#define CLASSIFICATION_STORAGE_H

#include "document_classifier.h"
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QList>

/**
 * @brief Classification Storage
 * 
 * Stores classification metadata in SQLite database.
 */
class ClassificationStorage : public QObject {
    Q_OBJECT

public:
    explicit ClassificationStorage(QObject* parent = nullptr);
    ~ClassificationStorage();

    /**
     * @brief Initialize classification storage
     * @param dbPath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Store classification metadata
     * @param filePath File path
     * @param metadata Classification metadata
     * @return true if store successful, false otherwise
     */
    bool storeClassification(const QString& filePath, const ClassificationMetadata& metadata);

    /**
     * @brief Retrieve classification metadata
     * @param filePath File path
     * @return Classification metadata or empty if not found
     */
    ClassificationMetadata retrieveClassification(const QString& filePath) const;

    /**
     * @brief Get all classified files
     * @return List of file paths
     */
    QList<QString> getAllClassifiedFiles() const;

    /**
     * @brief Get files by classification level
     * @param level Classification level
     * @return List of file paths
     */
    QList<QString> getFilesByLevel(ClassificationLevel level) const;

private:
    QSqlDatabase m_database;
    bool createTables();
    ClassificationMetadata metadataFromQuery(const QSqlQuery& query) const;
};

#endif // CLASSIFICATION_STORAGE_H

