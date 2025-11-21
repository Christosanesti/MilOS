#ifndef DOCUMENT_CLASSIFICATION_DBUS_INTERFACE_H
#define DOCUMENT_CLASSIFICATION_DBUS_INTERFACE_H

#include "document_classifier.h"
#include "classification_handler.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class DocumentClassifier;
class ClassificationHandler;

/**
 * @brief Document Classification D-Bus Interface
 */
class DocumentClassificationDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.DocumentClassification")

public:
    explicit DocumentClassificationDBusInterface(QObject* parent = nullptr);
    ~DocumentClassificationDBusInterface();

    bool initialize();
    void setClassifier(DocumentClassifier* classifier);
    void setHandler(ClassificationHandler* handler);

public slots:
    QString ClassifyDocument(const QString& filePath);
    QString GetClassificationLevel(const QString& filePath);
    QVariantMap GetClassificationMetadata(const QString& filePath);
    bool SetClassification(const QString& filePath, int level, const QString& category);
    bool IsOperationAllowed(const QString& filePath, const QString& operation);
    QStringList GetHandlingInstructions(const QString& filePath);

private:
    DocumentClassifier* m_classifier;
    ClassificationHandler* m_handler;
};

#endif // DOCUMENT_CLASSIFICATION_DBUS_INTERFACE_H

