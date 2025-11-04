#include "networkkillswitch.h"
#include <QtQml/qqml.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqmlextensionplugin.h>

class EmergencyWidgetPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(uri == QLatin1String("org.milos.emergencywidget"));
        qmlRegisterType<NetworkKillSwitch>(uri, 1, 0, "NetworkKillSwitch");
    }
};

#include "emergencywidgetplugin.moc"

