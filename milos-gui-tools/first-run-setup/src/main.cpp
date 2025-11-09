#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QFile>
#include "ui/mainwindow.h"
#include "ui/firewallconfigurationscreen.h"
#include "ui/updatecheckscreen.h"
#include "services/firewallservice.h"
#include "services/updateservice.h"
#include "services/hardeningservice.h"
#include "services/setupstatemanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("MilOS First-Run Security Setup");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Register QML types
    qmlRegisterType<MainWindow>("FirstRunSetup", 1, 0, "MainWindow");
    qmlRegisterType<FirewallConfigurationScreen>("FirstRunSetup", 1, 0, "FirewallConfigurationScreen");
    qmlRegisterType<UpdateCheckScreen>("FirstRunSetup", 1, 0, "UpdateCheckScreen");
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose services to QML
    FirewallService firewallService;
    UpdateService updateService;
    HardeningService hardeningService;
    SetupStateManager setupStateManager;
    engine.rootContext()->setContextProperty("firewallService", &firewallService);
    engine.rootContext()->setContextProperty("updateService", &updateService);
    engine.rootContext()->setContextProperty("hardeningService", &hardeningService);
    engine.rootContext()->setContextProperty("setupStateManager", &setupStateManager);
    
    // Set QML import paths
    QStringList importPaths = engine.importPathList();
    importPaths.prepend(QDir::currentPath() + "/src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../../milos-kde-theme/libraries/milos-component-library/src/qml");
    engine.setImportPathList(importPaths);
    
    // Load main QML file (development mode - load from source)
    QDir appDir(QCoreApplication::applicationDirPath());
    QString qmlPath = appDir.absoluteFilePath("../src/ui/mainwindow.qml");
    if (!QFile::exists(qmlPath)) {
        // Fallback to current directory
        qmlPath = QDir::currentPath() + "/src/ui/mainwindow.qml";
    }
    
    const QUrl url = QUrl::fromLocalFile(qmlPath);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

