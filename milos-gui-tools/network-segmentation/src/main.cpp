#include "network_segmentation.h"
#include <iostream>

int main(int argc, char* argv[]) {
    NetworkSegmentation app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Network Segmentation" << std::endl;
        return 1;
    }

    return app.run();
}

