#ifndef LAYOUTTEMPLATEMANAGER_H
#define LAYOUTTEMPLATEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>

/**
 * @brief Layout Template Manager
 * 
 * Manages layout templates for multi-monitor configurations
 */
class LayoutTemplateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList templates READ templates NOTIFY templatesChanged)
    Q_PROPERTY(QString currentTemplate READ currentTemplate NOTIFY currentTemplateChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)

public:
    explicit LayoutTemplateManager(QObject* parent = nullptr);
    ~LayoutTemplateManager();

    QVariantList templates() const { return m_templates; }
    QString currentTemplate() const { return m_currentTemplate; }
    QStringList categories() const { return m_categories; }

    Q_INVOKABLE void loadTemplates();
    Q_INVOKABLE void saveTemplate(const QVariantMap& template);
    Q_INVOKABLE void deleteTemplate(const QString& templateId);
    Q_INVOKABLE void duplicateTemplate(const QString& templateId);
    Q_INVOKABLE QVariantMap getTemplate(const QString& templateId);
    Q_INVOKABLE QVariantMap previewTemplate(const QString& templateId);
    Q_INVOKABLE bool validateTemplate(const QVariantMap& template);
    Q_INVOKABLE void applyTemplate(const QString& templateId);
    Q_INVOKABLE void exportTemplate(const QString& templateId, const QString& filePath);
    Q_INVOKABLE bool importTemplate(const QString& filePath);
    Q_INVOKABLE QVariantList getTemplatesByCategory(const QString& category);

signals:
    void templatesChanged();
    void currentTemplateChanged();
    void categoriesChanged();
    void templateApplied(const QString& templateId);
    void templateExported(const QString& templateId, const QString& filePath);
    void templateImported(const QString& templateId);
    void error(const QString& errorMessage);

private:
    QString getTemplatesDirectory() const;
    QVariantMap loadTemplateFromFile(const QString& filePath);
    bool saveTemplateToFile(const QVariantMap& template, const QString& filePath);
    QString generateTemplateId(const QString& name);

    QVariantList m_templates;
    QString m_currentTemplate;
    QStringList m_categories;
};

#endif // LAYOUTTEMPLATEMANAGER_H

