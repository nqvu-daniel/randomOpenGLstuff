cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings and flags
add_compile_options(-Wall -Wno-deprecated -fdiagnostics-color=always -g)

# Setup ImGui as a separate library
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/src/vendor/imgui)

# Get all ImGui source files including the backend implementations
# that are in the same directory
file(GLOB IMGUI_ALL_SOURCE_FILES "${IMGUI_DIR}/*.cpp")

# Create an ImGui library instead of including files directly
add_library(imgui STATIC ${IMGUI_ALL_SOURCE_FILES})
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${CMAKE_SOURCE_DIR}/dependencies/include  # For GLFW headers
)

# Include directories for main project
include_directories(
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/src/vendor
    ${CMAKE_SOURCE_DIR}/src
)

# Link directories
link_directories(${CMAKE_SOURCE_DIR}/dependencies/library)

# Source files for main project
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/glad.c
)

# Add executable
add_executable(app ${SRC_FILES})

# Link against ImGui library and other dependencies
target_link_libraries(app
    imgui
    glfw.3.4.dylib
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework CoreFoundation"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)