#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include "qmltypes.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("MilOS Anonymity Controller");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Register QML types
    registerAnonymityControllerTypes();
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Add import paths
    engine.addImportPath("qrc:/");
    engine.addImportPath(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/milos/qml");
    
    // Load main QML
    const QUrl url(QStringLiteral("qrc:/AnonymityController/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

