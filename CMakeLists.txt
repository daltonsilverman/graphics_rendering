cmake_minimum_required(VERSION 3.16)
project(graphics_rendering)

set(CMAKE_CXX_STANDARD 17)

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Find Eigen3 (first try system install)
find_package(Eigen3 QUIET)

# Add source files
file(GLOB SOURCES "src/*.cpp")

add_executable(graphics_rendering ${SOURCES})

# Include your own headers
target_include_directories(graphics_rendering PRIVATE include)

# Include SDL2
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(graphics_rendering ${SDL2_LIBRARIES})

# Handle Eigen include directories
if (NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found via find_package. Falling back to external/eigen3 directory")
    set(EIGEN3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/eigen3")
    target_include_directories(graphics_rendering PRIVATE ${EIGEN3_INCLUDE_DIR})
else()
    target_include_directories(graphics_rendering PRIVATE Eigen3::Eigen)
endif()
