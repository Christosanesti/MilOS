#include "drone_uav_system.h"
#include "drone_controller.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-drone-uav-control");
    app.setOrganizationName("MilOS");
    
    // Create and initialize drone/UAV system
    DroneUAVSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Drone/UAV System" << std::endl;
        return 1;
    }
    
    // Add sample drone
    DroneController* drone1 = new DroneController();
    drone1->initialize("DRONE-001");
    system.addDroneController(drone1);
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("droneSystem", &system);
    engine.rootContext()->setContextProperty("missionPlanner", system.getMissionPlanner());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Drone/UAV Control System running..." << std::endl;
    
    return app.exec();
}

