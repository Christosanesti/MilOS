#include <QtQml/qqml.h>
#include <QtQml/qqmlextensionplugin.h>
#include "../components/Button.h"
#include "../components/InputField.h"
#include "../components/StatusIndicator.h"
#include "../components/ProgressBar.h"
#include "../components/TabNavigation.h"
#include "../components/NotificationAlert.h"
#include "../components/Card.h"
#include "../components/DropdownMenu.h"
#include "../components/DataVisualization.h"
#include "../components/ModalDialog.h"

QT_BEGIN_NAMESPACE

class MilosComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(uri == QLatin1String("MilosComponents"));
        
        // Register all components
        qmlRegisterType<Button>(uri, 1, 0, "Button");
        qmlRegisterType<InputField>(uri, 1, 0, "InputField");
        qmlRegisterType<StatusIndicator>(uri, 1, 0, "StatusIndicator");
        qmlRegisterType<ProgressBar>(uri, 1, 0, "ProgressBar");
        qmlRegisterType<TabNavigation>(uri, 1, 0, "TabNavigation");
        qmlRegisterType<NotificationAlert>(uri, 1, 0, "NotificationAlert");
        qmlRegisterType<Card>(uri, 1, 0, "Card");
        qmlRegisterType<DropdownMenu>(uri, 1, 0, "DropdownMenu");
        qmlRegisterType<DataVisualization>(uri, 1, 0, "DataVisualization");
        qmlRegisterType<ModalDialog>(uri, 1, 0, "ModalDialog");
        
        // QML files are auto-registered via qmldir
    }
};

QT_END_NAMESPACE

#include "plugin.moc"

