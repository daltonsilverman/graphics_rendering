cmake_minimum_required(VERSION 3.16)
project(graphics_rendering)

set(CMAKE_CXX_STANDARD 17)

# Add source files
file(GLOB SOURCES "src/*.cpp")
add_executable(graphics_rendering ${SOURCES})

# Include project headers
target_include_directories(graphics_rendering PRIVATE include)

# SDL2 setup using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

include_directories(${SDL2_INCLUDE_DIRS})
target_include_directories(graphics_rendering PRIVATE ${SDL2_INCLUDE_DIRS})

# Link library directories (explicit)
target_link_directories(graphics_rendering PRIVATE ${SDL2_LIBRARY_DIRS})

# Link SDL2 directly by full path (more reliable than -lSDL2)
target_link_libraries(graphics_rendering ${SDL2_LIBRARY_DIRS}/libSDL2.dylib)

# Eigen from submodule
include_directories(${CMAKE_SOURCE_DIR}/external/eigen)
