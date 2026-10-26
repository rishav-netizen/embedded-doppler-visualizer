cmake_minimum_required(VERSION 3.16)
project(EmbeddedDopplerVisualizer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include header directory
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(doppler_visualizer ${SOURCES})

# Raylib dependency
find_package(raylib QUIET)
if(raylib_FOUND)
    target_link_libraries(doppler_visualizer PRIVATE raylib)
else()
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(raylib)
    target_link_libraries(doppler_visualizer PRIVATE raylib)
endif()
