cmake_minimum_required(VERSION 3.16)
project(graphics_rendering)

set(CMAKE_CXX_STANDARD 17)

# Find SDL2 via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Source files
file(GLOB SOURCES "src/*.cpp")
add_executable(graphics_rendering ${SOURCES})

# Set up include/link directories + libs
target_include_directories(graphics_rendering PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_directories(graphics_rendering PRIVATE ${SDL2_LIBRARY_DIRS})
target_link_libraries(graphics_rendering ${SDL2_LIBRARIES})

# Optionally define SDL macros if needed
target_compile_definitions(graphics_rendering PRIVATE ${SDL2_CFLAGS_OTHER})

