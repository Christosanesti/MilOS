#include "security_status_dashboard.h"
#include <iostream>

int main(int argc, char* argv[]) {
    SecurityStatusDashboard app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Security Status Dashboard" << std::endl;
        return 1;
    }

    return app.run();
}

