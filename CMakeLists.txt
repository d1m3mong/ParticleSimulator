cmake_minimum_required(VERSION 3.20)

project(ParticleSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Raylib custom build
set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(SUPPORT_MODULE_RAUDIO OFF CACHE BOOL "" FORCE)

# Do not contact GitHub again when CMake regenerates an existing build tree.
# Raylib is pinned to the immutable 5.5 tag below, so an automatic update is
# neither necessary nor desirable during an ordinary edit/build/run cycle.
set(FETCHCONTENT_UPDATES_DISCONNECTED_RAYLIB ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

add_executable(ParticleSimulator
    src/main.cpp
)

target_link_libraries(ParticleSimulator PRIVATE raylib)
