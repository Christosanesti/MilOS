#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QFile>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("MilOS Security Setup Wizard");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Register QML types
    qmlRegisterType<MainWindow>("SecuritySetupWizard", 1, 0, "MainWindow");
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
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

