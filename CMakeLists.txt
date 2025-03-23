cmake_minimum_required(VERSION 3.16)
project(graphics_rendering)

set(CMAKE_CXX_STANDARD 17)

# Include SDL2 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(graphics_rendering ${SOURCES})

# Include project headers
target_include_directories(graphics_rendering PRIVATE include)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(graphics_rendering ${SDL2_LIBRARIES})

# Include Eigen from submodule
include_directories(${CMAKE_SOURCE_DIR}/external/eigen)
