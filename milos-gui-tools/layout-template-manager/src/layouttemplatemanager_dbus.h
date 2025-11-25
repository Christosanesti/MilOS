#ifndef LAYOUTTEMPLATEMANAGER_DBUS_H
#define LAYOUTTEMPLATEMANAGER_DBUS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>
#include "layouttemplatemanager.h"
#include "windowpositioner.h"
#include "windowgrouper.h"
#include "xenoncoordinator.h"
#include "layoutrestorer.h"

/**
 * @brief D-Bus Interface for Layout Template Manager
 * 
 * Implements org.milos.LayoutTemplateManager D-Bus interface
 */
class LayoutTemplateManagerDBus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.LayoutTemplateManager")

public:
    explicit LayoutTemplateManagerDBus(LayoutTemplateManager* manager, QObject* parent = nullptr);
    ~LayoutTemplateManagerDBus();

    bool initialize();
    bool start();
    void stop();
    bool isRunning() const { return m_running; }

public Q_SLOTS:
    // D-Bus methods
    QString CreateTemplate(const QVariantMap& templateData);
    bool ApplyTemplate(const QString& templateId);
    bool RestoreLayout();
    QVariantList ListTemplates();
    QVariantMap GetTemplate(const QString& templateId);
    QVariantMap ValidateTemplate(const QString& templateId, const QVariantMap& monitorConfig);

Q_SIGNALS:
    void TemplateApplied(const QString& templateId, const QString& status);
    void TemplateValidationFailed(const QString& templateId, const QStringList& errors);

private:
    bool registerInterface();
    void unregisterInterface();

    LayoutTemplateManager* m_manager;
    WindowPositioner* m_windowPositioner;
    WindowGrouper* m_windowGrouper;
    XenonCoordinator* m_xenonCoordinator;
    LayoutRestorer* m_layoutRestorer;
    bool m_running;
    bool m_initialized;
};

#endif // LAYOUTTEMPLATEMANAGER_DBUS_H


