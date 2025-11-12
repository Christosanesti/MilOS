#include "launcher.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Launcher launcher;
    
    if (!launcher.initialize()) {
        std::cerr << "Failed to initialize BlackArch Launcher" << std::endl;
        return 1;
    }

    return launcher.run();
}

