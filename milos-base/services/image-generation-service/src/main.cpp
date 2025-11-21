#include "image_generation_service.h"
#include <QCoreApplication>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-image-generation-service");
    app.setOrganizationName("MilOS");
    
    ImageGenerationService service;
    
    if (!service.isAvailable()) {
        std::cerr << "Image Generation Service is not available" << std::endl;
        std::cerr << "Status: " << service.getStatus().toStdString() << std::endl;
        return 1;
    }
    
    std::cout << "MilOS Image Generation Service started" << std::endl;
    std::cout << "Status: " << service.getStatus().toStdString() << std::endl;
    
    return app.exec();
}

