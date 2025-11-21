cmake_minimum_required(VERSION 3.16)
project(milos-image-generation-service)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt6_standard_project_setup()

set(SOURCES
    src/image_generation_service.cpp
    src/main.cpp
)

set(HEADERS
    include/image_generation_service.h
)

qt6_add_executable(milos-image-generation-service ${SOURCES} ${HEADERS})

target_link_libraries(milos-image-generation-service
    Qt6::Core
    Qt6::DBus
)

# Install
install(TARGETS milos-image-generation-service
    RUNTIME DESTINATION bin
)

# Install config template
install(FILES data/config.json.template
    DESTINATION /etc/milos/image-generation
)

# Install wrapper script
install(FILES src/image_generation_wrapper.js
    DESTINATION /usr/lib/milos
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXEC GROUP_READ GROUP_EXEC WORLD_READ WORLD_EXEC
)

