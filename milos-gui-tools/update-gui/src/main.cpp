#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QFile>
#include "update_manager.h"
#include "dbus_interface_wrapper.h"
#include "milos/logging/logger.h"
#include "milos/ui/crash_handler.h"
#include "milos/ui/error_handler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("MilOS Update Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Initialize crash handler FIRST
    CrashHandler::instance()->initialize("milos-update-gui", "1.0.0");
    CrashHandler::instance()->installCrashHandlers();
    
    // Initialize logger
    Logger::instance()->initialize("milos-update-gui");
    
    // Initialize error handler
    ErrorHandler::instance()->initialize("milos-update-gui");
    
    // Create D-Bus interface wrapper
    DBusInterfaceWrapper* dbusInterface = new DBusInterfaceWrapper(&app);
    
    // Create update manager
    UpdateManager* updateManager = new UpdateManager(&app);
    updateManager->setDBusInterface(dbusInterface);
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose error handler and crash handler to QML
    engine.rootContext()->setContextProperty("errorHandler", ErrorHandler::instance());
    engine.rootContext()->setContextProperty("crashHandler", CrashHandler::instance());
    
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


