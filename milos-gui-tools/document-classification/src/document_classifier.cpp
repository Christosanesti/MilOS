#include "document_classifier.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

DocumentClassifier::DocumentClassifier(QObject* parent)
    : QObject(parent)
    , m_defaultLevel(ClassificationLevel::Unclassified)
{
}

DocumentClassifier::~DocumentClassifier() {
}

bool DocumentClassifier::initialize() {
    return true;
}

ClassificationMetadata DocumentClassifier::classifyDocument(const QString& filePath, const QString& content) {
    QString fileContent = content;
    
    // Read file content if not provided
    if (fileContent.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            fileContent = in.readAll();
            file.close();
        }
    }
    
    // Analyze content and determine classification level
    ClassificationLevel level = determineLevel(fileContent);
    QStringList keywords = extractKeywords(fileContent);
    
    ClassificationMetadata metadata;
    metadata.level = level;
    metadata.category = analyzeContent(fileContent);
    metadata.classifiedAt = QDateTime::currentDateTime();
    metadata.classifiedBy = "system";  // In production, would use current user
    // Get handling instructions based on level
    switch (level) {
        case ClassificationLevel::TopSecret:
            metadata.handlingInstructions << "Handle in secure facility only"
                                         << "Requires Top Secret clearance"
                                         << "No electronic transmission"
                                         << "Physical access control required";
            break;
        case ClassificationLevel::Secret:
            metadata.handlingInstructions << "Handle in controlled environment"
                                         << "Requires Secret clearance"
                                         << "Encrypted transmission only"
                                         << "Access logging required";
            break;
        case ClassificationLevel::Confidential:
            metadata.handlingInstructions << "Handle with care"
                                         << "Requires Confidential clearance"
                                         << "Encrypted storage required";
            break;
        default:
            metadata.handlingInstructions << "Standard handling";
            break;
    }
    metadata.customMetadata["keywords"] = keywords;
    metadata.customMetadata["file_path"] = filePath;
    
    // Store classification
    m_classifications[filePath] = metadata;
    
    emit documentClassified(filePath, level);
    
    return metadata;
}

ClassificationLevel DocumentClassifier::getClassificationLevel(const QString& filePath) const {
    if (m_classifications.contains(filePath)) {
        return m_classifications[filePath].level;
    }
    return m_defaultLevel;
}

ClassificationMetadata DocumentClassifier::getClassificationMetadata(const QString& filePath) const {
    return m_classifications.value(filePath, ClassificationMetadata());
}

bool DocumentClassifier::setClassification(const QString& filePath, const ClassificationMetadata& metadata) {
    m_classifications[filePath] = metadata;
    emit documentClassified(filePath, metadata.level);
    return true;
}

void DocumentClassifier::setDefaultLevel(ClassificationLevel level) {
    if (m_defaultLevel != level) {
        m_defaultLevel = level;
        emit defaultLevelChanged(level);
    }
}

QString DocumentClassifier::analyzeContent(const QString& content) const {
    // Simple content analysis
    // In production, would use more sophisticated NLP/AI analysis
    
    QString lowerContent = content.toLower();
    
    // Check for sensitive keywords
    QStringList topSecretKeywords = {"nuclear", "weapon", "classified", "top secret", "compartmented"};
    QStringList secretKeywords = {"military", "operation", "mission", "intelligence"};
    QStringList confidentialKeywords = {"personnel", "financial", "internal"};
    
    for (const QString& keyword : topSecretKeywords) {
        if (lowerContent.contains(keyword)) {
            return "TopSecret";
        }
    }
    
    for (const QString& keyword : secretKeywords) {
        if (lowerContent.contains(keyword)) {
            return "Secret";
        }
    }
    
    for (const QString& keyword : confidentialKeywords) {
        if (lowerContent.contains(keyword)) {
            return "Confidential";
        }
    }
    
    return "General";
}

ClassificationLevel DocumentClassifier::determineLevel(const QString& content) const {
    QString category = analyzeContent(content);
    
    if (category == "TopSecret") {
        return ClassificationLevel::TopSecret;
    } else if (category == "Secret") {
        return ClassificationLevel::Secret;
    } else if (category == "Confidential") {
        return ClassificationLevel::Confidential;
    }
    
    return m_defaultLevel;
}

QStringList DocumentClassifier::extractKeywords(const QString& content) const {
    QStringList keywords;
    
    // Simple keyword extraction
    // In production, would use more sophisticated NLP
    
    QRegularExpression wordRegex("\\b\\w{4,}\\b");
    QRegularExpressionMatchIterator matches = wordRegex.globalMatch(content);
    
    QMap<QString, int> wordCount;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString word = match.captured().toLower();
        wordCount[word]++;
    }
    
    // Get top keywords
    QList<QPair<QString, int>> sortedWords;
    for (auto it = wordCount.begin(); it != wordCount.end(); ++it) {
        sortedWords.append(qMakePair(it.key(), it.value()));
    }
    
    std::sort(sortedWords.begin(), sortedWords.end(), 
              [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                  return a.second > b.second;
              });
    
    for (int i = 0; i < qMin(10, sortedWords.size()); ++i) {
        keywords.append(sortedWords[i].first);
    }
    
    return keywords;
}

