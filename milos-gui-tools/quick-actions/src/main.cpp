#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include "quickactionsservice.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("MilOS Quick Actions");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Register QML types
    qmlRegisterType<QuickActionsService>("QuickActions", 1, 0, "QuickActionsService");
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Create service instance
    QuickActionsService service;
    engine.rootContext()->setContextProperty("quickActionsService", &service);
    
    // Load main QML
    engine.load(QUrl("qrc:/ui/main.qml"));
    
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    
    return app.exec();
}

