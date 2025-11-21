#ifndef DOCUMENT_CLASSIFIER_H
#define DOCUMENT_CLASSIFIER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Classification Level
 */
enum class ClassificationLevel {
    Unclassified,
    Confidential,
    Secret,
    TopSecret
};

/**
 * @brief Classification Metadata
 */
struct ClassificationMetadata {
    ClassificationLevel level;
    QString category;
    QString owner;
    QDateTime classifiedAt;
    QString classifiedBy;
    QStringList handlingInstructions;
    QVariantMap customMetadata;
};

/**
 * @brief Document Classifier
 * 
 * Automatically classifies documents based on content analysis.
 */
class DocumentClassifier : public QObject {
    Q_OBJECT
    Q_PROPERTY(ClassificationLevel defaultLevel READ defaultLevel WRITE setDefaultLevel NOTIFY defaultLevelChanged)

public:
    explicit DocumentClassifier(QObject* parent = nullptr);
    ~DocumentClassifier();

    /**
     * @brief Initialize document classifier
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Classify document
     * @param filePath File path
     * @param content Document content (optional, will be read if empty)
     * @return Classification metadata
     */
    ClassificationMetadata classifyDocument(const QString& filePath, const QString& content = QString());

    /**
     * @brief Get classification level for file
     * @param filePath File path
     * @return Classification level
     */
    ClassificationLevel getClassificationLevel(const QString& filePath) const;

    /**
     * @brief Get classification metadata for file
     * @param filePath File path
     * @return Classification metadata
     */
    ClassificationMetadata getClassificationMetadata(const QString& filePath) const;

    /**
     * @brief Set classification for file
     * @param filePath File path
     * @param metadata Classification metadata
     * @return true if set successful, false otherwise
     */
    bool setClassification(const QString& filePath, const ClassificationMetadata& metadata);

    /**
     * @brief Get default classification level
     * @return Default level
     */
    ClassificationLevel defaultLevel() const { return m_defaultLevel; }

    /**
     * @brief Set default classification level
     * @param level Default level
     */
    void setDefaultLevel(ClassificationLevel level);

Q_SIGNALS:
    /**
     * @brief Emitted when document is classified
     */
    void documentClassified(const QString& filePath, ClassificationLevel level);

    /**
     * @brief Emitted when default level changes
     */
    void defaultLevelChanged(ClassificationLevel level);

private:
    ClassificationLevel m_defaultLevel;
    QMap<QString, ClassificationMetadata> m_classifications;
    
    QString analyzeContent(const QString& content) const;
    ClassificationLevel determineLevel(const QString& content) const;
    QStringList extractKeywords(const QString& content) const;
};

Q_DECLARE_METATYPE(ClassificationLevel)
Q_DECLARE_METATYPE(ClassificationMetadata)

#endif // DOCUMENT_CLASSIFIER_H

