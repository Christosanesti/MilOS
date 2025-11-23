#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QFile>
#include "update_manager.h"
#include "dbus_interface_wrapper.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("MilOS Update Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Create D-Bus interface wrapper
    DBusInterfaceWrapper* dbusInterface = new DBusInterfaceWrapper(&app);
    
    // Create update manager
    UpdateManager* updateManager = new UpdateManager(&app);
    updateManager->setDBusInterface(dbusInterface);
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose update manager and D-Bus interface to QML
    engine.rootContext()->setContextProperty("updateManager", updateManager);
    engine.rootContext()->setContextProperty("dbusInterface", dbusInterface);
    
    // Set QML import paths
    QStringList importPaths = engine.importPathList();
    importPaths.prepend(QDir::currentPath() + "/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    engine.setImportPathList(importPaths);
    
    // Load main QML file
    const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

