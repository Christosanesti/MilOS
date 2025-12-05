#include "image_generation_service.h"
#include <milos/logging/logger.h>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-image-generation-service");
    app.setOrganizationName("MilOS");
    
    // Initialize logger
    Logger::instance()->initialize("milos-image-generation-service",
                                   "org.milos.AuditService",
                                   "/org/milos/AuditService",
                                   Logger::Info,
                                   true);
    
    ImageGenerationService service;
    
    if (!service.isAvailable()) {
        LOG_ERROR(QString("Image Generation Service is not available. Status: %1").arg(service.getStatus()));
        return 1;
    }
    
    LOG_INFO(QString("MilOS Image Generation Service started. Status: %1").arg(service.getStatus()));
    
    return app.exec();
}

